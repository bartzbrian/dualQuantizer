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

void populateLookup(){
  int pasteVal=0;
  for(int i=0;i<5;i++){
    for(int j=0;j<12;j++){
       lookup[j][i]=pasteVal;
       pasteVal+=68.266; 
    }  
  }
}

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
  
  inOne = analogRead(A1)*4;
  inTwo = analogRead(A0)*4;
  
  currDiffOne=0;
  currDiffTwo=0;
  outputDiffOne=5000;
  outputDiffTwo=5000;
  
  if(updateScale()){
    for(int j=0;j<12;j++){
      
      if (scaleMap[j][1]){
        
        for(int i=0;i<5;i++){
          currDiffOne = abs(inOne-lookup[j][i]);
          currDiffTwo = abs(inTwo-lookup[j][i]);
          
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
  } else {outputOne=0;outputTwo=0;}
  
  dacOne.setVoltage(outputOne,false);
  dacTwo.setVoltage(outputTwo,false);
}
