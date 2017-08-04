
#include "synth_waveform.h"
#include "arm_math.h"
#include "utility/dspinst.h"
#include "Arduino.h"


void AudioSynthWaveform::update(void)
{
  audio_block_t *block, *fm_input, *shape_input;
  short *bp, *end;
  int32_t val1, val2, val3;
  uint32_t index, index2, scale;
  int16_t mod;

  // temporaries for TRIANGLE
  uint32_t mag;
  short tmp_amp;

  fm_input = receiveReadOnly(0);
  shape_input = receiveReadOnly(1);


  if (tone_amp == 0) return;
  block = allocate();
  if (block) {
    bp = block->data;
    switch (tone_type) {

    case WAVEFORM_SINE:

      if (shape_input) {
        release(shape_input);
      }

      if (fm_input) {

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

          // Calculate interpolated sin
          index = tone_phase >> 23;
          val1 = AudioWaveformSine[index];
          val2 = AudioWaveformSine[index + 1];
          scale = (tone_phase >> 7) & 0xFFFF;
          val2 *= scale;
          val1 *= 0xFFFF - scale;
          val3 = (val1 + val2) >> 16;
          *bp++ = (short)((val3 * tone_amp) >> 15);

          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);

          //tone_phase += tone_incr;
          if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
        }
        release(fm_input);
      }
      else {

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

          // Calculate interpolated sin
          index = tone_phase >> 23;
          val1 = AudioWaveformSine[index];
          val2 = AudioWaveformSine[index + 1];
          scale = (tone_phase >> 7) & 0xFFFF;
          val2 *= scale;
          val1 *= 0xFFFF - scale;
          val3 = (val1 + val2) >> 16;
          *bp++ = (short)((val3 * tone_amp) >> 15);

          tone_phase += tone_incr;

          if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
        }

      }

      break;

    case WAVEFORM_ARBITRARY:

      if (shape_input) {
        release(shape_input);
      }

      if (!arbdata) {
        release(block);
        if (fm_input) {
          release(fm_input);
        }
        return;
      }

      if (arb_len > 255)
      {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

          index = tone_phase >> 20; //2048 max
          if (index > arb_len) {
            index = 0;
            tone_phase = 0;
          }
          //index %=arb_len-1;
          val1 = *(arbdata + index);
          int16_t ind2 = (index + 1);
          if (ind2 > arb_len - 1)
          {
            ind2 = 0;
          }
          val2 = *(arbdata + ind2);
          scale = (tone_phase >> 7) & 0xFFFF;
          val2 *= scale;
          val1 *= 0xFFFF - scale;
          val3 = (val1 + val2) >> 16;
          *bp++ = (short)((val3 * tone_amp) >> 15);

          if (fm_input) {
            mod = fm_input->data[i];
            tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
          }
          else {
            tone_phase += tone_incr;
          }

          tone_phase &= 0x7fffffff;
        }
      }


      if (arb_len == 255)
      {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

          index = tone_phase >> 23;
          val1 = *(arbdata + index);
          val2 = *(arbdata + ((index + 1) & 255));
          scale = (tone_phase >> 7) & 0xFFFF;
          val2 *= scale;
          val1 *= 0xFFFF - scale;
          val3 = (val1 + val2) >> 16;
          *bp++ = (short)((val3 * tone_amp) >> 15);

          if (fm_input) {
            mod = fm_input->data[i];
            tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
          }
          else {
            tone_phase += tone_incr;
          }
          //tone_phase += tone_incr;
          tone_phase &= 0x7fffffff;
        }
      }

      if (fm_input) {
        release(fm_input);
      }


      break;

    case WAVEFORM_SQUARE:

      if (shape_input) {
        release(shape_input);
      }

      if (fm_input) {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
          if (tone_phase & 0x40000000)*bp++ = -tone_amp;
          else *bp++ = tone_amp;
          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
          //  tone_phase += tone_incr;

          if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;

        }
        release(fm_input);
      }

      else {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
          if (tone_phase & 0x40000000)*bp++ = -tone_amp;
          else *bp++ = tone_amp;
          // phase and incr are both unsigned 32-bit fractions
          tone_phase += tone_incr;

        }
      }

      break;

    case WAVEFORM_SAWTOOTH:

      if (shape_input) {
        release(shape_input);
      }

      if (fm_input) {

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
          *bp++ = ((short)(tone_phase >> 15) * tone_amp) >> 15;
          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        release(fm_input);
      }

      else {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
          *bp++ = ((short)(tone_phase >> 15) * tone_amp) >> 15;
          tone_phase += tone_incr;
        }
      }
      break;

    case WAVEFORM_SAWTOOTH_REVERSE:
      if (shape_input) {
        release(shape_input);
      }

      if (fm_input) {

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
          *bp++ = ((short)(tone_phase >> 15) * tone_amp) >> 15;
          mod = fm_input->data[i];
          tone_phase -= tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        release(fm_input);
      }

      else {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
          *bp++ = ((short)(tone_phase >> 15) * tone_amp) >> 15;
          tone_phase -= tone_incr;
        }
      }
      break;


    case WAVEFORM_VARIABLE_TRIANGLE:

      static int32_t pindex;

      if (fm_input) {
        if (shape_input) {
          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

            int32_t shape_mod = shape_input->data[i];
            int32_t mod_knee = knee + (shape_mod >> 8);


            if (mod_knee < 0) {
              mod_knee = 0;
            }
            if (mod_knee > 255) {
              mod_knee = 255;
            }

            index = tone_phase >> 23;

            index2 = index + 1;
            if (index2 > 255) {
              index2 = 0;
            }

            if (index < mod_knee) {
              vtout = ((index * waveamp) / mod_knee) - waveamp / 2;
            }
            if (index >= mod_knee) {
              vtout = ((((index - mod_knee) * waveamp) / (wavelength - mod_knee)) * -1) + waveamp / 2;
              //vtout = 0;
            }

            if (index2 < mod_knee) {
              vtout2 = ((index2 * waveamp) / mod_knee) - waveamp / 2;
            }
            if (index2 >= mod_knee) {
              vtout2 = ((((index2 - mod_knee) * waveamp) / (wavelength - mod_knee)) * -1) + waveamp / 2;
              //vtout = 0;
            }

            val1 = vtout;
            val2 = vtout2;
            scale = (tone_phase >> 7) & 0xFFFF;
            val2 *= scale;
            val1 *= 0xFFFF - scale;
            val3 = (val1 + val2) >> 16;

            *bp++ = (short)((val3 * tone_amp) >> 15);

            mod = fm_input->data[i];

            tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);

            // If tone_phase has overflowed, truncate the top bit
            if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
          }
          release(fm_input);
          release(shape_input);
        }

        else {
          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

            index = tone_phase >> 23;

            index2 = index + 1;
            if (index2 > 255) {
              index2 = 0;
            }

            if (index < knee) {
              vtout = ((index * waveamp) / knee) - waveamp / 2;
            }
            if (index >= knee) {
              vtout = ((((index - knee) * waveamp) / (wavelength - knee)) * -1) + waveamp / 2;
              //vtout = 0;
            }

            if (index2 < knee) {
              vtout2 = ((index2 * waveamp) / knee) - waveamp / 2;
            }
            if (index2 >= knee) {
              vtout2 = ((((index2 - knee) * waveamp) / (wavelength - knee)) * -1) + waveamp / 2;
              //vtout = 0;
            }

            val1 = vtout;
            val2 = vtout2;
            scale = (tone_phase >> 7) & 0xFFFF;
            val2 *= scale;
            val1 *= 0xFFFF - scale;
            val3 = (val1 + val2) >> 16;

            *bp++ = (short)((val3 * tone_amp) >> 15);

            mod = fm_input->data[i];
            tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
            // If tone_phase has overflowed, truncate the top bit
            if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
          }
          release(fm_input);
          release(shape_input);
        }
      }

      else {
        if (shape_input) {
          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

            int32_t shape_mod = shape_input->data[i];
            knee += shape_mod >> 8;

            if (knee < 0) {
              knee = 0;
            }
            if (knee > 255) {
              knee = 255;
            }

            index = tone_phase >> 23;

            index2 = index + 1;
            if (index2 > 255) {
              index2 = 0;
            }

            if (index < knee) {
              vtout = ((index * waveamp) / knee) - waveamp / 2;
            }
            if (index >= knee) {
              vtout = ((((index - knee) * waveamp) / (wavelength - knee)) * -1) + waveamp / 2;
              //vtout = 0;
            }

            if (index2 < knee) {
              vtout2 = ((index2 * waveamp) / knee) - waveamp / 2;
            }
            if (index2 >= knee) {
              vtout2 = ((((index2 - knee) * waveamp) / (wavelength - knee)) * -1) + waveamp / 2;
              //vtout = 0;
            }

            val1 = vtout;
            val2 = vtout2;
            scale = (tone_phase >> 7) & 0xFFFF;
            val2 *= scale;
            val1 *= 0xFFFF - scale;
            val3 = (val1 + val2) >> 16;

            *bp++ = (short)((val3 * tone_amp) >> 15);
            tone_phase += tone_incr;

            // If tone_phase has overflowed, truncate the top bit
            if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
          }
          release(shape_input);
        }

        else {
          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

            index = tone_phase >> 23;

            index2 = index + 1;
            if (index2 > 255) {
              index2 = 0;
            }

            if (index < knee) {
              vtout = ((index * waveamp) / knee) - waveamp / 2;
            }
            if (index >= knee) {
              vtout = ((((index - knee) * waveamp) / (wavelength - knee)) * -1) + waveamp / 2;
              //vtout = 0;
            }

            if (index2 < knee) {
              vtout2 = ((index2 * waveamp) / knee) - waveamp / 2;
            }
            if (index2 >= knee) {
              vtout2 = ((((index2 - knee) * waveamp) / (wavelength - knee)) * -1) + waveamp / 2;
              //vtout = 0;
            }

            val1 = vtout;
            val2 = vtout2;
            scale = (tone_phase >> 7) & 0xFFFF;
            val2 *= scale;
            val1 *= 0xFFFF - scale;
            val3 = (val1 + val2) >> 16;

            *bp++ = (short)((val3 * tone_amp) >> 15);

            tone_phase += tone_incr;
            // If tone_phase has overflowed, truncate the top bit
            if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
          }
          release(fm_input);
          release(shape_input);
        }
      }

      break;

    case WAVEFORM_TRIANGLE:

      if (shape_input) {
        release(shape_input);
      }

      if (fm_input) {

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
          if (tone_phase & 0x80000000) {
            // negative half-cycle
            tmp_amp = -tone_amp;
          }
          else {
            // positive half-cycle
            tmp_amp = tone_amp;
          }
          mag = tone_phase << 2;
          // Determine which quadrant
          if (tone_phase & 0x40000000) {
            // negate the magnitude
            mag = ~mag + 1;
          }
          *bp++ = ((short)(mag >> 17) * tmp_amp) >> 15;

          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);

          //tone_phase += tone_incr;


          if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
        }

        release(fm_input);
      }
      else {

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
          if (tone_phase & 0x80000000) {
            // negative half-cycle
            tmp_amp = -tone_amp;
          }
          else {
            // positive half-cycle
            tmp_amp = tone_amp;
          }
          mag = tone_phase << 2;
          // Determine which quadrant
          if (tone_phase & 0x40000000) {
            // negate the magnitude
            mag = ~mag + 1;
          }
          *bp++ = ((short)(mag >> 17) * tmp_amp) >> 15;


          tone_phase += tone_incr;


          if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
        }
      }


      break;


    case WAVEFORM_PULSE:
      if (fm_input) {
        if (shape_input) {

          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

            int32_t shape_mod = shape_input->data[i];
            tone_width += shape_mod;
            if (tone_width < 1) {
              tone_width = 1;
            }

            if (tone_width > 0x7ffffffe) {
              tone_width = 0x7ffffffe;
            }

            if (tone_phase < tone_width)*bp++ = -tone_amp;
            else *bp++ = tone_amp;

            mod = fm_input->data[i];
            tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);


            //tone_phase += tone_incr;
            // If tone_phase has overflowed, truncate the top bit
            if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
          }
          release(shape_input);

        }

        else {

          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

            if (tone_phase < tone_width)*bp++ = -tone_amp;
            else *bp++ = tone_amp;

            mod = fm_input->data[i];
            tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);


            //tone_phase += tone_incr;
            // If tone_phase has overflowed, truncate the top bit
            if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
          }
        }
        release(fm_input);
      }

      else {
        if (shape_input) {

          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

            int32_t shape_mod = shape_input->data[i];
            uint32_t tone_width_mod = tone_width + (shape_mod << 16);


            if (tone_width_mod < 1) {
              tone_width_mod = 1;
            }


            if (tone_phase < tone_width_mod)*bp++ = -tone_amp;
            else *bp++ = tone_amp;
            tone_phase += tone_incr;

          }
          release(shape_input);
        }

        else {
          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

            if (tone_phase < tone_width)*bp++ = -tone_amp;
            else *bp++ = tone_amp;

            tone_phase += tone_incr;
            // If tone_phase has overflowed, truncate the top bit
            if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
          }
        }
      }
      break;

    case WAVEFORM_SAMPLE_HOLD:

      if (shape_input) {
        release(shape_input);
      }

      if (fm_input) {

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
          if (tone_phase < tone_incr) {
            sample = random(-tone_amp, tone_amp);
          }
          *bp++ = sample;
          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        release(fm_input);
      }
      else {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
          if (tone_phase < tone_incr) {
            sample = random(-tone_amp, tone_amp);
          }
          *bp++ = sample;
          tone_phase += tone_incr;
        }
      }
      break;


    }

    if (tone_offset) {
      bp = block->data;
      end = bp + AUDIO_BLOCK_SAMPLES;
      do {
        val1 = *bp;
        *bp++ = signed_saturate_rshift(val1 + tone_offset, 16, 0);
      } while (bp < end);
    }
    transmit(block, 0);
    release(block);
  }
}
