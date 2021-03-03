/* 
****** Volvo PentaTwin  KAD/KAMD 43 tachometer digital read *****
  
  This is an solution for digital reading of engines rpm where there is an pulsed signal delivered to tachometer.
  For Readme file and for updates & similar codes go to https://github.com/Nesse1/VP-Compressor-controller 

  Tachometer signal/puls is delivered from the generators fase W.
  Signal is 12 V so voltage devider is used.
  The interesting data for describing signal found among the suppliers information is the calculation 
  factor that spesify that generator deliveres 14,7 pulses per engine revolution.
  I`m using this information to calculate the pulses recieved within engins rpm working area....
   
  How does the range of pulses from my engines look:
  Number of pulses/min at idle or 750/rpm:             750 * 14,7 = 11025 pulses/min at  750 rpm
  Number of pulses/min at mid thrittle or 2500/rpm    2500 * 14,7 = 36750 pulses/min at 2500 rpm
  Number of pulses/min at full throttle or 3900/rpm:  3900 * 14,7 = 57330 pulses/min at 3900 rpm

  How many pulses per engine shaft revolution:
  Number of pulses/second at idle or 750/rpm:           11025 / 60 = 183,75 Hz or pulses/sec
  Number of pulses/second at mid thrittle or 2500/rpm   36750 / 60 = 612,5 Hz or pulses/sec
  Number of pulses/second at full throttle or 3900/rpm: 57330 / 60 = 955,5 Hz or pulses/sec

  How many microseconds from start to end of puls (HIGH & LOW):
  Length puls at idle or 750/rpm:                       1000000 / 183,75 = 5442 micros
  Length puls at mid throttle or 2500/rpm:              1000000 / 612,5  = 1633 micros
  Lenght puls at full throttle or 3900/rpm:             1000000 / 955,5  = 1047 micros
 
 */

//For smoother rpmEngine_1 values
const int numReadingsRpm_Engine1 = 200;       // How many pulses u calculate rpm average from, i fond that 200 still updates fast enough
const int numReadingsRpm_Engine1Stat = 100;   // How many rpm u calculate average for statusupdate from
const int StatusDelay_Engine1 = 4000;         // Set an delay in millis for status to change, this delay avoids compressor for rapidly turning on/off
const int StatusTollerance_Engine_1 = 45;     // Tolerance for how many +/- difference in averageRpm_Engine1Stat before statuschange to accure. Higher number stabilizes Status 2 (rpm stabile) but then it also takes longer time for statuschange when rpm increasing/decreasing.
const int CompressorActivateLimitEngine_1 = 2000;   // Status will not change to 3 (increasing rpm)and activate compressor if rpm is over this.

//For smoother rpmEngine_2 values
const int numReadingsRpm_Engine2 = 200;             // How many pulses u calculate rpm average from, i fond that 200 still updates fast enough
const int numReadingsRpm_Engine2Stat = 100;         // How many rpm u calculate average for statusupdate from
const int StatusDelay_Engine2 = 4000;               // Set an delay in millis for status to change, this delay avoids compressor for rapidly turning on/off
const int StatusTollerance_Engine_2 = 45;           // Tolerance for how many +/- difference in averageRpm_Engine2Stat before statuschange to accure. Higher number stabilizes Status 2 (rpm stabile) but then it also takes longer time for statuschange when rpm increasing/decreasing.
const int CompressorActivateLimitEngine_2 = 2000;   // Status will not change to 3 (increasing rpm)and activate compressor if rpm is over this.


//Array for reading average of pulses for rpmEngine_1
int readingsRpm_Engine1[numReadingsRpm_Engine1];    
int thisReadingRpm_Engine1 = 0;
int readIndexRpm_Engine1 = 0;
int totalRpm_Engine1 = 0;
int averageRpm_Engine1 = 0; 
//Array for reading average of pulses for rpmEngine_2
int readingsRpm_Engine2[numReadingsRpm_Engine2];    
int thisReadingRpm_Engine2 = 0;
int readIndexRpm_Engine2 = 0;
int totalRpm_Engine2 = 0;
int averageRpm_Engine2 = 0; 

