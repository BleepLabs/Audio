/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "effect_combine.h"

void AudioEffectDigitalCombine::update(void)
{
#if defined(KINETISK)
	audio_block_t *blocka, *blockb;
	int16_t *pa, *pb, *end;

	const int16_t fold_max = 32767;
	const int16_t fold_min = fold_max*-1;

	blocka = receiveWritable(0);
	blockb = receiveReadOnly(1);
	if (!blocka) {
		if (blockb) release(blockb);
		return;
	}
	if (!blockb) {
     	transmit(blocka);

		release(blocka);
		return;
	}
	pa = (blocka->data);
	pb = (blockb->data);
	end = pa + AUDIO_BLOCK_SAMPLES;
	while (pa < end) {
		shifta = *pa;
		shiftb = *pb++;
		if (bitmode==0){
		temp1 = shifta | shiftb;
     	//	out1 = shifta & shiftb; no real diff between & and |
		//out1 = shifta ^ shiftb; // not too diff either

		}

		if (bitmode==1){
			//out1 = shifta % shiftb;
		    temp1 = (shifta*shiftb)>>10; //yup

		}

		if (bitmode==2){

		    temp1 = (shifta)^shiftb; 

		}


		if (bitmode==3){

		    temp1 = (shiftb)&shiftb; 

		}


		if (bitmode==3333){
		int32_t rate_reduction_amt=(shiftb)/2;
		if (rate_reduction_amt<1){
			rate_reduction_amt=1;
		}
		tick1++;
		if (tick1>4000){
		//Serial.println(rate_reduction_amt);
			tick1=0;
		}

	      accumulator0 += rate_reduction_amt;

	      if (accumulator0 > 65535){
	      //	Serial.println(accumulator0);
	        accumulator0 -= 65534;
	        temp1=shifta;
	        paudio_in0=temp1;
	      }

	      else {
	        temp1 = paudio_in0;
	      }
		}



		if (bitmode==4){
			//shifta+=32767;
			//shiftb+=32767;
		    byte hba = (shifta>>8) & 0xFF;
			byte lba = (shifta) & 0xFF;
			byte hbb = (shiftb>>8) & 0xFF;
			byte lbb = (shiftb) & 0xFF;

	    
		    temp1 = (((hba*lbb) * (hbb*lba))>>8)-32768; 
   			//Serial.println(temp1);
		}


		if (bitmode==5){
			shifta+=32767;
			//shiftb+=32767;

		    byte hba = (shifta>>8) & 0xFF;
			byte lba = (shifta) & 0xFF;
			byte hbb = (shiftb>>8) & 0xFF;
			byte lbb = (shiftb) & 0xFF;

	    
		    temp1 = ((hba*lba) + hbb*lbb)-32768; 

   			//Serial.println(temp1);
		}

		if (bitmode>=6){
			//brobably the best syte shuffle so far 
			//shifta+=32767;
			//shiftb+=32767;
		    byte hba = (shifta>>8) & 0xFF;
			byte lba = (shifta) & 0xFF;
			byte hbb = (shiftb>>8) & 0xFF;
			byte lbb = (shiftb) & 0xFF;

	    
		    temp1 = ((hba|lbb) * (hbb|lba))-32768;
   			//Serial.println(temp1);
		}

		int16_t j=0;
		while (j<7) {
			if (temp1>fold_max){
	      		temp1 -= ((temp1 - fold_max) * 2);
	  		}

			if (temp1<fold_min){
	      		temp1 += ((temp1 - fold_min) * -2);
	  		}
		j++;

		}

		out1=temp1;
		*pa++ = out1;

	}
	transmit(blocka);
	release(blocka);
	release(blockb);

#elif defined(KINETISL)
	audio_block_t *block;

	block = receiveReadOnly(0);
	if (block) release(block);
	block = receiveReadOnly(1);
	if (block) release(block);
#endif
}



/*
			//ballon fight of mr hithat
			// cool but only works if both are  slow. I guess this mode could change the freq of the main osc? 

			byte hb = (shiftb) & 0xFF;
			byte lb = (shifta) & 0xFF;
		    out1 = ((hb * lb)); 


			// this works maginall better than that?
			int32_t r1=(shifta*shiftb)>>4;
			int16_t j=0;
     		const int16_t fold_max = 32768;
  			const int16_t fold_min = fold_max*-1;

//meh but works
			while (j<2) {
				if (r1>fold_max){
		      		r1 -= ((r1 - fold_max) * 2);
		  		}

				if (r1<fold_min){
		      		r1 += ((r1 - fold_min) * -2);
		  		}
			j++;
		    out1 = r1; 

			}



		    


//pretty ok
		    		writehead++;

		apos=shifta+32767;
	    bpos=shiftb+32767;

		if (writehead>4000) {
			writehead=0;
		}

		delayline[writehead]=(shifta+readt1)>>1;

		readhead=writehead+1+(bpos>>4);
		if (readhead>3999) {
			readhead-=4000;
		}

		readt1=delayline[readhead];
        temp1 = (shifta+readt1); 


//it doesnt go to 0 but otherwise the sa,e
			apos=shifta+32767;
			bpos=shiftb+32767;
		    temp1 = (apos|bpos)-32767; 


//jsut a buzzsaw
		    			shifta+=32767;
			shiftb+=32767;
		    byte hba = (shifta>>8) & 0xFF;
			byte lba = (shifta) & 0xFF;
			byte hbb = (shiftb>>8) & 0xFF;
			byte lbb = (shiftb) & 0xFF;
	    
		    temp1 = ((hba+lbb + hbb+lba)<<9)-32768; 

*/

