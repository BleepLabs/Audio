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

#include "sampler.h"
#include "arm_math.h"
#include "utility/dspinst.h"



void AudioSampler::update(void)
{
  audio_block_t *ab_left;
  audio_block_t *ab_right;

  short *bpl, *bpr, *end;
  int32_t val1, val2, val3;
  short tempf;


  if (tone_amp == 0 || playing == 0) return;
  ab_left = allocate();
  ab_right = allocate();

  if (ab_left) {
    bpl = ab_left->data;
    bpr = ab_right->data;


    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

      if (rev == 0) {

        if (playing == 0) {
          *bpl++ = (short)(0);
          *bpr++ = (short)(0);
        }

        if (playing == 1) {
          pindex = index;
          index = tone_phase >> 23; //23 is 256 //changing causes rate reduction

          if (pindex > 128 && index < 128) { //did it wrap around
            fpos += 256; // this is fixed its the max size of index
            get = 1;
          }

          if (fpos + index > sample_len ) {
            if (looping == 1) {
              fpos = mod_start_pos;
              index = 0;
              tone_phase = 0;
              get = 1;
              loop_trig1 = !loop_trig1;
            }
            if (looping == 0) {
              playing = 0;
            }

          }

          if (fpos + index > mod_start_pos + end_mod && end_mod_en == 1) {
            fpos = mod_start_pos;
            index = 0;
            tone_phase = 0;
            get = 0;
            bankshift = 0;
            bankoff = 0;
            if (looping == 1) {
              loop_trig1 = !loop_trig1;
            }
            if (looping == 0) {
              playing = 0;
            }
          }

          if (get == 1) {
            get = 0;
            //gfd = micros() - gfcm;
            //gfcm = micros();
            uint32_t actualget = start_pos + (fpos * 4);

            //if (SerialFlash.ready() == 1) {   //seems to always be ready untill its crashed
           //   AudioStartUsingSPI();
             SerialFlash.read(actualget, in8, buffrl);
           //   AudioStopUsingSPI();
            

            js = 0;
            for (uint16_t  j = 0; j < buffrl - 4; j += 4) {
              sample_left_A[js] = (in8[j + 1] << 8) | (in8[j]);
              sample_right_A[js] = (in8[j + 3] << 8) | (in8[j + 2]);
              js++;
            }

          }

          scale = (tone_phase >> 7) & 0xFFFF;

          val1 = *(sample_left_A + index );
          val2 = *(sample_left_A + (index + 1) );
          val2 *= scale;
          val1 *= 0xFFFF - scale;
          val3 = (val1 + val2) >> 16;
          *bpl++ = (short)((val3 * tone_amp) >> 15);

          val1 = *(sample_right_A + index );
          val2 = *(sample_right_A + (index + 1) );
          val2 *= scale;
          val1 *= 0xFFFF - scale;
          val3 = (val1 + val2) >> 16;
          *bpr++ = (short)((val3 * tone_amp) >> 15);

          tone_phase += tone_incr;
          tone_phase &= 0x7fffffff;


        }
      }


      if (rev == 1) {


        if (playing == 0) {
          *bpl++ = (short)(0);
          *bpr++ = (short)(0);
        }

        if (playing == 1) {
          pindex = index;
          index = tone_phase >> 23;
          revindex = (index - 255) * -1;
          bankoff = 0;

          if (pindex > 128 && index < 128) {
            fpos -= (256);

            get = 1;


          }

          if (fpos < 0) {
            fpos = sample_len;
            index = 0;
            tone_phase = 0;
            get = 1;
            //Serial.println("end");
            if (looping == 0) {
              playing = 0;
            }
            if (looping == 1) {
              loop_trig1 = !loop_trig1;
            }

          }

          if (get == 1) {
            get = 0;
            // uint32_t tt = micros();
            uint32_t rev_sactualget = rev_start + (fpos * 4);
            //Serial.print("Rag ");
            // Serial.println(rev_sactualget);
          //  if (SerialFlash.ready() == 1) {
              AudioStartUsingSPI();
              SerialFlash.read(rev_sactualget, in8, buffrl);
              AudioStopUsingSPI();
            

            js = 0;
            for (uint16_t  j = 0; j < buffrl - 4; j += 4) {
              sample_left_A[js] = (in8[j + 1] << 8) | (in8[j]);
              sample_right_A[js] = (in8[j + 3] << 8) | (in8[j + 2]);
              js++;
            }

          }

          scale = (tone_phase >> 7) & 0xFFFF;

          val1 = *(sample_left_A + revindex + 1 + bankoff);
          val2 = *(sample_left_A + (revindex ) + bankoff);
          val2 *= scale;
          val1 *= 0xFFFF - scale;
          val3 = (val1 + val2) >> 16;
          *bpl++ = (short)((val3 * tone_amp) >> 15);

          val1 = *(sample_right_A + revindex + 1 + bankoff);
          val2 = *(sample_right_A + revindex + bankoff);
          val2 *= scale;
          val1 *= 0xFFFF - scale;
          val3 = (val1 + val2) >> 16;
          *bpr++ = (short)((val3 * tone_amp) >> 15);

          tone_phase += tone_incr;
          tone_phase &= 0x7fffffff;

        }
      }
    }

    transmit(ab_left, 0);
    transmit(ab_right, 1);

    release(ab_left);
    release(ab_right);

  }
}