//Array for reading average rpmEngine_1 for Status
int readingsRpm_Engine1Stat[numReadingsRpm_Engine1];    
int thisReadingRpm_Engine1Stat = 0;
int readIndexRpm_Engine1Stat = 0;
int totalRpm_Engine1Stat = 0;
int averageRpm_Engine1Stat = 0; 
//Array for reading average rpmEngine_2 for Status
int readingsRpm_Engine2Stat[numReadingsRpm_Engine2];    
int thisReadingRpm_Engine2Stat = 0;
int readIndexRpm_Engine2Stat = 0;
int totalRpm_Engine2Stat = 0;
int averageRpm_Engine2Stat = 0; 
// declare rpm Engine 1 Status data holder
int rpmEngine_1Status = 0;
// declare rpm Engine 2 Status data holder
int rpmEngine_2Status = 0;


//Values that U might have to change for Engine 1:
int tachoinputPinEngine_1 = 5;              // Specify pin for input signal from generator
int relayoutputPinEngine_1 = 2;            // Specify pin for output signal to relay
int ledPinGreenEngine_1 = 0;           //Used for debugging and for testing status when connected to engine, defines pin used for led
int ledPinRedEngine_1 = 4;             //Used for debugging and for testing status when connected to engine, defines pin used for led
int pulsesPerRevEngine_1 = 147;             // 14.7 * 10 (so we include desimal after dot) - Factor for calculating, number of pulses from generator pr engine revolution 
int correctingRevEngine_1 = 20;             // For eventual correction of output rpm, i found that 20 (2.0%) gives me more acurate readings according to tachometer in boat.

//Values that U might have to change for Engine 2:
int tachoinputPinEngine_2 = 16;              // Specify pin for input signal from generator
int relayoutputPinEngine_2 = 13;            // Specify pin for output signal to relay
int ledPinGreenEngine_2 = 14;           //Used for debugging and for testing status when connected to engine, defines pin used for led
int ledPinRedEngine_2 = 12;             //Used for debugging and for testing status when connected to engine, defines pin used for led
int pulsesPerRevEngine_2 = 147;             // 14.7 * 10 (so we include desimal after dot) - Factor for calculating, number of pulses from generator pr engine revolution 
int correctingRevEngine_2 = 20;             // For eventual correction of output rpm, i found that 20 (2.0%) gives me more acurate readings according to tachometer in boat.

// Pulse signal from generator Engine 1 is full og ripples, so by setting max/min pulse allowed code will only read relevant and "clean" pulses.
int lowestPulseAllowedEngine_1 = 900;      // Lowes pulse allowed - At 3900 rpm pulse is 1047 micros, so shorter pulses are most lightly ripples or an uncorrect pulse
int highestPulseAllowedEngine_1 = 6000;     // Highest pulse allowed - At 750 rpm pulse is 5442 micros, so longer pulses are most lightly ripples or an uncorrect pulse
// Pulse signal from generator Engine 2 is full og ripples, so by setting max/min pulse allowed code will only read relevant and "clean" pulses.
int lowestPulseAllowedEngine_2 = 900;      // Lowes pulse allowed - At 3900 rpm pulse is 1047 micros, so shorter pulses are most lightly ripples or an uncorrect pulse
int highestPulseAllowedEngine_2 = 6000;     // Highest pulse allowed - At 750 rpm pulse is 5442 micros, so longer pulses are most lightly ripples or an uncorrect pulse


//For calculating length of pulse Engine 1 we need to know both HIGH and LOW lenght of pulse
unsigned long highpulseDurationEngine_1;    // to hold the data for how long duration high part of pulse is
unsigned long lowpulseDurationEngine_1;     // to hold the data for how long duration low part of pulse is
unsigned long totalRpmDurationEngine_1;     // to hold the data for how long duration high + low part of pulse is
unsigned long rpmEngine_1;                  // to hold the data for rpmEngine_1
unsigned long millisEngine_1Stat_1;         // for holding millis data for rpmEngine_1Status_1
unsigned long millisEngine_1Stat_2;         // for holding millis data for rpmStatus_2
unsigned long millisEngine_1Stat_3;         // for holding millis data for rpmStatus_3

