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
AudioInputI2S            i2s1;           //xy=104,218
AudioMixer4              mixer1;         //xy=315,240
AudioEffectTapeDelay         tapeDelay1;         //xy=473,232
AudioOutputI2S           i2s2;           //xy=701.4285774230957,192.8571548461914

AudioConnection          patchCord1(i2s1, 0, mixer1, 0);
AudioConnection          patchCord2(mixer1, tapeDelay1);
AudioConnection          patchCord3(tapeDelay1, 0, i2s2, 0);
AudioConnection          patchCord5(tapeDelay1, 0, i2s2, 1);
AudioConnection          patchCord4(tapeDelay1, 0, mixer1, 1);


AudioControlSGTL5000     sgtl5000_1;     //xy=725.9999961853027,277.4285707473755
// GUItool: end automatically generated code


#define pot1_pin 33
#define pot2_pin 34

#define DELAY_MAX_LEN 10000
short tape_delay_bank[DELAY_MAX_LEN] = {};

float delay_time;
float feedback_level;

uint32_t cm, prev[4];



void setup() {

  AudioNoInterrupts();
  AudioMemory(10);

  tapeDelay1.begin(tape_delay_bank, DELAY_MAX_LEN, 5000, 1, 10); //bank to use, size of bank, delay time in samples , rate reduction, interpolation time
  //rate reduction of 0 means the delay will increment at the same rate as playback
  //at 1 we double out delay time by halving the sample rate. 2 quadrules it and so on.   
  //Halving the sample rate is nothing to be scared of!
  
  //interpolation time is how ling it takes for the "read head" to get to the desired dlay length. 
  //0 is as quickly as possible, 10 is a standard tape delay sound and 20 is getting a little slow and crazy
  
  
  mixer1.gain(0, .5);
  mixer1.gain(1, .5); //feedback level

  AudioInterrupts();
  analogReadAveraging(41);

}



void loop() {

  cm = millis();

  if (cm - prev[1] > 5) { //to redusce nosie ist's best not to do this too rapidly and to smooth the pot reading.
    prev[1] = cm;
    delay_time = analogRead(pot1_pin) * ( DELAY_MAX_LEN / 1023.00);
    feedback_level = analogRead(pot2_pin) / 1023.00;
    mixer1.gain(1, feedback_level);
    tapeDelay1.length(delay_time);
  }

  if (cm - prev[0] > 500) {
    prev[0] = cm;


    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());

    Serial.println(delay_time);
    Serial.println();

    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
  }

}


