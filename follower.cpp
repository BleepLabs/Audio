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

#include "follower.h"

void AudioAnalyzeFollower::update(void)
{
	//audio_block_t *audio_in_block;
	audio_block_t *in_block, *env_out_block;
	int16_t *p;

	in_block = receiveReadOnly();

	if (!in_block) { // doesent matter needs to keep calculating the output
		//	return;
	}


	env_out_block = allocate();

	if (env_out_block) {
		p = env_out_block->data;
		prev_in_ave = max_in;

		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
			if (in_block) {
				audio_in = abs(in_block->data[i]);
			}

			else {
				audio_in = 0;
			}
/*
			ave_cnt++;
			if (ave_cnt > nave - 1) {
				ave_cnt = 0;
			}
			aves[ave_cnt] = audio_in;
			ave_sum = 0;

			for (int i = 0; i < nave; ++i) {
				ave_sum += aves[i] >> 5;
			}
			//output = ave_sum;
			
*/
			rising = 0;
			if (audio_in > output) {
				//output = ave_sum;
				output = audio_in ;
				rising = 1;
			}

			if (output > 0 && rising == 0) {
				j++;
				if (j > fr) {   //a accumulator would be better but this works
					j = 0;
					output = (output * exp_div) >> 12;
				}
			}

			if (output < 2) {
				output = 0;
			}

			int32_t fout= ((output * amp_div) >> 8) - 32767; //so it can use the DACs output with the full range
			//prob could just do this with input gain but this was the way i did it for some reason???
			if (fout > 32767) {
				fout = 32767;
			}

			*p++ = fout; 


		}
		transmit(env_out_block, 0);
		//release(audio_in_block);
		if (in_block) {
			release(in_block);
		}

		release(env_out_block);

	}
}



