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

class AudioEffectTapeDelay : 
public AudioStream
{
public:
  AudioEffectTapeDelay(void): 
  AudioStream(1,inputQueueArray) { 
  }

  void begin(short *delayline,int max_len,int dly_len, short redux, short lerp);
  int16_t length(int dly_len);
  virtual void update(void);
  
private:
    uint32_t dlyd,dlyt;    

  audio_block_t *inputQueueArray[1];
  short *l_delayline;
  int delay_length,desired_delay_length;
  int16_t max_dly_len;
  short write_head;
  int delay_depth;
  int rate_redux;
  int delay_offset_idx;
  int   delay_rate_incr;
  int read_head,feedback;
  short SIMPLE_SMOOTH,lerp_len;
  unsigned int l_delay_rate_index;
};
