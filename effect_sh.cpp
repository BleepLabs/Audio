#include "effect_sh.h"




void AudioEffectSH::update(void){

  audio_block_t *blocka,*blockb;
  int16_t *pa,*pb,*end;
  static uint32_t prev_sh;
  static int16_t paudio_in0,paudio_in1,ccnt0,ccnt1,audio_in0,audio_out0,audio_out1,audio_out_c,mod_in;
  float step = 1/2^(16);
  static uint32_t accumulator0;
  static int16_t last,hey;


  blocka = receiveWritable(0);
  blockb = receiveReadOnly(1);

  if (!blocka) {
   return;
  }

  pa = (int16_t *)(blocka->data);
  pb = (int16_t *)(blockb->data);

  end = (pa + AUDIO_BLOCK_SAMPLES);

  while (pa < end) {
    paudio_in0=audio_out_c;
    audio_in0 = *pa;
    
    if (blockb) {
    mod_in= *pb++;
    }

    if (!blockb) {
    mod_in= -1;
    }
    

    //uint32_t increment0=rate_reduction_amt;
    //if (smooth_en==1){

      accumulator0 += rate_reduction_amt;

      if (accumulator0 > 65535){
        accumulator0 -= 65535;
        audio_out_c=audio_in0;
      }

      else {
        audio_out_c = paudio_in0;
      }
/*
    }

    if (smooth_en==0){
      ccnt0++;

      if (ccnt0 >= rate_reduction_amt){
          ccnt0=0;
         audio_out_c = audio_in0;
      }

      else {
        audio_out_c = paudio_in0;
      }

    }          


    if (manual_hold_en==1 || mod_in>=1){
      audio_out_c = paudio_in0;
    }
*/

    *pa++ = audio_out_c;

/*
    if (millis() - prev_sh>150){  
      prev_sh=millis();
      Serial.println(hey);

      Serial.println(rate_reduction_amt);
      Serial.println(mod_in);
      Serial.println();

    }  
*/
    
   // *pa++ = t1;

  }

  transmit(blocka);
  release(blocka);
  if (blockb)
  {
      release(blockb);
  }



}

