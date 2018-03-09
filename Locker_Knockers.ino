unsigned int EchoPin = 2;
unsigned int TrigPin = 3;
const int switchPin = 7; // magnet
const int buzzer = 9; //buzzer to arduino pin 9
const int bluePin = 11; // storing knock
const int redPin = 12; // failed knock attempt
const int greenPin = 13; // successful knock attempt

bool Successful_Knock = false;

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
 pinMode(switchPin, INPUT);
 digitalWrite(switchPin, HIGH);
 pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
}
void loop()
{  
 digitalWrite(TrigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(TrigPin, LOW);

 Time_Echo_us = pulseIn(EchoPin, HIGH);
 if((Time_Echo_us < 60000) && (Time_Echo_us > 1))
 {
 Len_mm_X100 = (Time_Echo_us*34)/2;
 Len_Integer = Len_mm_X100/100;
 Len_Fraction = Len_mm_X100%100;
 if(Len_Integer > 400)
 {
 Len_Integer = 400;
 }
 if(digitalRead(switchPin) == HIGH){
   if(Successful_Knock){
    // SUCCESS
    digitalWrite(greenPin, HIGH);
   }
   else {
    // FAIL
    digitalWrite(redPin, HIGH);
    tone(buzzer, 100); // Send 1KHz sound signal...
    delay(200);        // ...for 1 sec
   }
 }
 noTone(buzzer);     // Stop sound...
 //Serial.print("Present Length is: ");
 Serial.println(Len_Integer);
 //Serial.print(".");
 //if(Len_Fraction < 10)
 //Serial.print("0");
 //Serial.print(Len_Fraction, DEC);
 //Serial.println("mm");


 }
 delay(10);
}
