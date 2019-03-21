/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nrf.h"
#include "nrfx_glue.h"

#include <rtthread.h>
#include <rthw.h>

#define NUM_IRQS_PER_REG 32
#define REG_FROM_IRQ(irq) (irq / NUM_IRQS_PER_REG)
#define BIT_FROM_IRQ(irq) (irq % NUM_IRQS_PER_REG)

/**
 * @brief Return IRQ enable state
 *
 * @param irq IRQ line
 * @return interrupt enable state, true or false
 */
int NVIC_IRQ_IS_ENABLED(unsigned int irq)
{
	return NVIC->ISER[REG_FROM_IRQ(irq)] & (1 << BIT_FROM_IRQ(irq));
}

unsigned int nrfx_enter_critical(void)
{
	return rt_hw_interrupt_disable();
}

void nrfx_exit_critical(unsigned int ctx)
{
	rt_hw_interrupt_enable(ctx);
}