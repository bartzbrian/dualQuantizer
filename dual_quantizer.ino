//Brian Bartz 2018
//two channel pitch quantizer
//for eurorack modular synth
//using nano and double 12-bit dac 

#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dacOne;
Adafruit_MCP4725 dacTwo;

int inOne;
int inTwo;
int outputOne;
int outputTwo;

int currDiffOne=0;
int outputDiffOne=5000;

int currDiffTwo=0;
int outputDiffTwo=5000;

// pin,state
int scaleMap[12][2]{
  {13,0},{12,0},{11,0},{10,0},
  {9,0},{8,0},{7,0},{6,0},
  {5,0},{4,0},{3,0},{2,0}
};

// dac output values
int lookup[12][5];

void setup() {
  Serial.begin(9600);
  dacOne.begin(0x62);
  dacTwo.begin(0x64);
  populateLookup();
  updateScale();
}
//inits lookup table with semitone dac vals
void populateLookup(){
  int pasteVal=0;
  for(int i=0;i<5;i++){
    for(int j=0;j<12;j++){
       lookup[j][i]=pasteVal;
       pasteVal+=68.266; 
    }  
  }
}
//updates the states of the 12 switches that create scale
int updateScale(){
  int on=0;
  for(int i=0;i<12;i++){
    if (digitalRead(scaleMap[i][0])){
      scaleMap[i][1]=1;
      on=1;
    }else{
      scaleMap[i][1]=0;  
    }
  }
  return on;
}

void loop() {
  //adjusts 10bit input to 12bit out
  inOne = analogRead(A1)*4;
  inTwo = analogRead(A0)*4;
  
  currDiffOne=0;
  currDiffTwo=0;
  outputDiffOne=5000;
  outputDiffTwo=5000;
  //if there is at least one note in the scale
  if(updateScale()){
    for(int j=0;j<12;j++){
      //if this iteration's semitone is in the selected scale
      if (scaleMap[j][1]){
        //iterated through the five octave outputs, checking for minimal difference (i.e. quantization targets)
        for(int i=0;i<5;i++){
          currDiffOne = abs(inOne-lookup[j][i]);
          currDiffTwo = abs(inTwo-lookup[j][i]);
          //if a new minimum difference is found, set the outputs to it
          if (currDiffOne<outputDiffOne){
            outputDiffOne=currDiffOne;
            outputOne=lookup[j][i];  
          }
          
          if (currDiffTwo<outputDiffTwo){
            outputDiffTwo=currDiffTwo;
            outputTwo=lookup[j][i];  
          }
                              
        }
      }
    }
  } else {outputOne=0;outputTwo=0;}//output 0V if no notes in scale
  //output the voltages
  dacOne.setVoltage(outputOne,false);
  dacTwo.setVoltage(outputTwo,false);
}
