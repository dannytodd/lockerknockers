// TODO
// - record time with distance sensor
// - add monitor

// MONITOR PIN
unsigned int EchoPin = 2;
unsigned int TrigPin = 3;
const int StorePin = 5;
const int SwitchPin = 7; // magnet
const int Buzzer = 9; //buzzer to arduino pin 9

const int BluePin = 11; // storing knock
const int RedPin = 12; // failed knock attempt
const int GreenPin = 13; // successful knock attempt

bool Successful_Knock = false;
bool Door_Open = false;
float Stored_Knock[100] = {};
float Temp_Knock[10] = {};
int Temp_Knock_Count = 0;
bool Attempt_Finished = false;

unsigned long Time_Echo_us = 0;
//Len_mm_X100 = length*100
unsigned long Len_mm_X100 = 0;
unsigned long Len_Integer = 0; //
unsigned int Len_Fraction = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(EchoPin, INPUT);
  pinMode(TrigPin, OUTPUT);
  pinMode(StorePin, INPUT);
  pinMode(SwitchPin, INPUT);
  pinMode(Buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  pinMode(BluePin, OUTPUT);
  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);

  digitalWrite(SwitchPin, HIGH);
}

void resetTempKnock(){
  Serial.println("RESET TEMP KNOCK");
  // sizeof(Temp_Knock) TODO
  for( int i = 0; i < 10;  ++i ){
   Temp_Knock[i] = (char)0;    
  }

  Temp_Knock_Count = 0;
}

float readDistanceData(unsigned long Time_Echo_us){
  //Serial.println("READ DISTANCE DATA");
  Len_mm_X100 = (Time_Echo_us * 34) / 2;
    Len_Integer = Len_mm_X100 / 100;
    Len_Fraction = Len_mm_X100 % 100;
    if (Len_Integer > 200)
    {
      Len_Integer = 200;
    }
    return Len_Integer;
}

void resetState(){
  digitalWrite(GreenPin, LOW);
  digitalWrite(RedPin, LOW);
  digitalWrite(BluePin, LOW);  
  noTone(Buzzer); // stop sound
}

void setState(String state){
  resetState();
  Serial.println(state);
 
  if (state == "SUCCESS"){
    digitalWrite(GreenPin, HIGH);
    delay(600);
  }
  else if (state == "FAIL"){
    digitalWrite(RedPin, HIGH);
    tone(Buzzer, 100); // send 1KHz sound signal...
    delay(200);        // for 2 secONDS
  }
  else if (state == "READING"){
    digitalWrite(BluePin, HIGH);
    delay(500);
  }
}

void loop()
{
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  Time_Echo_us = pulseIn(EchoPin, HIGH);
  if ((Time_Echo_us < 60000) && (Time_Echo_us > 1))
  {
    float distance = readDistanceData(Time_Echo_us);

    if (distance < 150){
      if (Attempt_Finished) {
        resetTempKnock();
      }
      else {
        Temp_Knock[Temp_Knock_Count] = distance;
        Temp_Knock_Count += 1;
        // http://andybrown.me.uk/2011/01/15/the-standard-template-library-stl-for-avr-with-c-streams/
      }
    }
  
    // Store button pressed
    if (digitalRead(StorePin) == HIGH) {
      setState("READING");
//      knock = readKnock();
//      if (knockAttempt(knock)) {
//        // print NEW KNOCK
//        newKnock = readKnock();
//        storeKnock(newKnock);
//      }
//      else {
//        // print KNOCK ATTEMPT FAIL
//        digitalWrite(RedPin, HIGH);
//      }
      
    }
    // Door open
    if (digitalRead(SwitchPin) == HIGH) {
      if (Successful_Knock or distance < 100) {
        setState("SUCCESS");
      }
      else {
        setState("FAIL");
      }
    }
    else {
      resetState();
    }
    //Serial.print("Present Length is: ");
    //Serial.println(distance);
    //Serial.print(".");
    //if(Len_Fraction < 10)
    //Serial.print("0");
    //Serial.print(Len_Fraction, DEC);
    //Serial.println("mm");


  }
  delay(10);
}



void readKnock()
{
  float knock[] = {};
  
}

//void storeKnock()
//{
//  int knock = readKnock()
//  processedKnock = processKnock(knock)
//  
//  Stored_Knock = processed_knock
//}
//
//int[] processKnock(int rawKnock)
//{
//  int[] knockTroughTimes = getKnockTroughs(rawKnock)
//  abstractKnocks = abstractifyKnocks(knockTroughTimes) 
//}
//
//int[] getKnockTroughs(int knock)
//{
//  // returns a list of lower bounds of knock distances by their timestamp  
//}
//
//int[] abstractifyKnocks(int knock)
//{
//  // return list of abstract multipliers for each knock e.g. [1, 1.243, 0.9435, 1.5435]
//  //                                                          ^ 1 because the first knock is compared to by the rest
//}
//
//void knockAttempt()
//{
//  int knock = 0
// if (compareKnockToStored(knock)){
//  SUCCESS 
// }
// else{
//  FAIL
// }
//}
//
//bool compareKnockToStored(int knock)
//{
//  if(checkStoredKnockLimit()) // limit of literal times
//  
//  return true
//}



