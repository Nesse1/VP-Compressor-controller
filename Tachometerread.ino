/* 
****** Volvo Penta KAD/KAMD 43 tachometer digital read *****
  
  This is an solution for digital reading of engines rpm where there is an pulsed signal delivered to tachometer. 
  For Readme file and similar codes go to https://github.com/Nesse1/VP-Compressor-controller

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

//For smoother rpm values
const int numReadings = 200;    // How many pulses u calculate average from, i fond that 200 still updates fast enough

//Array for reading average of pulses
int readings[numReadings];    
int thisReading = 0;
int readIndex = 0;
int total = 0;
int averageRpm = 0; 

//Values that U might have to change:
int tachoinputPin = 1;          // Specify pin for input signal from generator
int pulsesPerRev = 147;         // 14.7 * 10 (so we include desimal after dot) - Factor for calculating, number of pulses from generator pr engine revolution 
int correctingRev = 20;         // For eventual correction of output rpm, i found that 20 (2.0%) gives me more acurate readings according to tachometer in boat.

// Pulse signal from generator is full og ripples, so by setting max/min pulse allowed code will only read relevant pulses, not ripples.
int lowestPulseAllowed = 1000;  // Lowes pulse allowed - At 3900 rpm pulse is 1047 micros, so shorter pulses are most lightly ripples
int highestPulseAllowed = 5500; // Highest pulse allowed - At 750 rpm pulse is 5442 micros, so longer pulses are most lightly ripples

//For calculating length of pulse we need to know both HIGH and LOW lenght of pulse
unsigned long highpulseDuration;    // to hold the data for how long duration high part of pulse is
unsigned long lowpulseDuration;     // to hold the data for how long duration low part of pulse is
unsigned long totalDuration;        // to hold the data for how long duration high + low part of pulse is
unsigned long rpm;                  // to hold the data for rpm


void setup() {
  // start serial:
Serial.begin(115200);

//For smoother rpm values
for (thisReading = 0; thisReading < numReadings; thisReading++){
  readings[thisReading] = 0;
  }

pinMode(tachoinputPin, INPUT);    //Define pin as INPUT


}

void loop() {


//Get duration of pulses HIG & LOW
highpulseDuration = pulseIn(tachoinputPin, HIGH); //Returns duration of state HIGH in microseconds
lowpulseDuration = pulseIn(tachoinputPin, LOW);   //Returns duration of state LOW in microseconds

//Calculate total 1 puls duration im microseconds
if(((highpulseDuration > (lowestPulseAllowed / 2)) & (lowpulseDuration > (lowestPulseAllowed / 2)))& ((highpulseDuration < (highestPulseAllowed /2)) & (lowpulseDuration < (highestPulseAllowed / 2)))){

  totalDuration = (highpulseDuration + lowpulseDuration) + (((highpulseDuration + lowpulseDuration) * correctingRev) / 1000); // Adding hig & low pulses and correctionRev
}


//Calculate rpm
if(totalDuration < 10){     //If no signal we want rpm to be 0
  rpm = 0;
}
else{
rpm = (((1000000 / totalDuration) *60) / pulsesPerRev) * 10;  //Readjusting the factor where we took 14,7 * 10 to include desimal behind dot
}

//For smoother rpm values:
 // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = rpm;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the averageRpm:
  averageRpm = total / numReadings;


// Print result to serial:
Serial.print("RPM Average:  ");
Serial.println(averageRpm);

Serial.print("Rpm:  ");
Serial.println(rpm);


/*
// Use these for debugging:
Serial.print("Total:  ");
Serial.println(totalDuration);


Serial.print("High:  ");
Serial.println(highpulseDuration);


Serial.print("Low:  ");
Serial.println(lowpulseDuration);
*/


}
