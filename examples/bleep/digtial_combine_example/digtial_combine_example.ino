/*
   Copyright (c) 2018 John-Michael Reed
   bleeplabs.com

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.


   This is a classic granular effect that uses a varible speed buffer to shift the pitch and freeze incoming audio.

digitalCombine1

*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=154,306
AudioSynthWaveformSine   sine2;          //xy=156,252
AudioSynthWaveformSine   sine1;          //xy=241,140
AudioMixer4              mixer1;         //xy=346,298
AudioEffectDigitalCombine    digitalCombine1;      //xy=424,210
AudioOutputI2S           i2s2;           //xy=701.4285774230957,192.8571548461914
AudioConnection          patchCord1(i2s1, 0, mixer1, 1);
AudioConnection          patchCord2(sine2, 0, mixer1, 0);
AudioConnection          patchCord3(sine1, 0, digitalCombine1, 0);
AudioConnection          patchCord4(mixer1, 0, digitalCombine1, 1);
AudioConnection          patchCord5(digitalCombine1, 0, i2s2, 0);
AudioConnection          patchCord6(digitalCombine1, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=725.9999961853027,277.4285707473755
// GUItool: end automatically generated code


#define pot1_pin 33
#define pot2_pin 34

int16_t grain_pot;
int16_t rate_pot;
int mode_sel;
uint32_t cm, prev[4];
float freq1;
float freq2;


void setup() {

  AudioNoInterrupts();
  AudioMemory(100);

  digitalCombine1.mode(OR); //OR is default. OR XOR AND MODULO or 0-3 in that order are the options

  sine1.amplitude(1);
  sine2.amplitude(1);

  mixer1.gain(0,1); //sine level
  mixer1.gain(1,0); //input level

  //Combining two simple oscillators results in interesting new waveforms.
  //Combining white noise or dynamic incoming audio results in aggressive digital distortion.
  

  AudioInterrupts();

}



void loop() {



  freq1 = analogRead(pot1_pin) / 10.00;
  freq2 = analogRead(pot2_pin);
  sine1.frequency(freq1);
  sine2.frequency(freq2);

  cm = millis();

  if (cm - prev[0] > 500) {
    prev[0] = cm;

    mode_sel++;  //step through the 4 modes
    if ( mode_sel > 3) {
      mode_sel = 0;
    }
    digitalCombine1.mode(mode_sel); //OR is default. OR XOR AND MODULO or 0-3 in that order are the options


    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());

    Serial.println(grain_pot);
    Serial.println();

    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
  }

}


