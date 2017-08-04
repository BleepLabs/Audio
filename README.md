Some additions to Paul's incredible teesny audio library that I created for the Thingamagoop 3000 and other projects. 
<br>
Additions:<br>
FM input in waveform synth - All you need to do is patch it manually as it's not in the tool. For example:<br>
AudioConnection          patchCord3(waveform1, 0, waveform2, 0);<br>
AudioConnection          patchCord1(waveform2, 0, i2s1, 0);<br>

Digital combine - For simple digital distortion<br>

Lazy mixer setup -  "mixer1.gain(.25,.25,.25.25)" and "mixer1.gain(.25)" can be used to setup all inputs to .25

Variable delay - Interpolated delay time gives tape like response. <br>

Sample and Hold - Allows for smooth bitcrushing at arbitrary rates and classic s&h modulators.<br>

Granular effect - Simple pitch shifting and freezing<br>

Arbitrary wavetable voice length<br>
arbitraryWaveform(sample array, maxFreq (unuses still), length of array (2047 max)) <br>

Bandlimited wavetables<br>
See bandlimiting directory for an example on using WAVEFORM_ARBITRARY to produce bandlimited oscillators as well as the processing sketch used to generate the arrays.<br>

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




