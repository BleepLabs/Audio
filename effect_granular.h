#include "AudioStream.h"

class AudioEffectGranular : 
public AudioStream
{
public:
  AudioEffectGranular(void): 
  AudioStream(1,inputQueueArray) { 
  }

  void begin(int16_t *sample_bank_def,int16_t max_len_def);
  void length(int16_t max_len_def);
  void rate(int16_t playpack_rate_def);

  void freeze(int16_t activate,int16_t playpack_rate_def,int16_t grain_length_def);
  void shift(int16_t activate,int16_t playpack_rate_def,int16_t grain_length_def);


  virtual void update(void);
  
private:
  audio_block_t *inputQueueArray[1];
  int16_t *sample_bank;
  uint32_t max_sample_len;
  int16_t freeze_en,shift_en;
  uint32_t write_head,read_head;
  int16_t grain_mode,freeze_len,allow_len_change;
  int16_t playback_rate;
  int16_t capture_trigger,capture_index,current_mode,playpack_rate;
  int32_t accumulator,play_index,increment;
  int16_t sample_loaded,prev_input,zero_cross_up,zero_cross_down,sample_trigger,write_en,glitch_cross_len,load_req,glitch_len,glitch_min_len,sample_req;


};