//For calculating length of pulse Engine 2 we need to know both HIGH and LOW lenght of pulse
unsigned long highpulseDurationEngine_2;    // to hold the data for how long duration high part of pulse is
unsigned long lowpulseDurationEngine_2;     // to hold the data for how long duration low part of pulse is
unsigned long totalRpmDurationEngine_2;     // to hold the data for how long duration high + low part of pulse is
unsigned long rpmEngine_2;                  // to hold the data for rpmEngine_1
unsigned long millisEngine_2Stat_1;         // for holding millis data for rpmEngine_1Status_1
unsigned long millisEngine_2Stat_2;         // for holding millis data for rpmStatus_2
unsigned long millisEngine_2Stat_3;         // for holding millis data for rpmStatus_3


void setup() {

  // start serial:
Serial.begin(115200);

//Array for reading pulses for average rpmEngine_1
for (thisReadingRpm_Engine1 = 0; thisReadingRpm_Engine1 < numReadingsRpm_Engine1; thisReadingRpm_Engine1++){
  readingsRpm_Engine1[thisReadingRpm_Engine1] = 0;
  }

//Array for reading pulses for average rpmEngine_2
for (thisReadingRpm_Engine2 = 0; thisReadingRpm_Engine2 < numReadingsRpm_Engine2; thisReadingRpm_Engine2++){
  readingsRpm_Engine2[thisReadingRpm_Engine2] = 0;
  }

//Array for reading rpm Engine_1 for new average for updating Status
for (thisReadingRpm_Engine1Stat = 0; thisReadingRpm_Engine1Stat < numReadingsRpm_Engine1Stat; thisReadingRpm_Engine1Stat++){
  readingsRpm_Engine1Stat[thisReadingRpm_Engine1Stat] = 0;
  }

//Array for reading rpm Engine_2 for new average for updating Status
for (thisReadingRpm_Engine2Stat = 0; thisReadingRpm_Engine2Stat < numReadingsRpm_Engine2Stat; thisReadingRpm_Engine2Stat++){
  readingsRpm_Engine2Stat[thisReadingRpm_Engine2Stat] = 0;
  }

//Define pinmode Engine 1  
pinMode(tachoinputPinEngine_1, INPUT);    //Define pin as INPUT
pinMode(ledPinGreenEngine_1, OUTPUT);
pinMode(ledPinRedEngine_1, OUTPUT);
pinMode(relayoutputPinEngine_1, OUTPUT);

//Define pinmode Engine 2  
pinMode(tachoinputPinEngine_2, INPUT);    //Define pin as INPUT
pinMode(ledPinGreenEngine_2, OUTPUT);
pinMode(ledPinRedEngine_2, OUTPUT);
pinMode(relayoutputPinEngine_2, OUTPUT);

}


