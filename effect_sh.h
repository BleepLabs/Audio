#ifndef effect_sh_h_
#define effect_sh_h_
#include "AudioStream.h"
#include "utility/dspinst.h"

class AudioEffectSH : public AudioStream
{
public:
  AudioEffectSH() : AudioStream(2, inputQueueArray) { }


  virtual void update(void);
  void begin(float rr_amt,short sm){
    smooth(sm);
    amount(rr_amt);
  }

  void smooth(short sm) {
    smooth_en=sm;
  }

  void manual_hold(short mh) {
    manual_hold_en=mh;
  }

  void amount(float rr_amt) {



      if (rr_amt>0){
        float a3t = rr_amt * rr_amt ; //pow doesn't work here?
        rate_reduction_amt =rr_amt* (65536.0f);
        //rate_reduction_amt =rr_amt* (65536.0f);

      }
      if (rr_amt<=0){
        rate_reduction_amt=0;
      } 
    

    }     

  


 


private:
  audio_block_t *inputQueueArray[2];
  uint32_t rate_reduction_amt;  
  int32_t  mix;  
  int32_t  mix_inv;  
  short smooth_en ;  
  short manual_hold_en;  


};

#endif