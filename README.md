Some additions to Pauls incredible teesny audio library that I created fro the THingamagoop 300 and other projects. 
<br>
Additions:<br>
FM input in waveform synth - All you need to do is patch it manually as it's not in the tool. For example:<br>
AudioConnection          patchCord3(waveform1, 0, waveform2, 0);<br>
AudioConnection          patchCord1(waveform2, 0, i2s1, 0);<br>

Bandlimited waveforms see the Thingamagoop 3000 code for usage . https://github.com/BleepLabs/Thingamagoop-3000/blob/f677f54a9b7510a64d8e80f4b67bba2a56f2a077/t3k_v15/t3k_v15.ino#L994 & https://github.com/BleepLabs/Thingamagoop-3000/blob/f677f54a9b7510a64d8e80f4b67bba2a56f2a077/t3k_v15/modes.ino#L32<br>

Digital combine - For simple digital distortion<br>

Lazy mixer setup -  "mixer1.gain(.25,.25,.25.25)" and "mixer1.gain(.25)" can be used to setup all inputs to .25

Variable delay - Interpolated delay time gives tape like response. <br>

Sample and Hold - Allows for smooth bitcrushing at arbitrary rates and classic s&h modulators.<br>

Granular effect - Simple pitch shifting and freezing<br>

Arbitrary wavetable voice length<br>
arbitraryWaveform(sample array, maxFreq (unuses still), length of array (2047 max)) <br>

Bandlimited wavetables
See bandlimited direct for an example and the processing sketch uised to generate the arrays.

Variable triangle wave. WAVEFORM_VARIABLE_TRIANGLE <br>
waveform1.varible_triangle(0); or waveform1.pulseWidth(0) would be a saw<br>
waveform1.varible_triangle(.75); would be halfway between a regular triangle and a ramp<br>

Control input for pulse width and varible triangle. For example:<br>
AudioConnection          patchCord1(sine1, 0, waveform1, 0);<br>
AudioConnection          patchCord2(sine2, 0, waveform1, 1);<br>
Would be fm from the sine wone and shpe modulation from sine2<br>


<br>
http://bleeplabs.com/store/thingamagoop-3000/
<br><br>

http://www.pjrc.com/teensy/td_libs_Audio.html