void loop() {

unsigned long currentMillis = millis();   

  
//Get duration of Engine 1 pulses HIG & LOW
highpulseDurationEngine_1 = pulseIn(tachoinputPinEngine_1, HIGH); //Returns duration of state HIGH in microseconds
lowpulseDurationEngine_1 = pulseIn(tachoinputPinEngine_1, LOW);   //Returns duration of state LOW in microseconds

//Get duration of Engine 2 pulses HIG & LOW
highpulseDurationEngine_2 = pulseIn(tachoinputPinEngine_2, HIGH); //Returns duration of state HIGH in microseconds
lowpulseDurationEngine_2 = pulseIn(tachoinputPinEngine_2, LOW);   //Returns duration of state LOW in microseconds


//Calculate totalRpm_Engine1 1 puls duration im microseconds
if(((highpulseDurationEngine_1 > (lowestPulseAllowedEngine_1 / 2)) & (lowpulseDurationEngine_1 > (lowestPulseAllowedEngine_1 / 2)))& ((highpulseDurationEngine_1 < (highestPulseAllowedEngine_1 /2)) & (lowpulseDurationEngine_1 < (highestPulseAllowedEngine_1 / 2)))){

  totalRpmDurationEngine_1 = (highpulseDurationEngine_1 + lowpulseDurationEngine_1) + (((highpulseDurationEngine_1 + lowpulseDurationEngine_1) * correctingRevEngine_1) / 1000); // Adding hig & low pulses and correctionRev
}
if(((highpulseDurationEngine_1 < (lowestPulseAllowedEngine_1 / 2)) & (lowpulseDurationEngine_1 < (lowestPulseAllowedEngine_1 / 2)))& ((highpulseDurationEngine_1 > (highestPulseAllowedEngine_1 /2)) & (lowpulseDurationEngine_1 > (highestPulseAllowedEngine_1 / 2)))){
(totalRpmDurationEngine_1 = 0);        //If there is no good signal so set rpmEngine_1 to 0
}

//Calculate totalRpm_Engine1 2 puls duration im microseconds
if(((highpulseDurationEngine_2 > (lowestPulseAllowedEngine_2 / 2)) & (lowpulseDurationEngine_2 > (lowestPulseAllowedEngine_2 / 2)))& ((highpulseDurationEngine_2 < (highestPulseAllowedEngine_2 /2)) & (lowpulseDurationEngine_2 < (highestPulseAllowedEngine_2 / 2)))){

  totalRpmDurationEngine_2 = (highpulseDurationEngine_2 + lowpulseDurationEngine_2) + (((highpulseDurationEngine_2 + lowpulseDurationEngine_2) * correctingRevEngine_2) / 1000); // Adding hig & low pulses and correctionRev
}
if(((highpulseDurationEngine_2 < (lowestPulseAllowedEngine_2 / 2)) & (lowpulseDurationEngine_2 < (lowestPulseAllowedEngine_2 / 2)))& ((highpulseDurationEngine_2 > (highestPulseAllowedEngine_2 /2)) & (lowpulseDurationEngine_2 > (highestPulseAllowedEngine_2 / 2)))){
(totalRpmDurationEngine_2 = 0);        //If there is no good signal so set rpmEngine_2 to 0
}

//Calculate rpmEngine_1
if(totalRpmDurationEngine_1 < 10){     //If no signal we want rpmEngine_1 to be 0
  rpmEngine_1 = 0;
}
else{
rpmEngine_1 = (((1000000 / totalRpmDurationEngine_1) * 60) / pulsesPerRevEngine_1) * 10;  //Readjusting the factor where we took 14,7 * 10 to include desimal behind dot
}

//Calculate rpmEngine_2
if(totalRpmDurationEngine_2 < 10){     //If no signal we want rpmEngine_1 to be 0
  rpmEngine_2 = 0;
}
else{
rpmEngine_2 = (((1000000 / totalRpmDurationEngine_2) * 60) / pulsesPerRevEngine_2) * 10;  //Readjusting the factor where we took 14,7 * 10 to include desimal behind dot
}


//Array - For smoother rpmEngine_1 values calculate average:
 // subtract the last reading:
  totalRpm_Engine1 = totalRpm_Engine1 - readingsRpm_Engine1[readIndexRpm_Engine1];
  // read from the sensor:
  readingsRpm_Engine1[readIndexRpm_Engine1] = rpmEngine_1;
  // add the reading to the totalRpm_Engine1:
  totalRpm_Engine1 = totalRpm_Engine1 + readingsRpm_Engine1[readIndexRpm_Engine1];
  // advance to the next position in the array:
  readIndexRpm_Engine1 = readIndexRpm_Engine1 + 1;
  // if we're at the end of the array...
  if (readIndexRpm_Engine1 >= numReadingsRpm_Engine1) {
    // ...wrap around to the beginning:
    readIndexRpm_Engine1 = 0;
  }
  // calculate the averageRpm_Engine1:
  averageRpm_Engine1 = totalRpm_Engine1 / numReadingsRpm_Engine1;

//Array - For smoother rpmEngine_2 values calculate average:
 // subtract the last reading:
  totalRpm_Engine2 = totalRpm_Engine2 - readingsRpm_Engine2[readIndexRpm_Engine2];
  // read from the sensor:
  readingsRpm_Engine2[readIndexRpm_Engine2] = rpmEngine_2;
  // add the reading to the totalRpm_Engine1:
  totalRpm_Engine2 = totalRpm_Engine2 + readingsRpm_Engine2[readIndexRpm_Engine2];
  // advance to the next position in the array:
  readIndexRpm_Engine2 = readIndexRpm_Engine2 + 1;
  // if we're at the end of the array...
  if (readIndexRpm_Engine2 >= numReadingsRpm_Engine2) {
    // ...wrap around to the beginning:
    readIndexRpm_Engine2 = 0;
  }
  // calculate the averageRpm_Engine2:
  averageRpm_Engine2 = totalRpm_Engine2 / numReadingsRpm_Engine2;



//Array - For smoother Engine 1 rpmStatus values we calculate average:
 // subtract the last reading:
  totalRpm_Engine1Stat = totalRpm_Engine1Stat - readingsRpm_Engine1Stat[readIndexRpm_Engine1Stat];
  // read from the sensor:
  readingsRpm_Engine1Stat[readIndexRpm_Engine1Stat] = averageRpm_Engine1;
  // add the reading to the totalRpm_Engine1:
  totalRpm_Engine1Stat = totalRpm_Engine1Stat + readingsRpm_Engine1Stat[readIndexRpm_Engine1Stat];
  // advance to the next position in the array:
  readIndexRpm_Engine1Stat = readIndexRpm_Engine1Stat + 1;
  // if we're at the end of the array...
  if (readIndexRpm_Engine1Stat >= numReadingsRpm_Engine1Stat) {
    // ...wrap around to the beginning:
    readIndexRpm_Engine1Stat = 0;
  }
  // calculate the averageRpm_Engine1:
  averageRpm_Engine1Stat = totalRpm_Engine1Stat / numReadingsRpm_Engine1Stat;

//Array - For smoother Engine 2 rpmStatus values we calculate average:
 // subtract the last reading:
  totalRpm_Engine2Stat = totalRpm_Engine2Stat - readingsRpm_Engine2Stat[readIndexRpm_Engine2Stat];
  // read from the sensor:
  readingsRpm_Engine2Stat[readIndexRpm_Engine2Stat] = averageRpm_Engine2;
  // add the reading to the totalRpm_Engine1:
  totalRpm_Engine2Stat = totalRpm_Engine2Stat + readingsRpm_Engine2Stat[readIndexRpm_Engine2Stat];
  // advance to the next position in the array:
  readIndexRpm_Engine2Stat = readIndexRpm_Engine2Stat + 1;
  // if we're at the end of the array...
  if (readIndexRpm_Engine2Stat >= numReadingsRpm_Engine2Stat) {
    // ...wrap around to the beginning:
    readIndexRpm_Engine2Stat = 0;
  }
  // calculate the averageRpm_Engine2:
  averageRpm_Engine2Stat = totalRpm_Engine2Stat / numReadingsRpm_Engine2Stat;



// Status for rpmEngine_1 increasing, decreasing or stabil
if(averageRpm_Engine1 > (averageRpm_Engine1Stat + StatusTollerance_Engine_1)&((millisEngine_1Stat_1 + StatusDelay_Engine1) <= currentMillis)&(averageRpm_Engine1 < CompressorActivateLimitEngine_1)){                   // If rpmEngine_1 is increasing status is set to 3, there has to be an increase of rpm before status is set to 3
  rpmEngine_1Status = 3;
  millisEngine_1Stat_3 = millis();
}
else if(averageRpm_Engine1 < (averageRpm_Engine1Stat - StatusTollerance_Engine_1)&((millisEngine_1Stat_3 + StatusDelay_Engine1) <= currentMillis)){               // If rpmEngine_1 is decreasing status is set to 1, there has to be an decrease of rpm before status is set to 1
  rpmEngine_1Status = 1;
  millisEngine_1Stat_1 = millis();
}
else{
  if(((millisEngine_1Stat_1 + StatusDelay_Engine1) <= currentMillis)&((millisEngine_1Stat_3 + StatusDelay_Engine1) <= currentMillis)){     // If rpmEngine_1 is stabile status is set to 2, if there is no increase or decrease more than 25 rpm status is set to 2
  rpmEngine_1Status = 2;
  millisEngine_1Stat_2 = millis();
}
}

// Status for rpmEngine_2 increasing, decreasing or stabil
if(averageRpm_Engine2 > (averageRpm_Engine2Stat + StatusTollerance_Engine_2)&((millisEngine_2Stat_1 + StatusDelay_Engine2) <= currentMillis)&(averageRpm_Engine2 < CompressorActivateLimitEngine_2)){                   // If rpmEngine_2 is increasing status is set to 3, there has to be an increase of 25 rpm before status is set to 3
  rpmEngine_2Status = 3;
  millisEngine_2Stat_3 = millis();
}
else if(averageRpm_Engine2 < (averageRpm_Engine2Stat - StatusTollerance_Engine_2)&((millisEngine_2Stat_3 + StatusDelay_Engine2) <= currentMillis)){               // If rpmEngine_2 is decreasing status is set to 1, there has to be an decrease of 25 rpm before status is set to 1
  rpmEngine_2Status = 1;
  millisEngine_2Stat_1 = millis();
}
else{
  if(((millisEngine_2Stat_1 + StatusDelay_Engine2) <= currentMillis)&((millisEngine_2Stat_3 + StatusDelay_Engine2) <= currentMillis)){     // If rpmEngine_2 is stabile status is set to 2, if there is no increase or decrease more than 25 rpm status is set to 2
  rpmEngine_2Status = 2;
  millisEngine_2Stat_2 = millis();
}
}



// Lets get some results for Engine 1
if(rpmEngine_1Status == 3){
  digitalWrite(ledPinGreenEngine_1, HIGH);      //Used for debugging and for testing status when connected to engine
  digitalWrite(ledPinRedEngine_1, LOW);         //Used for debugging and for testing status when connected to engine
  digitalWrite(relayoutputPinEngine_1, HIGH);   //Turns on relay and activates comressor
}

if(rpmEngine_1Status == 1){
  digitalWrite(ledPinRedEngine_1, HIGH);        //Used for debugging and for testing status when connected to engine
  digitalWrite(ledPinGreenEngine_1, LOW);       //Used for debugging and for testing status when connected to engine
  digitalWrite(relayoutputPinEngine_1, LOW);    //Turns off relay and comressor is deactivated
}

if(rpmEngine_1Status == 2){
  digitalWrite(ledPinGreenEngine_1, HIGH);      //Used for debugging and for testing status when connected to engine
  digitalWrite(ledPinRedEngine_1, HIGH);        //Used for debugging and for testing status when connected to engine
  digitalWrite(relayoutputPinEngine_1, LOW);    // Turn off relay and comressor is deactivated
}

// Lets get some results for Engine 2
if(rpmEngine_2Status == 3){
  digitalWrite(ledPinGreenEngine_2, HIGH);      //Used for debugging and for testing status when connected to engine
  digitalWrite(ledPinRedEngine_2, LOW);         //Used for debugging and for testing status when connected to engine
  digitalWrite(relayoutputPinEngine_2, HIGH);   //Turns on relay and activates comressor
}

if(rpmEngine_2Status == 1){
  digitalWrite(ledPinRedEngine_2, HIGH);        //Used for debugging and for testing status when connected to engine
  digitalWrite(ledPinGreenEngine_2, LOW);       //Used for debugging and for testing status when connected to engine
  digitalWrite(relayoutputPinEngine_2, LOW);    //Turns off relay and comressor is deactivated
}

if(rpmEngine_2Status == 2){
  digitalWrite(ledPinGreenEngine_2, HIGH);      //Used for debugging and for testing status when connected to engine
  digitalWrite(ledPinRedEngine_2, HIGH);        //Used for debugging and for testing status when connected to engine
  digitalWrite(relayoutputPinEngine_2, LOW);    // Turn off relay and comressor is deactivated
}

// Print result to serial:
Serial.print("RPM Engine 1 Average:  ");
Serial.println(averageRpm_Engine1);

Serial.print("rpmEngine_1:  ");
Serial.println(rpmEngine_1);

Serial.print("rpmEngine_1 Status:  ");
Serial.println(rpmEngine_1Status);

Serial.print("RPM Engine 2 Average:  ");
Serial.println(averageRpm_Engine2);

Serial.print("rpmEngine_2:  ");
Serial.println(rpmEngine_2);

Serial.print("rpmEngine_2 Status:  ");
Serial.println(rpmEngine_2Status);




// I used these for debugging:
Serial.print("Total Duration Engine 1:  ");
Serial.println(totalRpmDurationEngine_1);


Serial.print("High Duration Engine 1:  ");
Serial.println(highpulseDurationEngine_1);


Serial.print("Low Duration Engine 1:  ");
Serial.println(lowpulseDurationEngine_1);

Serial.print("Total Duration Engine 2:  ");
Serial.println(totalRpmDurationEngine_2);


Serial.print("High Duration Engine 2:  ");
Serial.println(highpulseDurationEngine_2);


Serial.print("Low Duration Engine 2:  ");
Serial.println(lowpulseDurationEngine_2);


}
