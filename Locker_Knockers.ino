#include <Time.h>
#include <TimeLib.h>

// TODO
// - storing knock to data store, could be done by chaning state to STORING and using main loop for ease
// - knock store comparing
// - data store
// 
//MOVING AVERAGE  https://www.dallasfed.org/research/basics/moving.aspx


// BUGS
// - DISTANCES: [133, 125, 140, 126, 132, 94, 84, 83, 102, 84, 85, 83, 78, 114, 97, 100, 142, 141, 100, 136, 122, 106, 82, 77, 82, 86, 81, 77, 83, 112, 137, 127, 112, 116, 106, 93, 95, 147, 131, 122, 113, 111, 141, 106, 130, 98, 92, 105, 110, 134, 149, 133, 130, 135, 131, 120, 124, 147, 144, 121, 129, 140, 143, 133, 139, 118, 98, 99, 107, 114, 109, 99, 97, 83, 80, 81, 86, 91, 93, 100, 125, 145, 126, 117, 131, 136, 114, 111, 113, 125, 130, 122, 105, 102, 116, 120, 116, 121, 129, 112]
//   TIMES:     [0, 12, 24, 357, 369, 381, 393, 404, 416, 428, 440, 453, 472, 492, 511, 530, 551, 570, 891, 903, 915, 927, 939, 951, 962, 977, 996, 1014, 1033, 1511, 1957, 1969, 1980, 1992, 2005, 2016, 2028, 2234, 2557, 2569, 2581, 2593, 2606, 2617, 2720, 2732, 2744, 2756, 3247, 3440, 3796, 3808, 3820, 3832, 3936, 3947, 3959, 4413, 4438, 4643, 4655, 4667, 4989, 5001, 5013, 5116, 5128, 5139, 5151, 5575, 5588, 5599, 5611, 5623, 5635, 5647, 5659, 5678, 5697, 5717, 5736, 6165, 6189, 6201, 6213, 6225, 6855, 6958, 6970, 7509, 7521, 7532, 7636, 7648, 7659, 7671, 7696, 7707, 7720, 8179]
//                   ^^^^ low times as knock is picked up again and carries on after hand is moved back
//  ^^^^^ this might be good actually, means the hand doesn't have to be there the whole time

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

int Stored_Knock[10] = {};

int Temp_Knock_Distances[100] = {};
int Temp_Knock_Times[100] = {};
int Temp_Knock_Count = 0;
int Temp_Time_Start = 0;

bool Attempt_Finished = false;
bool Hand_Moved_Away = false;

unsigned long Time_Echo_us = 0;
//Len_mm_X100 = length*100
unsigned long Len_mm_X100 = 0;
unsigned long Len_Integer = 0; //
unsigned int Len_Fraction = 0;

// TESTING
void loop(){
  int dists[100] = {133, 125, 140, 126, 132, 94, 84, 83, 102, 84, 85, 83, 78, 114, 97, 100, 142, 141, 100, 136, 122, 106, 82, 77, 82, 86, 81, 77, 83, 112, 137, 127, 112, 116, 106, 93, 95, 147, 131, 122, 113, 111, 141, 106, 130, 98, 92, 105, 110, 134, 149, 133, 130, 135, 131, 120, 124, 147, 144, 121, 129, 140, 143, 133, 139, 118, 98, 99, 107, 114, 109, 99, 97, 83, 80, 81, 86, 91, 93, 100, 125, 145, 126, 117, 131, 136, 114, 111, 113, 125, 130, 122, 105, 102, 116, 120, 116, 121, 129, 112};
  int times[100] = {0, 12, 24, 357, 369, 381, 393, 404, 416, 428, 440, 453, 472, 492, 511, 530, 551, 570, 891, 903, 915, 927, 939, 951, 962, 977, 996, 1014, 1033, 1511, 1957, 1969, 1980, 1992, 2005, 2016, 2028, 2234, 2557, 2569, 2581, 2593, 2606, 2617, 2720, 2732, 2744, 2756, 3247, 3440, 3796, 3808, 3820, 3832, 3936, 3947, 3959, 4413, 4438, 4643, 4655, 4667, 4989, 5001, 5013, 5116, 5128, 5139, 5151, 5575, 5588, 5599, 5611, 5623, 5635, 5647, 5659, 5678, 5697, 5717, 5736, 6165, 6189, 6201, 6213, 6225, 6855, 6958, 6970, 7509, 7521, 7532, 7636, 7648, 7659, 7671, 7696, 7707, 7720, 8179};
  processKnock(dists, times);
}

