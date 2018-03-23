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

float Stored_Knock[10] = {};

float Temp_Knock[100] = {};
int Temp_Knock_Count = 0;
bool Attempt_Finished = false;
bool Hand_Moved_Away = false;

unsigned long Time_Echo_us = 0;
//Len_mm_X100 = length*100
unsigned long Len_mm_X100 = 0;
unsigned long Len_Integer = 0; //
unsigned int Len_Fraction = 0;

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

void loop()
{
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  Time_Echo_us = pulseIn(EchoPin, HIGH);
  if ((Time_Echo_us < 60000) && (Time_Echo_us > 1))
  {
    float distance = readDistanceData();

    knockStep(Temp_Knock, distance);

    // Door open
    if (digitalRead(SwitchPin) == HIGH) {
      if (Successful_Knock or distance < 100) {
        setState("SUCCESS");
      }
      else {
        setState("FAIL");
      }
    }
    // Store button pressed
    else if (digitalRead(StorePin) == HIGH) {
      float knockPasswordAttempt[10] = {};
      setState("READING");
      readWholeKnock(knockPasswordAttempt);
      Serial.println(knockPasswordAttempt);

      if (knockAttempt(knockPasswordAttempt)) {
        Serial.println("NEW KNOCK");
        float newKnock[10] = {};
        readWholeKnock(newKnock);
        storeKnock(newKnock);
      }
      else {
        digitalWrite(RedPin, HIGH);
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
  Serial.print("");
  delay(30);
}
//// END ////

//// TEMPORARY KNOCK STORE
void resetTempKnock() {
  Serial.println("RESET TEMP KNOCK");
  // sizeof(Temp_Knock) TODO
  for ( int i = 0; i < 10;  ++i ) {
    Temp_Knock[i] = (char)0;
  }

  Temp_Knock_Count = 0;
}

void storeKnock(float processedKnock[10])
{
  memcpy( Stored_Knock, processKnock, 5 * sizeof(int) );
}

//// KNOCK READING
float readDistanceData() {
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

void knockStep(float knockArray[100], int distance)
{
  if (distance < 150) {
    if (Attempt_Finished and Hand_Moved_Away) {   // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE
      resetTempKnock();
    }
    else if (Hand_Moved_Away) {
      Attempt_Finished = true;
    }
    else if (Attempt_Finished == false) {
      knockArray[Temp_Knock_Count] = distance;
      knockArray += 1;
      if (Temp_Knock_Count >= 100) {
        Attempt_Finished = true;
      }
      // http://andybrown.me.uk/2011/01/15/the-standard-template-library-stl-for-avr-with-c-streams/
    }
  }
  else {
    Hand_Moved_Away = true;
  }
}

void readWholeKnock(float knock[100])
{
  // scans whole knock and transfers to Temp_Knock
  if ((Time_Echo_us < 60000) && (Time_Echo_us > 1))
  {
    for (int i = 0; i < 100; i++) {
      float distance = readDistanceData();
      knockStep(knock, distance);
    }
    processKnock(knock);
  }
}

//// KNOCK PROCESSING
void processKnock(float rawKnock[100])
{
  // list of times in seconds
  float knockTroughTimes[10] = {};
  getKnockTroughs(rawKnock, knockTroughTimes);
  float abstractKnock[10] = {};
  abstractifyKnocks(abstractKnock, knockTroughTimes);
}

void getKnockTroughs(float knock[100], float knockTroughTimes[10])
{
  // returns a list of lower bounds of knock distances by their timestamp
}

void abstractifyKnocks(float abstractKnock[10], float knockTroughTimes[10])
{
  // return list of abstract multipliers for each knock e.g. [1, 1.243, 0.9435, 1.5435]
  //                                                          ^ 1 because the first knock is compared to by the rest
}

//// KNOCK PASSWORD ATTEMPT
bool knockAttempt(float knock[10])
{
  if (compareKnockToStored(knock)) {
    Serial.println("KNOCK ATTEMPT SUCCESS");
    return true;
  }
  else {
    Serial.println("KNOCK ATTEMPT FAIL");
    return false;
  }
}

bool compareKnockToStored(float knock[10])
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
  Serial.println(state);

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
}








