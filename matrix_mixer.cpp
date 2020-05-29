/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Arduino.h>
#include "matrix_mixer.h"
#include "utility/dspinst.h"


#define MULTI_UNITYGAIN 65536

static void MMapplyGain(int16_t *data, int32_t mult)
{
	uint32_t *p = (uint32_t *)data;
	const uint32_t *end = (uint32_t *)(data + AUDIO_BLOCK_SAMPLES);

	do {
		uint32_t tmp32 = *p; // read 2 samples from *data
		int32_t val1 = signed_multiply_32x16b(mult, tmp32);
		int32_t val2 = signed_multiply_32x16t(mult, tmp32);
		val1 = signed_saturate_rshift(val1, 16, 0);
		val2 = signed_saturate_rshift(val2, 16, 0);
		*p++ = pack_16b_16b(val2, val1);
	} while (p < end);
}

static void MMapplyGainThenAdd(int16_t *data, const int16_t *in, int32_t mult)
{
	uint32_t *dst = (uint32_t *)data;
	const uint32_t *src = (uint32_t *)in;
	const uint32_t *end = (uint32_t *)(data + AUDIO_BLOCK_SAMPLES);

	if (mult == MULTI_UNITYGAIN) {
		do {
			uint32_t tmp32 = *dst;
			*dst++ = signed_add_16_and_16(tmp32, *src++);
			tmp32 = *dst;
			*dst++ = signed_add_16_and_16(tmp32, *src++);
		} while (dst < end);
	} else {
		do {
			uint32_t tmp32 = *src++; // read 2 samples from *data
			int32_t val1 = signed_multiply_32x16b(mult, tmp32);
			int32_t val2 = signed_multiply_32x16t(mult, tmp32);
			val1 = signed_saturate_rshift(val1, 16, 0);
			val2 = signed_saturate_rshift(val2, 16, 0);
			tmp32 = pack_16b_16b(val2, val1);
			uint32_t tmp32b = *dst;
			*dst++ = signed_add_16_and_16(tmp32, tmp32b);
		} while (dst < end);
	}
}

static void simpleMix(int16_t *data, const int16_t *in, int32_t mult)
{
	if (data) {
		uint32_t *dst = (uint32_t *)data;

		const uint32_t *src = (uint32_t *)in;
		const uint32_t *end = (uint32_t *)(data + AUDIO_BLOCK_SAMPLES);

		if (mult == MULTI_UNITYGAIN) {
			do {
				uint32_t tmp32 = *dst;
				*dst++ = signed_add_16_and_16(tmp32, *src++);
				tmp32 = *dst;
				*dst++ = signed_add_16_and_16(tmp32, *src++);
			} while (dst < end);
		}
		else {
			do {
				uint32_t tmp32 = *src++; // read 2 samples from *data
				int32_t val1 = signed_multiply_32x16b(mult, tmp32);
				int32_t val2 = signed_multiply_32x16t(mult, tmp32);
				val1 = signed_saturate_rshift(val1, 16, 0);
				val2 = signed_saturate_rshift(val2, 16, 0);
				tmp32 = pack_16b_16b(val2, val1);
				uint32_t tmp32b = *dst;
				*dst++ = signed_add_16_and_16(tmp32, tmp32b);
			} while (dst < end);
		}
	}
}


void AudioMatrixMixer::update(void)
{
	audio_block_t  *ins[4], *outs[4];
	int16_t *p[4], ti[4];
	int16_t *po, *end;
	int16_t sum = 0;

	for (byte i = 0; i < 4; i++) {
		ins[i] =  receiveReadOnly(i);
		outs[i] = allocate();
		p[i] = (ins[i]->data);
	}

	for (byte out_sel = 0; out_sel < 4; out_sel++) {

		po = (outs[out_sel]->data);
		end = po + AUDIO_BLOCK_SAMPLES;

		while (po < end) {
			sum = 0;
			//for (byte j = 0; j < 4; j++) {
				ti[out_sel] = *p[out_sel]++;
//				sum += (ti[j] * (multiplier[out_sel][j])) >> 8; //multiplier value is 0-255
				sum=ti[out_sel];
			//}
			*po++ = sum;
		}
	}

	for (byte i = 0; i < 4; i++) {
		if (ins[i]) {
			release(ins[i]);
		}
		if (outs[i]) {
			transmit(outs[i], i);
			release(outs[i]);
		}
	}
}




