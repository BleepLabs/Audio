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

#ifndef analyze_follower_h_
#define analyze_follower_h_

#include "Arduino.h"
#include "AudioStream.h"

class AudioAnalyzeFollower : public AudioStream
{
public:
	AudioAnalyzeFollower(void) : AudioStream(1, inputQueueArray)
	{
	}
	void rate(uint32_t r1) {
		//fr = r1 * 4096.00;
		fr = r1;
	}
	void exp_set(uint16_t es){ 
		exp_div=es;
	}
	int32_t current() {
		return output;
	}

	void amplitude(float amp) {
		amp_div=amp*256;

	}

	int32_t aveo() {
		return max_in;
	}

	virtual void update(void);
private:
#define ave_div (AUDIO_BLOCK_SAMPLES)
	audio_block_t *inputQueueArray[2];
	int32_t min_sample;
	int32_t max_sample;
    int32_t aves[68];
    short ave_cnt,rising;
    const short nave=32;
	int32_t in_ave, prev_in_ave,ave_sum;
	int32_t audio_in, prev_audio_in;
	int32_t min, max, max_in, output;
	int32_t fr, j;
	int32_t exp_div = 4092;
	int32_t amp_div;
};

#endif
