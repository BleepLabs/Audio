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

    Interpolated delay time gives delay effect with a tape like response.
    Control of this interpolation speed and delay sample rate is now possible

 */

#include "AudioStream.h"
#include "Arduino.h"

class AudioEffectTapeDelay :
  public AudioStream
{
public:
  AudioEffectTapeDelay(void):
    AudioStream(1, inputQueueArray) {
  }

  void begin(short *delayline, uint32_t max_len, uint32_t dly_len, short redux, short lerp);
  uint32_t length(uint32_t dly_len);
    uint32_t length_no_lerp(uint32_t dly_len);

    void sampleRate(short redux);


  virtual void update(void);


private:
  uint32_t dlyd, dlyt;

  audio_block_t *inputQueueArray[1];
  short *l_delayline;
  uint32_t delay_length, desired_delay_length;
  int32_t inv_delay_length;
  uint32_t max_dly_len;
  uint32_t write_head;
  uint32_t delay_depth;
  uint32_t rate_redux;
  uint32_t delay_offset_idx;
  uint32_t   delay_rate_incr;
  uint32_t read_head, feedback;
  short SIMPLE_SMOOTH, lerp_len;
  uint32_t l_delay_rate_index;

  short sync_out_latch;
  short sync_out_count;
};