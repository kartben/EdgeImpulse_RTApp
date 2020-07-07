/* Edge Impulse inferencing library
 * Copyright (c) 2020 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

// #include "CPUFreq.h"
// #include "NVIC.h"
// #include "UART.h"
// #include "Print.h"

// #include "mt3620/gpt.h"
// #include "GPT.h"

#ifdef __cplusplus
}
#endif

#include "../ei_classifier_porting.h"

#define EI_WEAK_FN __attribute__((weak))

EI_WEAK_FN EI_IMPULSE_ERROR ei_run_impulse_check_canceled()
{
    return EI_IMPULSE_OK;
}

EI_WEAK_FN EI_IMPULSE_ERROR ei_sleep(int32_t time_ms)
{
    return EI_IMPULSE_OK;
}

uint64_t ei_read_timer_ms()
{
    return 0;
}

uint64_t ei_read_timer_us()
{
    return 0;
}

__attribute__((weak)) void ei_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    //UART_vPrintf(uart, format, args);
    va_end(args);
}

__attribute__((weak)) void ei_printf_float(float f)
{
    ei_printf("%f", f);
}

__attribute__((weak)) void DebugLog(const char *s)
{
    ei_printf("%s", s);
}