//// MAIN ////
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

void lodop()
{
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  Time_Echo_us = pulseIn(EchoPin, HIGH);
  if ((Time_Echo_us < 60000) && (Time_Echo_us > 1))
  {
    int distance = readDistanceData();

    knockStep(distance);

    if (Attempt_Finished) {
      
    }
    // Door open
    if (digitalRead(SwitchPin) == HIGH) {
      processKnock(Temp_Knock_Distances, Temp_Knock_Times);
      if (Successful_Knock) {
        setState("SUCCESS");
      }
      else {
        setState("FAIL");
      }
    }
    // Store button pressed
    else if (digitalRead(StorePin) == HIGH) {
      int knockPasswordAttempt[10] = {};
      setState("READING");
      readWholeKnock(knockPasswordAttempt);
      //Serial.print("ATTEMPT: ");
      print100Array(knockPasswordAttempt);
      if (knockAttempt(knockPasswordAttempt)) {
        //Serial.println("NEW KNOCK");
        int newKnock[10] = {};
        readWholeKnock(newKnock);
        storeKnock(newKnock);
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
  //Serial.print("");
  delay(10);
}
//// END ////

//// TEMPORARY KNOCK STORE
void resetTempKnock() {
  //Serial.println("RESET TEMP KNOCK");
  // sizeof(Temp_Knock) TODO
  for ( int i = 0; i < 10;  ++i ) {
    Temp_Knock_Distances[i] = (char)0;
    Temp_Knock_Times[i] = (char)0;
  }

  Temp_Knock_Count = 0;
  Temp_Time_Start = 0;
  Attempt_Finished = false;
}

void storeKnock(int processedKnock[10])
{
  memcpy( Stored_Knock, processKnock, 5 * sizeof(int) );
}

//// KNOCK READING
int readDistanceData() {
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

void knockStep(int distance)
{
  // Hand within distance
  if (distance < 200) {
    Hand_Moved_Away = false;
    // Buzz to simulate knocking sound
    if (distance < 100) {
      //tone(Buzzer, 50);
      //delay(200);        
    }
    // Partial knock interrupted
    if (Attempt_Finished and Temp_Knock_Count < 100) {
      
    }
    // Hand within distance again after knock attempt => reset stored knock
    if (Attempt_Finished) {
      resetTempKnock();
    }
    else if (Hand_Moved_Away and Temp_Knock_Count >= 100) {
      Attempt_Finished = true;
    }
    // Dynamic recording
    if (Attempt_Finished == false) {
      if (Temp_Knock_Count == 0) {
        Temp_Time_Start = millis();
      }
      // Knock read continuing 
      if (Temp_Knock_Count < 100) {
        //Serial.print("#");
        //Serial.print(Temp_Knock_Count);
        //Serial.print(" Distance: ");
        Serial.println(distance, DEC);
        
        Temp_Knock_Distances[Temp_Knock_Count] = distance;
        Temp_Knock_Times[Temp_Knock_Count] = millis() - Temp_Time_Start;
        Temp_Knock_Count += 1;
      }
      // Knock finished
      else if (Temp_Knock_Count >= 100) {
        //Serial.print("DISTANCES: ");
        //print100Array(Temp_Knock_Distances);
        //Serial.print("TIMES: ");
        //print100Array(Temp_Knock_Times);
        
        Attempt_Finished = true;
        setState("DONE");
        delay(5000);
      }
      // http://andybrown.me.uk/2011/01/15/the-standard-template-library-stl-for-avr-with-c-streams/
    }
  }
  else {
    Hand_Moved_Away = true;
  }
}

void readWholeKnock(int knock[100])
{
  // scans whole knock and transfers to Temp_Knock
  if ((Time_Echo_us < 60000) && (Time_Echo_us > 1))
  {
    for (int i = 0; i < 100; i++) {
      int distance = readDistanceData();
      knockStep(distance);    // needs to be passed an array
    }
    //processKnock(knock);
  }
}

//// KNOCK PROCESSING
void processKnock(int rawKnockDistances[100], int rawKnockTimes[100])
{
  applyMovingAverage(rawKnockDistances);
  
  // list of times in seconds
  int knockTroughTimes[10] = {546, 1432, 3097, 4004, 4569, 5532, 6032, 7184, 8634, 10023};  // MARKS CODE !!!!!!!!!!!!!!!!
  //getKnockTroughs(rawKnockDistances, rawKnockTimes, knockTroughTimes);
  
  float abstractKnock[10] = {};
  abstractifyKnocks(abstractKnock, knockTroughTimes);
}

// smooth graph by applying Moving Average Algorithm
void applyMovingAverage(int rawKnockDistances[100]){
  for (int i = 0; i < 98; i++){                                                                     //       /\       //
    int average = abs(rawKnockDistances[i] + rawKnockDistances[i+2])/2;                            //     \x/  \     /
    rawKnockDistances[i+1] = average;                                                             //          X \   /  <= Applies average of two neighbouring
  }                                                                                              //              \x/      points to smooth graph data.       
  Serial.print("SMOOTHED: ");
  print100Array(rawKnockDistances);
}

// returns a list of lower bounds of knock distances by their timestamp
void getKnockTroughs(int rawKnockDistances[100], int rawKnockTimes[100], int knockTroughTimes[10])
{
  int knockCount = 0;
  for (int i = 2; i <= 98; i++) {
    int iMinus2 = rawKnockDistances[i-2];
    int iMinus1 = rawKnockDistances[i-1];
    int iBase = rawKnockDistances[i];
    int iPlus1 = rawKnockDistances[i+1];
    int iPlus2 = rawKnockDistances[i+2];

    if((iMinus2 > iBase) && (iMinus2 > iMinus1) && (iMinus1 > iBase) && (iPlus1 > iBase) && (iPlus2 > iPlus1) && (iPlus2 > iBase)){

      knockTroughTimes[knockCount] = rawKnockTimes[i];

      knockCount++;
    }
  }
}

void abstractifyKnocks(float abstractKnock[10], int knockTroughTimes[10])
{
  // return list of abstract multipliers for each knock e.g. [1, 1.243, 0.9435, 1.5435]
  //                                                          ^ 1 because the first knock is compared to by the rest
  abstractKnock[0] = 1;
  for (int i = 0; i < 10; i++) {
    abstractKnock[i+1] = ((float)knockTroughTimes[i+1] - (float)knockTroughTimes[i]) / ((float)knockTroughTimes[i] - (float)knockTroughTimes[i-1]);
  }
  Serial.print("ABSTRACT: ");
  print10Array(abstractKnock);
  delay(100000);
}

//// KNOCK PASSWORD ATTEMPT
bool knockAttempt(int knock[10])
{
  if (compareKnockToStored(knock)) {
    //Serial.println("KNOCK ATTEMPT SUCCESS");
    return true;
  }
  else {
    //Serial.println("KNOCK ATTEMPT FAIL");
    return false;
  }
}

bool compareKnockToStored(int knock[10])
{
  //if(checkStoredKnockLimit()) // limit of literal times

  return false;
}

//// STATE MACHINE
void resetState() {
  digitalWrite(GreenPin, LOW);
  digitalWrite(RedPin, LOW);
  digitalWrite(BluePin, LOW);
  noTone(Buzzer); // stop sound
}

void setState(String state) {
  resetState();
  //Serial.println(state);

  if (state == "SUCCESS") {
    digitalWrite(GreenPin, HIGH);
    delay(600);
  }
  else if (state == "FAIL") {
    digitalWrite(RedPin, HIGH);
    tone(Buzzer, 100); // send 1KHz sound signal...
    delay(200);        // for 2 secONDS
  }
  else if (state == "READING") {
    digitalWrite(BluePin, HIGH);
    delay(500);
  }
  else if (state == "DONE") {
    digitalWrite(GreenPin, HIGH);
    digitalWrite(RedPin, HIGH);
    digitalWrite(BluePin, HIGH);
    delay(500);
  }
}

//// PRINT STUFF
void print10Array(float myArray[10]) {
  Serial.print("[");
  for (int i = 0; i < 10; i++)
  {
    Serial.print(myArray[i]);
    if (i == 9) {
      Serial.print("]");
    }
    else {
      Serial.print(", ");
    }
  }
  Serial.println("");
}

void print100Array(int myArray[100]) {
  Serial.print("[");
  for (int i = 0; i < 100; i++)
  {
    Serial.print(myArray[i]);
    if (i == 99) {
      Serial.print("]");
    }
    else {
      Serial.print(", ");
    }
  }
  Serial.println("");
}






