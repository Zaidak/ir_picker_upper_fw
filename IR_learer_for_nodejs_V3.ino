/* 
2016 restart. 
This new version of the firmware will work with a nodejs server on a raspberry pi. 
The arduino will become the universal remote controller of the Pi. 
May begin development with Node-red. 

Previous:
this is based on the code that worked for TV and speakers but not the computer.
 - Uses tone to pulse a signal at 38 KHz.  
 - I wish to have a two way communication with the computer
 - Computer prepares the device to be ready for transmission, 
 - Then when the device sees a 1, it starts recording and sends the whole array to the PC [could be improved later]
 - Computer will process the array and ask the device to prepare for a new signal.
 */


#define STAR 42 //'*'
#define PLUS 43 // '+'   -> Inform Server Arduino is ready
#define READY 45 //'-'    <- Prepare Arduino to record a signal
#define REPEAT 46 // '.'  <- Repeat the last signal recorded
#define FREQ 47 // '/'    <- Keep repeating the last signal recorded
#define takes 500 //500 ensures recording one signal + space + and the begining of the next repitition. // 1700 //1700 is optimal but takes most of the memory
#define dt 100
#define spaceBeforeRep 120 // number of consequtive zeros to count before determining a signal is recorded and the following is repetition command
            // TODO make this value dynamically determined using the next define
#define zerosToOnesRatio 2 // ex. 2 means look for consequtive zeros that repeat for 2 times longer than the consequtive ones at the  begining of the signal 

#define encodedSize 100 //takes
/*  TODO clearout
#define b0_1 digitalWrite(5,HIGH);#define b1_1 digitalWrite(6,HIGH);#define b2_1 digitalWrite(7,HIGH);#define b3_1 digitalWrite(9,HIGH);//#define b4_1 digitalWrite(8,HIGH);
#define b0_0 digitalWrite(5,LOW);#define b1_0 digitalWrite(6,LOW);#define b2_0 digitalWrite(7,LOW);#define b3_0 digitalWrite(9,LOW);//#define b4_0 digitalWrite(8,LOW);*/
#define BPin 2
#define diPin 8// 8 for shield // 13 for breadboard prototype // A0 // IR sensor input 
#define LEDPin 9 // 9 for shield // 4 for breadboard prototype
#define pulsePin 10 // 11 for shield, //12 for breadboard prototype
#define boardLED 13
bool A[takes];
//int encoded[50];//takes];

unsigned char encoded[encodedSize];
bool sensorValue = 0;        // value read from the port
int in;
int f  =2;
bool done = false;

void arrayToHex();
void hexToArray();

bool connConfirmed = false;
void blink(int x){
  for(int i=0;i<x;i++){
    digitalWrite(boardLED,HIGH);
   delay(750); 
    digitalWrite(boardLED,LOW); 
       delay(250);
  }
  
}
void setup() {  
  pinMode(boardLED,OUTPUT);
          blink(10);

  Serial.begin(9600); 
  Serial.println("+");
  for(int i=0; i<10000;i++)if(Serial.read()==PLUS) connConfirmed = true;
  pinMode(LEDPin, OUTPUT);
  pinMode(diPin, INPUT);
  pinMode(BPin, INPUT);
  pinMode(pulsePin, OUTPUT);
  digitalWrite(LEDPin, LOW);
  for(int i=5;i<=9; i++){ //parallel bus with the pulse generator
    pinMode(i,OUTPUT);
    digitalWrite(i,LOW);
  }

  tone (pulsePin, 38000); // send to port 12 a pulse with frequency 38 KHz
  do{
    in = Serial.read();
    if (in == STAR){
      Serial.println("+");
      break;
    }
  }
  while(!connConfirmed);
  for(int i=0;i<encodedSize; i++)
  {
    encoded[i] = -1; 
  }
}
void customDelay(unsigned long  time) {
  unsigned long end_time = micros() + time;
  while(micros() < end_time);
}

void loop() {
  digitalWrite(boardLED,LOW);
  if(Serial.available() > 0)
  {
    in = Serial.read();
    switch (in){
    case READY:// ready to sense an IR signal
      digitalWrite(boardLED,HIGH);
          blink(10);
      while(1){
        sensorValue = digitalRead(diPin); 
        if ((sensorValue ==1) /*&& (done == false)*/)
        {
          for(int i =0; i< takes; i++){
            delayMicroseconds(dt);
            A[i] = digitalRead(diPin);
          }
          blink(10);
        sensorValue = digitalRead(diPin); 

          bool Astate = 1, foundSpace = false;
          int encIndex = 0;
          for(int i=0; i<takes && !foundSpace; ){///i++){
           encoded[encIndex] = 0; // init the encoded counts to 0
           while(A[i] == Astate){
             if(i>= takes) break;
             encoded [encIndex]++;
             if(encoded[encIndex] >= spaceBeforeRep) {
              encoded[encIndex] = -1;
              foundSpace = true;
              encIndex--;
               break;
               
             }
             i++;
           }
           // out of the while once the sequence of first one's or zeros after the first is done, 
           encIndex++;
           if(encIndex== 49) Serial.println("Reached end of encoded array");
           if(Astate == 1) Astate = 0;
           else Astate = 1;
           }
          Serial.println("D");
          Serial.println(encIndex);
          Serial.println("C");
          done = true;
          //for (int j=0; j<takes; j++) {
          //  Serial.println(A[j]);
          for (int j=0; j<encIndex; j++) {
            Serial.println(encoded[j]);
          }
          Serial.println("d");
          break;
        }
      }
      break;
    case REPEAT:
      //while(1){
      if (done == true){
        //        Serial.println("R");
        for(int w=0; w < takes; w++){
          if (A[w] ==1 ) digitalWrite(LEDPin, HIGH);
          else           digitalWrite(LEDPin, LOW);
          delayMicroseconds(dt); // 144, 143, 140
        }
        digitalWrite(LEDPin,LOW);
        Serial.println("T");
      }
      //    }
      break;
    case FREQ:
      while(Serial.available()==0){
      } // wait for amount

      in = Serial.read();
/*      f = in - 48; // because 0 is 48 in ascii
      if(f%2 == 0) {        b0_0;      }      else {        b0_1;      }
      f/=2;
      if(f%2 == 0) {        b1_0;      }      else {        b1_1;      }
      f/=2;
      if(f%2 == 0) {        b2_0;      }      else {        b2_1;      }
      f/=2;
      if(f%2 == 0) {        b3_0;      }      else {        b3_1;      }*/ // TODO Rewrite the above code to update the tone pulse frequency using the read value. 
      //     f/=2;
      //     if(f%2 == 0) {b4_0;}
      //     else {b4_1;}

      break;
 //   default:
   //   Serial.println("Unknown input");
    }
  }
}


