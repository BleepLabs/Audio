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
 
 todo:
 arbitrary sample rate control rather than just divide
 interpolate between sample mode rather than interpolate time
 arbitraty multi tap
 
 */

#include "effect_tape_delay.h"
#include "arm_math.h"

void AudioEffectTapeDelay::begin(short *delayline,int max_len,int dly_len, short redux, short lerp)
{
  max_dly_len=max_len-1;

  desired_delay_length = dly_len;
  if (desired_delay_length>max_dly_len){
    desired_delay_length=max_dly_len;
  }
  l_delayline = delayline;
  write_head = 0;

rate_redux=redux;
  lerp_len=lerp;
  

}

int16_t AudioEffectTapeDelay::length(int dly_len)
{
  desired_delay_length = dly_len;

  if (desired_delay_length>max_dly_len){
    desired_delay_length=max_dly_len;
  }

  return delay_length;

}


void AudioEffectTapeDelay::update(void)
{
  audio_block_t *block;
  short *bp;
  //static uint32_t preva;
  static short tick,tock;
  int input1;

  if(l_delayline == NULL)return;
  int max_dly_len_m=max_dly_len;

  block = receiveWritable(0);
  if(block) {
    bp = block->data;


    for(int i = 0;i < AUDIO_BLOCK_SAMPLES;i++) {


      tick++;
      if (tick>lerp_len){
      if (delay_length<desired_delay_length-8){
        delay_length++;
      }
      
      if (delay_length>desired_delay_length+8){
        delay_length--;
      }
      tick=0;
      }


      // increment the index into the circular delay line buffer
        tock++;
      if (tock>rate_redux){
        tock=0;
      write_head++;
    }
      // wrap the index around if necessary
      if(write_head >= max_dly_len_m) {

        write_head = 0;
        //digitalWrite(3,tock);

      }



      read_head = ((write_head) + delay_length);

      if (read_head > (max_dly_len_m-1)){
      read_head-=(max_dly_len_m);

      }

      l_delayline[write_head] = (*bp + feedback)/2;
      input1 = *bp;

      l_delayline[write_head] = *bp ;
      input1 = *bp;
      *bp++ = l_delayline[read_head];
      

      feedback = (l_delayline[read_head]+input1)/2;

}
    
    // send the effect output to the left channel
    transmit(block);
    release(block);
  }
}




