/*
2017-03-15 - update: 
  - revise logic: 
    * On startup the arduino is in waiting mode, builtin LED on pin 13 is to keep blinking, until a server connect request is received. 
  - improved feadback
    * When server is connected the LED will blink twice then ACK connection
    * when ready to record a signal, the LED is ON, when done, the LED is OFF.
*/

// *********************** Communication Chars
//                     Arduino     Server
#define S_STARTED 43  // '+'    <- Server is started and trying to connect and remove the arduino from the inital waiting state
#define I_STARTED 44  // ','   -> Inform Server Arduino is ready
#define ARM_TO_RECORD 45      // '-'    <- Prepare Arduino to record a signal
#define REPEAT 46     // '.'    <- Repeat the last signal recorded
#define FREQ 47       // '/'    <- Set the pulsing freq with the next received char
#define ACK 50
// *********************** Learning config parameters
#define takes 500           //500 ensures recording one signal + space + and the begining of the next repitition. // 1700 //1700 is optimal but takes most of the memory
#define encodedSize 100     // Size of char array holding encoded signal 
#define dt 100              // Sensing reading delay in ms
#define spaceBeforeRep 120  // number of consequtive zeros to count before determining a signal is recorded and the following is repetition command // TODO make this value dynamically determined using the next define
#define zerosToOnesRatio 2  // ex. 2 means look for consequtive zeros that repeat for 2 times longer than the consequtive ones at the  begining of the signal 
#define INIT_WAIT 1000
// *********************** Shield Pins
#define diPin 8// 8 for shield // 13 for breadboard prototype // A0 // IR sensor input 
#define LEDPin 9 // 9 for shield // 4 for breadboard prototype
#define pulsePin 11 // 11 for shield, //12 for breadboard prototype
#define boardLED 13

bool dIn[takes];
unsigned char encoded[encodedSize];
bool sensorValue = 0;        // value read from the port
int in;
int f  =2;
bool done = false;

bool connConfirmed = false;


void blink(int x){  //for(int i=0;i<x;i++){ digitalWrite(boardLED,HIGH);   delay(750);  digitalWrite(boardLED,LOW);    delay(250); }
  }
void setup() {  
  pinMode(diPin, INPUT);
  pinMode(LEDPin, OUTPUT);
  pinMode(pulsePin, OUTPUT);
  pinMode(boardLED, OUTPUT);
  digitalWrite(LEDPin, LOW);
  digitalWrite(pulsePin, LOW);
  Serial.begin(9600); 

  int blinktimer = 0;
  bool board_led_status = false;
  while(1){ // Waiting for server request to connect. did we get S_STARTED ?
    if(Serial.read()==S_STARTED){
      digitalWrite(boardLED, LOW);      delay(150);
      digitalWrite(boardLED,HIGH);      delay (200);
      digitalWrite(boardLED, LOW);      delay(100);
      digitalWrite(boardLED,HIGH);      delay (200);
      digitalWrite(boardLED, LOW);
      
      Serial.println(ACK); 
      break;
    }
    blinktimer++;
    if(blinktimer> 20000){
      board_led_status = board_led_status ? false: true;
      blinktimer = 0;
    }
    digitalWrite(boardLED, board_led_status? HIGH : LOW);
  }
    
  Serial.println(I_STARTED);    // Allow servers to know arduino just started..
 /* while(1){                     // Wait for server's ACK (S_STARTED)
    if(Serial.read()==S_STARTED){ connConfirmed = true; Serial.println(ACK);break;}
  }*/

  tone (pulsePin, 38000); // send to port 12 a pulse with frequency 38 KHz
  for(int i=0;i<encodedSize; i++) encoded[i] = -1; // init array
}

void loop() {
  if(Serial.available() > 0)
  {
    in = Serial.read();
    switch (in){
    case ARM_TO_RECORD:// ready to sense an IR signal
    digitalWrite(boardLED, HIGH);
      while(1){
        sensorValue = digitalRead(diPin); 
        if ((sensorValue ==1) /*&& (done == false)*/)
        {
          for(int i =0; i< takes; i++){
            delayMicroseconds(dt);
            dIn[i] = digitalRead(diPin);
          }
          blink(10);
        sensorValue = digitalRead(diPin); 

          bool Astate = 1, foundSpace = false;
          int encIndex = 0;
          for(int i=0; i<takes && !foundSpace; ){///i++){
           encoded[encIndex] = 0; // init the encoded counts to 0
           while(dIn[i] == Astate){
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
          //  Serial.println(dIn[j]);
          for (int j=0; j<encIndex; j++) {
            Serial.println(encoded[j]);
          }
          Serial.println("d");
          break;
        }
      } // DONE RECORDING
      digitalWrite(boardLED,LOW);
      break;
    case REPEAT:
      //while(1){
      if (done == true){
        //        Serial.println("R");
        for(int w=0; w < takes; w++){
          if (dIn[w] ==1 ) digitalWrite(LEDPin, HIGH);
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

      break;
 //   default:
   //   Serial.println("Unknown input");
    }
  }
}


