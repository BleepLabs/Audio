byte table_n = 63; // number of tables to create from "octave" banks. 
int wave_n=149;  //number of waves to calcilate. Leave this alone as "max_freq" takes care of the band limiting


int peak=32000;  //highest positive value. It will need to be lower than waht you want. This yields something closer to 2^15
int period_len=256;
float pi= 3.14159265359;
int[] out_table = new  int [1024]; //what were them numbers live

size((period_len +20) , 512); 
background(0);

PrintWriter output;
output = createWriter("tri.txt"); 


int[] square_harm_table = { 0,1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39,41,43,45,47,49,51,53,55,57,59,61,63,
65,67,69,71,73,75,77,79,81,83,85,87,89,91,93,95,97,99,101,103,105,107,109,111,113,115,117,119,121,123,125,127,
129,131,133,135,137,139,141,143,145,147,149,151,153,155,157,159,161,163,165,167,169,171,173,175,177,179,181,183
,185,187,189,191,193,195,197,199,201,203,205,207,209,211,213,215,217,219,221,223,225,227,229,231,233,235,237,239
,241,243,245,247,249,251,253,255,257,259,261,263,265,267,269,271,273,275,277,279,281,283,285,287,289,291,293,295};

int[] eigth_octave = {1,100,120,130,140,150,160,170,180,190,200,220,240,260,280,300,320,340,360,380,400,220,240,260,280,300
,320,340,360,380,400,440,480,520,560,600,640,680,720,760,800,880,960,1040,1120,1200,1280,1360,1520,1600,1760,1920,
2240,2400,2560,2720,2880,3040,3200,3840,4800,6400,9600,};
 // 8 tabels per octave with unessceary ones removed. leaving 63 

int max_freq =19000;
int base_freq =100;   
int harmonic=0;
int harmonic_amp=0;

//////////////////////////////////////////////////////

  
  //    output.print("int16_t  ramp");
 //     output.print("[32][256] = {");      

 
for(int bf=0; bf<table_n; bf++){
   base_freq=eigth_octave[bf];

   for(int g=0;g<wave_n;g++){
   harmonic=square_harm_table[g];
   harmonic_amp=square_harm_table[g]*(square_harm_table[g]);  

        for(int j=0;j<period_len;j++){
            
          if ((base_freq*harmonic)<max_freq){
             
            out_table[j]+=peak*(sin((harmonic*j*2*pi)/period_len))/harmonic_amp;
         
            if (out_table[j]>32766 || out_table[j]<-32766){
             //  print("clip");
            } 
          }

        }
      }
  
      stroke(0,30,100);
    line(0, 255, 1025 , 255);
  
      stroke(255);

      output.print(" {");      
    

   for(int g=0;g<period_len;g++){
     line((g)+10, (out_table[g]/128)+255, ((g)+1)+10, (out_table[g+1]/128)+255);
   
     }
  
    
    for(int h=0;h<period_len;h++){
      output.print(out_table[h]);      
      output.print(", ");
      
      if (h>1 && h%32==0){
   //       output.println();
      }
    }
    //  output.println();      
      output.println("},");      
    //  output.println();      
     }
   
///////////  

  output.flush(); 
  output.close();  
  println("done");
