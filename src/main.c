/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// -- MT3620 SDK ------
#include "CPUFreq.h"
#include "VectorTable.h"


#include "NVIC.h"
#include "UART.h"
#include "Print.h"
// --------------------

#include "dsp/numpy_types.h"
#include "ei_classifier_porting.h"
#include "edge-impulse-sdk/porting/azure_sphere/ei_classifier_porting.cpp" /// meh!
#include "ei_classifier_types.h"

extern uint32_t StackTop; // &StackTop == end of TCM0

UART *uart;
GPT *timer;

EI_IMPULSE_ERROR run_classifier(signal_t *, ei_impulse_result_t *, bool);

static const float features[] = {
// ambient 
// 745, 196, 139, 56, 745, 195, 140, 56, 745, 196, 139, 56, 745, 196, 139, 56, 745, 196, 139, 56, 745, 196, 139, 56, 745, 196, 139, 56, 745, 196, 139, 56, 745, 196, 139, 56, 744, 196, 139, 56, 745, 196, 139, 56, 745, 196, 139, 56
// hechicera
759, 205, 622, 191, 760, 204, 620, 189, 760, 204, 619, 188, 759, 204, 618, 187, 759, 204, 617, 186, 760, 204, 616, 185, 760, 204, 614, 182, 760, 204, 613, 180, 759, 203, 612, 179, 760, 203, 611, 176, 759, 203, 609, 175, 760, 202, 608, 174
};

/**
 * @brief      Copy raw feature data in out_ptr
 *             Function called by inference library
 *
 * @param[in]  offset   The offset
 * @param[in]  length   The length
 * @param      out_ptr  The out pointer
 *
 * @return     0
 */
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr)
{
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

_Noreturn void RTCoreMain(void)
{
    VectorTableInit();
    CPUFreq_Set(26000000);

    uart = UART_Open(MT3620_UNIT_UART_DEBUG, 115200, UART_PARITY_NONE, 1, NULL);

    int32_t error;
    if(timer == NULL) {
        timer = GPT_Open(MT3620_UNIT_GPT3, 1000000, GPT_MODE_NONE);
        if (!timer) {
            ei_printf("ERROR: GPT3 timer init failed\r\n");
        }
        if ((error = GPT_Start_Freerun(timer)) != ERROR_NONE) {
            ei_printf("ERROR: Starting freerun timer (%ld)\r\n",
                        error);
        }
    }


    ei_printf("EdgeImpulse Demo - READY!\r\n");
    ei_printf("App built on: " __DATE__ ", " __TIME__ "\r\n");

    int i = 0;
    while (true)
    {
        ei_impulse_result_t result = {0};

        // the features are stored into flash, and we don't want to load everything into RAM
        signal_t features_signal;
        features_signal.total_length = sizeof(features) / sizeof(features[0]);
        features_signal.get_data = &raw_feature_get_data;
        i++;
        // invoke the impulse
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug y/n */);

        if (res != 0)
            continue;

        ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                  result.timing.dsp, result.timing.classification, result.timing.anomaly);

        // print the predictions
        ei_printf("[");
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
        {
           ei_printf("%.5f", result.classification[ix].value);
#if EI_CLASSIFIER_HAS_ANOMALY == 1
            ei_printf(", ");
#else
            if (ix != EI_CLASSIFIER_LABEL_COUNT - 1)
            {
                ei_printf(", ");
            }
#endif
        }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("%.3f", result.anomaly);
#endif
        ei_printf("]\n\n");

        ei_sleep(1000);
    }

    for (;;) {
        __asm__("wfi");
    }

}
