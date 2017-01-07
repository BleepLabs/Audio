
#include "effect_var_dly.h"
#include "arm_math.h"

void AudioEffectvariableDelay::begin(short *delayline,int max_len,int dly_len, short sm, short lerp)
{
  max_dly_len=max_len-1;

  desired_delay_length = dly_len;
  if (desired_delay_length>max_dly_len){
    desired_delay_length=max_dly_len;
  }
  l_delayline = delayline;
  write_head = 0;

  lerp_len=lerp;
  
    if (sm>1){sm=1;}
  SIMPLE_SMOOTH=sm;



}

int16_t AudioEffectvariableDelay::length(int dly_len)
{
  desired_delay_length = dly_len;

  if (desired_delay_length>max_dly_len){
    desired_delay_length=max_dly_len;
  }

  return delay_length;

}


void AudioEffectvariableDelay::update(void)
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
        tock=!tock;
      if (tock==0){
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


      if (SIMPLE_SMOOTH==1){
      l_delayline[write_head] = (*bp + feedback)/2;  // 
      input1 = *bp;
      *bp++ = (l_delayline[read_head]+input1)/2;
      }

      else{
      l_delayline[write_head] = *bp ;
      input1 = *bp;
      *bp++ = l_delayline[read_head];
      }

      feedback = (l_delayline[read_head]+input1)/2;

}
    
    // send the effect output to the left channel
    transmit(block);
    release(block);
  }
}




