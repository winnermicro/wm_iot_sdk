/* ---------------------------------------------------------------------------
 * Copyright (C) 2016 CSKY Limited. All rights reserved.
 *
 * Redistribution and use of this software in source and binary forms,
 * with or without modification, are permitted provided that the following
 * conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *   * Neither the name of CSKY Ltd. nor the names of CSKY's contributors may
 *     be used to endorse or promote products derived from this software without
 *     specific prior written permission of CSKY Ltd.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------------- */

#ifndef TEST_PERF_H
#define TEST_PERF_H

extern void init_testsuite(unsigned int cycle);
extern int  done_testing(void);

unsigned int tick_cycle;
unsigned int tick_count;
unsigned int current_cycle;
unsigned long long total_cycle;
volatile unsigned int current_csr;

int coretimer_irq_handler(void)
{
	current_csr = CORET->CTRL;
	tick_count++;
}

int done_testing(void)
{
	unsigned int current_count;
	current_cycle = CORET->VAL;
	current_count = tick_count;

	total_cycle = (unsigned long long)current_count * tick_cycle + tick_cycle - current_cycle;

	//	NVIC_DisableIRQ(CORET_IRQn);
//	asm("bkpt");
	return 0;
}

void init_testsuite(unsigned int cycle)
{
	tick_cycle = cycle;
	if (tick_cycle != 0) {
		CORET_Config(tick_cycle);
		NVIC_EnableIRQ(CORET_IRQn);
		__enable_excp_irq();
	}
}


#endif	/* TEST_PERF_H */
