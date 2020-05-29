// matrix_mixer.h

#ifndef matrix_mixer_h_
#define matrix_mixer_h_

#include "Arduino.h"
#include "AudioStream.h"

class AudioMatrixMixer : public AudioStream
{

public:
	AudioMatrixMixer(void) : AudioStream(4, inputQueueArray) {
		/*		for (int j = 0; j < 4; j++) {
					for (int i = 0; i < 4; i++) {
						multiplier[j][i] = 65536;
					}*/
	}
	virtual void update(void);

	void gain(unsigned int out_sel, unsigned int channel, float gain) {
		if (channel >= 4) return;
		if (out_sel >= 4) return;
		if (gain > 32767.0f) gain = 32767.0f;
		else if (gain < -32767.0f) gain = -32767.0f;
		multiplier[out_sel][channel] = gain * 255.0f; // TODO: proper roundoff?
	}
private:
#define mmax 4

	byte multiplier[mmax][mmax];
	audio_block_t *inputQueueArray[mmax];
};
#endif
