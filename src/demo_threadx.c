#include "stdio.h" // make sure to include stdio.h first so that printf() etc.
                   // can be overriden by printf.h below

#include "tx_api.h"
#include "printf.h"

#include "malloc_threadx.h"

#include <stdbool.h>
#include "dsp/config.hpp"
#include "numpy_types.h"
#include "ei_classifier_porting.h"
#include "ei_classifier_types.h"
#include "model-parameters/model_metadata.h"

#define DEMO_STACK_SIZE 1024

float sensor_readings[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
char inference_result[10];

/* Define the ThreadX object control blocks...  */

TX_THREAD thread_0; // logging thread
TX_THREAD thread_1; // edge impulse inference
TX_TIMER timer_0;   // data acquisition timer (simulated)
TX_MUTEX mutex_0;

ULONG thread_1_counter;
ULONG thread_1_messages_received;
ULONG timer_0_counter;

/* Define thread prototypes.  */

void thread_0_entry(ULONG thread_input);
void thread_1_entry(ULONG thread_input);
void timer_0_handler(ULONG id);

EI_IMPULSE_ERROR run_classifier(signal_t *, ei_impulse_result_t *, bool);

/* Define main entry point.  */

int main(void)
{
    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

/* Define what the initial system looks like.  */

void tx_application_define(void *first_unused_memory)
{

    CHAR *pointer;

    initMemoryPool();

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Allocate the stack for thread 0.  */
    tx_byte_allocate(&byte_pool, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,
                     pointer, DEMO_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 1.  */
    tx_byte_allocate(&byte_pool, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create Edge Impulse thread  */
    tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,
                     pointer, DEMO_STACK_SIZE,
                     16, 16, 4, TX_AUTO_START);

    /* Allocate the stack for thread 2.  */
    tx_byte_allocate(&byte_pool, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Start a timer to read sensor data at EI_CLASSIFIER_FREQUENCY Hz */
    tx_timer_create(&timer_0, "sampling_timer", timer_0_handler, 0xBEEF, EI_CLASSIFIER_INTERVAL_MS, EI_CLASSIFIER_INTERVAL_MS, TX_AUTO_ACTIVATE);

    tx_mutex_create(&mutex_0, "mutex sensor readings", TX_NO_INHERIT);
}

/* Define the test threads.  */

void thread_0_entry(ULONG thread_input)
{

    /* This thread simply sits in while-forever-sleep loop and regularly outputs debug information.  */
    while (1)
    {
        /* Print results regularly.  */
        printf("\n\n**** Edge Impulse + Azure RTOS Demonstration **** (c) 2006-2020 [build: " __DATE__ ", " __TIME__ "]\n\n");
        printf("           thread 1 || # of inferences:        %lu\n", thread_1_counter);
        printf("           thread 1 || latest inference:       %s\n", inference_result);
        printf("           timer 0  || # of sensor readings:   %lu\n", timer_0_counter);

        /* Sleep for 1s.  */
        tx_thread_sleep(1000);
    }
}

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr)
{
    /* Note: `sensor_readings` is effectively a circular buffer but the implementation needs to be completed
       so that the copy below takes into account the actual chronological order of the samples */
    tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
    memcpy(out_ptr, sensor_readings + offset, length * sizeof(float));
    tx_mutex_put(&mutex_0);
    return 0;
}

void thread_1_entry(ULONG thread_input)
{
    UINT status;

    /* This thread performs Edge Impulse inference indefinitely, using sensor data read by sampling timer */
    while (1)
    {
        thread_1_counter++;

        ei_impulse_result_t result = {0};

        signal_t features_signal;
        features_signal.total_length = sizeof(sensor_readings) / sizeof(sensor_readings[0]);
        features_signal.get_data = &raw_feature_get_data;
        // invoke the impulse...
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, true /* debug y/n */);
        // and pretend it is taking a very long time...
        // In the meantime, the sampling timer goes on with its life and ticks like a Swiss watch :)
        ei_sleep(200 + tx_time_get() % 500);

        if (res == EI_IMPULSE_OK)
        {
            float bestScore = 0;
            int bestIx = 0;

            for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
            {
                if (result.classification[ix].value > bestScore)
                {
                    bestScore = result.classification[ix].value;
                    bestIx = ix;
                }
                strcpy(inference_result, result.classification[bestIx].label);
            }
        }
    }
}

/**
 * This thread simulates data sampling 
 */
void timer_0_handler(ULONG id)
{
    UINT status;

    tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
    /* Generate random sensor readings.
       In a real-world scenario this is where you would read actual data from your sensor(s) */
    sensor_readings[timer_0_counter % EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = tx_time_get() % 1024;
    tx_mutex_put(&mutex_0);
    timer_0_counter++;
}
