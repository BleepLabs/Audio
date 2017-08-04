#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "wave_tables.h"


// GUItool: begin automatically generated code
AudioSynthWaveform       waveform4;      //xy=173,428
AudioSynthWaveform       waveform3;      //xy=191,346
AudioSynthWaveform       waveform2;      //xy=239,300
AudioSynthWaveform       waveform1;      //xy=282,245
AudioMixer4              mixer1;         //xy=492,338
AudioOutputPT8211        pt8211_1;       //xy=640,330
AudioConnection          patchCord1(waveform4, 0, mixer1, 3);
AudioConnection          patchCord2(waveform3, 0, mixer1, 2);
AudioConnection          patchCord3(waveform2, 0, mixer1, 1);
AudioConnection          patchCord4(waveform1, 0, mixer1, 0);
AudioConnection          patchCord5(mixer1, 0, pt8211_1, 0);
// GUItool: end automatically generated code

int16_t bl_sel[16] = {};
float pot[16] = {0};
float freq[16] = {};
const int16_t eigth_octave[80] = {1, 100, 120, 130, 140, 150, 160, 170, 180, 190, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400, 220, 240, 260, 280, 300
                                  , 320, 340, 360, 380, 400, 440, 480, 520, 560, 600, 640, 680, 720, 760, 800, 880, 960, 1040, 1120, 1200, 1280, 1360, 1520, 1600, 1760, 1920,
                                  2240, 2400, 2560, 2720, 2880, 3040, 3200, 3840, 4800, 6400, 9600,
                                 }; // unessceary ones removed. leaving 63 = table_n
                                 
byte wave_switch[8][table_n] = {};

uint32_t cm, prev[4];


void setup() {

  AudioNoInterrupts();
  delay(1000);

  AudioMemory(20);
  waveform1.begin(1, 0, WAVEFORM_ARBITRARY);
  waveform2.begin(1, 0, WAVEFORM_ARBITRARY);
  waveform3.begin(1, 0, WAVEFORM_ARBITRARY);
  waveform4.begin(1, 0, WAVEFORM_ARBITRARY);
  AudioInterrupts();

  mixer1.gain(0,.22); 
  mixer1.gain(1,.22); 
  mixer1.gain(2,.22); 
  mixer1.gain(3,.22); 

}



void loop() {


  cm = millis();

  pot[0] = analogRead(A14);
  pot[1] = (analogRead(A15));

  freq[1] = pot[0];
  freq[2] = pot[0] / 2.002  ;
  freq[3] = pot[1];
  freq[4] = pot[1] / 2.001;



  waveform1.frequency(freq[1]);
  waveform2.frequency(freq[2]);
  waveform3.frequency(freq[3]);
  waveform4.frequency(freq[4]);


  bandlimit(4);


  waveform1.arbitraryWaveform(ramp[bl_sel[1]], 20000);
  waveform2.arbitraryWaveform(square[bl_sel[2]], 20000);
  waveform3.arbitraryWaveform(triangle[bl_sel[3]], 20000);
  waveform4.arbitraryWaveform(square[bl_sel[4]], 20000);


  if (cm - prev[0] > 200) {
    prev[0] = cm;

    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());

    Serial.println(" ");
    
    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
  }

}



void bandlimit (byte wave_n) {
//use "freq[n]" to determine which wavetable to use 
  
  for (int h = 1; h < wave_n; h++) {
    if (freq[h] > 0) {
      for (int wj = 0; wj < table_n; wj++) {

        if (freq[h] > eigth_octave[wj]  && freq[h] <= eigth_octave[wj + 1]) {

          bl_sel[h] = wj;
        }
      }
    }
  }
}

