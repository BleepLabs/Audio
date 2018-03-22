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
     


*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=195.71426391601562,211.42856407165527
AudioEffectGranular       granular1;        //xy=431.4285583496094,208.5714292526245
AudioOutputI2S           i2s2;           //xy=701.4285774230957,192.8571548461914
AudioConnection          patchCord1(i2s1, 0, granular1, 0);
AudioConnection          patchCord2(granular1, 0, i2s2, 0);
AudioConnection          patchCord3(granular1, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=725.9999961853027,277.4285707473755
// GUItool: end automatically generated code



#define freeze_button_pin 32
#define shift_button_pin 31

#define grain_pot_pin 33
#define rate_pot_pin 34

int16_t grain_pot;
int16_t rate_pot;

float pot[16] = {0};
float freq[16] = {};

uint32_t cm, prev[4];
int16_t freeze_button;
byte freeze_activate;
int16_t shift_button;

#define granular_bank_len  6000   //this can be any size really but the bigger the get the more shift becomes an odd stutter effect.   
int16_t granular_bank[granular_bank_len] = {}; //required


void setup() {

  AudioNoInterrupts();
  AudioMemory(100);

  granular1.begin(granular_bank, granular_bank_len);  //bank to use , max size of bank

  pinMode(freeze_button_pin, INPUT_PULLUP);
  pinMode(shift_button_pin, INPUT_PULLUP);

  AudioInterrupts();

}



void loop() {


  cm = millis();

  grain_pot = analogRead(grain_pot_pin) * (granular_bank_len / 1023); //have the pot go through the entire bank size.
  rate_pot = analogRead(rate_pot_pin);

  shift_button = !digitalRead(shift_button_pin);
  freeze_button = !digitalRead(freeze_button_pin);

  granular1.shift(shift_button, rate_pot, grain_pot);
  //shifts the pitch of the audio by playing small chunks (grains) back at faster or slower rate
  //(activate 1 on 0 bypass, playback speed, size of grain)
  //this effect uses a 0 crossing detector to remove some of the noise so the grain size will vary from assigned value

  granular1.freeze(freeze_button, rate_pot, grain_pot);//(activate 1 on 0 bypass, playback speed, size of grain)
  //Freezes a small chunk of audio
  //(activate 1 on 0 bypass, playback speed, size of grain)
  // try activating it at regular intervals for an interesting effect:
  //granular1.freeze(freeze_activate, rate_pot, grain_pot);//(activate 1 on 0 bypass, playback speed, size of grain)


  //granular is capturing audio while not active.
  //This means that granular1.shift/freeze(0, ratt, grain_size); need to called between the times it is activated to fill up the buffer.

  //freeze and shift can be activated simultaneously but freeze will take precidence.



  if (cm - prev[0] > 100) {
    prev[0] = cm;

    freeze_activate = !freeze_activate;

    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());

    Serial.println(grain_pot);
    Serial.println();

    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
  }

}


