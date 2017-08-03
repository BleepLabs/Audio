
#include "synth_waveform.h"
#include "arm_math.h"
#include "utility/dspinst.h"
#include "Arduino.h"


void AudioSynthWaveform::update(void)
{
  audio_block_t *block, *fm_input;
  short *bp, *end;
  int32_t val1, val2, val3;
  uint32_t index, index2, scale;
  int16_t mod;

  // temporaries for TRIANGLE
  uint32_t mag;
  short tmp_amp;

  fm_input = receiveReadOnly(0);


  if (tone_amp == 0) return;
  block = allocate();
  if (block) {
    bp = block->data;
    switch (tone_type) {

    case WAVEFORM_SINE:



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

        // phase and incr are both unsigned 32-bit fractions
        if (fm_input) {
          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        else {
          tone_phase += tone_incr;
        }
        //tone_phase += tone_incr;
        // If tone_phase has overflowed, truncate the top bit
        if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
      }

      if (fm_input) {
        release(fm_input);
      }


      break;

    case WAVEFORM_ARBITRARY:


      if (!arbdata) {
        release(block);
        if (fm_input) {
          release(fm_input);
        }
        return;
      }

      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        if (arb_len > 255)
        {
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

        if (arb_len == 255)
        {
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



      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        if (tone_phase & 0x40000000)*bp++ = -tone_amp;
        else *bp++ = tone_amp;
        if (fm_input) {
          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        else {
          tone_phase += tone_incr;
        }
        // If tone_phase has overflowed, truncate the top bit
        if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
      }


      if (fm_input) {
        release(fm_input);
      }

      break;

    case WAVEFORM_SAWTOOTH:


      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        *bp++ = ((short)(tone_phase >> 15) * tone_amp) >> 15;
        // phase and incr are both unsigned 32-bit fractions
        tone_phase += tone_incr;
        if (fm_input) {
          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        else {
          tone_phase += tone_incr;
        }
        //tone_phase += tone_incr;
        // If tone_phase has overflowed, truncate the top bit
        if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
      }

      if (fm_input) {
        release(fm_input);
      }

    case WAVEFORM_SAWTOOTH_REVERSE:
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        *bp++ = ((short)(tone_phase >> 15) * tone_amp) >> 15;
        // phase and incr are both unsigned 32-bit fractions
        if (fm_input) {
          mod = fm_input->data[i];
          tone_phase -= tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        else {
          tone_phase -= tone_incr;
        }
      }
      if (fm_input) {
        release(fm_input);
      }

      break;

    case WAVEFORM_VARIABLE_TRIANGLE:

      static int32_t pindex;

      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // Calculate interpolated sin

        index = tone_phase >> 23;

        if (index < knee) {
          vtout = ((index * waveamp) / knee) - waveamp / 2;
        }
        if (index >= knee) {
          vtout = ((((index - knee) * waveamp) / (wavelength - knee)) * -1) + waveamp / 2;
          //vtout = 0;

        }
        val1 = vtout;

        *bp++ = (short)((val1 * tone_amp) >> 15);

        if (fm_input) {
          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        else {
          tone_phase += tone_incr;
        }
        //tone_phase += tone_incr;
        // If tone_phase has overflowed, truncate the top bit
        if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
      }

      if (fm_input) {
        release(fm_input);
      }

      break;

    case WAVEFORM_TRIANGLE:

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

        if (fm_input) {
          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        else {
          tone_phase += tone_incr;
        }

        if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
      }

      if (fm_input) {
        release(fm_input);
      }
      break;


    case WAVEFORM_PULSE:

      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

        if (tone_phase < tone_width)*bp++ = -tone_amp;
        else *bp++ = tone_amp;

        if (fm_input) {
          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        else {
          tone_phase += tone_incr;
        }
        //tone_phase += tone_incr;
        // If tone_phase has overflowed, truncate the top bit
        if (tone_phase & 0x80000000)tone_phase &= 0x7fffffff;
      }

      if (fm_input) {
        release(fm_input);
      }
      break;




    case WAVEFORM_SAMPLE_HOLD:
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        if (tone_phase < tone_incr) {
          sample = random(-tone_amp, tone_amp);
        }
        *bp++ = sample;
        if (fm_input) {
          mod = fm_input->data[i];
          tone_phase += tone_incr + (multiply_32x32_rshift32(tone_incr, mod << 16) << 1);
        }
        else {
          tone_phase += tone_incr;
        }
      }
      if (fm_input) {
        release(fm_input);
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

