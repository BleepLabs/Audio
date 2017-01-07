#include "AudioStream.h"

class AudioEffectvariableDelay : 
public AudioStream
{
public:
  AudioEffectvariableDelay(void): 
  AudioStream(1,inputQueueArray) { 
  }

  void begin(short *delayline,int max_len,int dly_len, short sm, short lerp);
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
  int delay_offset_idx;
  int   delay_rate_incr;
  int read_head,feedback;
  short SIMPLE_SMOOTH,lerp_len;
  unsigned int l_delay_rate_index;
};
