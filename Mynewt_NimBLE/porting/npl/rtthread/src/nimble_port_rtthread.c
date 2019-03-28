/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2018-12-18     ChenYong     first implementation
 */


#include <stddef.h>
#include <rtthread.h>
#include <rthw.h>
#include "syscfg/syscfg.h"
#include "nimble/nimble_port.h"

#define NIMBLE_THRAED_STACK_SIZE (1024)
#define NIMBLE_THREAD_PRIORITY (10)

#if NIMBLE_CFG_CONTROLLER
static rt_thread_t ll_task_h;
#endif
static rt_thread_t host_task_h;

extern void ble_ll_task(void *arg);

RT_WEAK void __aeabi_assert(const char *expr, const char *file, int line)
{
#ifdef RT_DEBUG
    rt_assert_handler(expr, file, line);
#endif    
}

int nimble_port_rtthread_init(void)
{
    nimble_port_init();

#if NIMBLE_CFG_CONTROLLER
    /*
     * Create task where NimBLE LL will run. This one is required as LL has its
     * own event queue and should have highest priority. The task function is
     * provided by NimBLE and in case of FreeRTOS it does not need to be wrapped
     * since it has compatible prototype.
     */
    ll_task_h = rt_thread_create("ll", ble_ll_task, NULL, NIMBLE_THRAED_STACK_SIZE + 256, NIMBLE_THREAD_PRIORITY - 1, 10);
    RT_ASSERT(ll_task_h != RT_NULL);
    if (ll_task_h != RT_NULL)
        rt_thread_startup(ll_task_h);

#endif
}

void ble_hs_thread_entry(void *parameter)
{
    nimble_port_run();
}

void ble_hs_thread_startup(void)
{
    host_task_h = rt_thread_create("host", ble_hs_thread_entry, NULL, NIMBLE_THRAED_STACK_SIZE, NIMBLE_THREAD_PRIORITY + 1, 10);
    RT_ASSERT(host_task_h != RT_NULL);
    if (host_task_h != RT_NULL)
        rt_thread_startup(host_task_h);
}

INIT_COMPONENT_EXPORT(nimble_port_rtthread_init);
