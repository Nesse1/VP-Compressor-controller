# Volvo Penta Compressor Controller
For reading tach signal from generator with microcontroller and with this set status on/off for compressor

## Why I started this project:

My Sealine F37 cabin cruiser has two Volvo Penta KAMD/KAD 43 engines with turbo intercoolers & compressor. Compressors primary use is for boat to easier reach/pass planning threshold. The compressor activates when rpm is between 1600 & 2400 rpm, and this is not always a good thing.
When I am lowering speed/rpm and are slowing down they also activate from 2400 to 1600 rpm, this is not necessary. Also, if I want to cruise at a lower speed where rpm is between 1600-2400rpm I don’t want them to be active.

The reason for that I do not want the compressors to be active in these situations: 

  1 - Unnecessary wear - Repeating activation/deactivation of compressors means unnecessary wearing of the clutch and drive belt. Also cruising with compressor active means an hi loading pressure of air to engines which is unnecessary wear of engine.
  
  2 - Noise - Compressors activates by an electrical signal, so there is a common solutions among boatowners to mount an switch and relay to isolate this signal so they can run engines without compressors active. I ‘we have heard that Volvo recommends this, but what do I know. 
I thought of mounting an switch & relay, but I ‘we always meant that anything worth doing is worth overdoing.... 


## First a look at the signal I want to read:

Searching through manuals for generator and tachometer i found this table in an .pdf installation guide:


![Table of VP engines](https://github.com/Nesse1/images/raw/main/TachometerTable.bmp)

Following the column for Marine Engines I found my engine (31-43) and the conversion factor of 14,7. So then I found that the signal from generators face W should be 14,7 "triggers" per engine revolution.

## Calculations: 
I´m building the code using the command "pulseIn" which returns how many microseconds pulse is defined HIGH or LOW. Then we must do some calculations in code to get rpm.

How does the range of pulses/min from my engine looks:

  Number of pulses/min at idle or 750/rpm:             750 * 14,7 = 11025 pulses/min at 750 rpm  
  Number of pulses/min at mid throttle or 2500/rpm    2500 * 14,7 = 36750 pulses/min at 2500 rpm  
  Number of pulses/min at full throttle or 3900/rpm:  3900 * 14,7 = 57330 pulses/min at 3900 rpm  

How many pulses/second pr engine shaft revolution:

  Number of pulses/second at idle or 750/rpm:           11025 / 60 = 183,75 Hz or pulses/sec  
  Number of pulses/second at mid throttle or 2500/rpm   36750 / 60 = 612,5 Hz or pulses/sec  
  Number of pulses/second at full throttle or 3900/rpm: 57330 / 60 = 955,5 Hz or pulses/sec  
  
How many microseconds from start to end of pulse (HIGH & LOW):
  
   Length pulse at idle or 750/rpm:                       1000000 / 183,75 = 5442 micros  
  Length pulse at mid throttle or 2500/rpm:              1000000 / 612,5  = 1633 micros  
  Length pulse at full throttle or 3900/rpm:             1000000 / 955,5  = 1047 micros  
  

### Some issues:
At first I assumed that signals Voltage would be about 14 V as this is the voltage usually delivered from generators, but after playing around with the code and an voltage divider and just getting strange rpm readings, I found that connecting an oscilloscope was necessary. This reviled that there is a stabile 12 Voltage signal and I had to change resistors in my voltage divider. Furthermore, this signal has some interference.
In my microcontroller code I use the pulseIn function to read how many microseconds pulse is high, then also to read microsecond pulse is low an adding them both together for finding pulse high-low length. Total length of pulse is then used to calculate rpm. The interference has some ripples that are pulling the pulse to LOW and this is then of course messing with my code calculations.  
I don´t have the actual readings, but to give U an idea they look something like this:  
![Example of pulse noice](https://github.com/Nesse1/images/raw/main/Examplenoice.png)

Example shows a good pulse at 750 rpm to the left and a bad pulse to the right. With calculations in code result will show 750 rpm for the first pulse, but the second pulse with one ripple in it will deliver an LOW reading mid pulse. "Bad" pulses have of course often several ripples, this is just an example. 


I´m using an Wemos D1 mini board for this project because I suspect that in the future, I want a mesh network of these boards in my boat for controlling several stuffs. This board has as i found from 2,6 to 3.3 Voltage tolerance for input signal, so voltage divider must be pretty accurate. There is a while since I ‘we used and build filters for signals so my probably rubbish attempt to passive filter the signal with a small capacitor wasn´t any good. Result was good signal at low rpm (from 750 to about 1000) but higher rpm also means higher frequency of pulse and voltage dropped therefore below 2,6 and no signals got registered by the card. I´m sure there is a good way to "polish" the signal, so this issue disappears, but my way around it was some coding that specify an allowance of length of pulse High & Low, and this seems to work well. If U got a drawing of a filter that would do an good job I´m interested :-).
The solution for this to work without filtering of frequency was to alter the code so it´s only uses pulses that are within normal range of engine.
The pulse reading in code happens using microseconds, ore 1/1000000 second if U will. This happens fast, so the process must be slowed down before an output. I´m using an Array to get a pulse length average of rpm readings, and further using an Array again to get an rpm average for Status updates. This slows the process down and helps to get a smoother and more correct output.

## Tachometerread.ino

Tachometerread.ino contains just the code for reading tach signal and outputs rpm on serial, I added this as an tach read only with thoughts that this could be the interesting part for some users...


### About Code, I´m focusing on the parts U might want to change:


Number of readings for pulse average for smoother rpm reading:
```

const int numReadings = 200;    // How many pulses u calculate average from, I found that 200 still updates fast enough

```

How many pulses should you calculate average from. Higher number gives smooth readings/slower updates of rpm and lower number gives a more "jumping" reading but faster updates. One pulse duration is at most (for my engine) 5442 micros, which means that slowest pulse rate is 183,75 pulses in second (if all pulses are good and without ripples) so 200 pulses isn´t as much as it seems.


Input Pin:
```

int tachoinputPin = 5;          // Specify pin for input signal from generator

```

Specify pin for tachometer signal, this is an digital pin.



Values U must change according to your engine:
```

int pulsesPerRev = 147;         // 14.7 * 10 (so we include desimal after dot) - Factor for calculating, number of pulses from generator pr engine revolution 
int correctingRev = 20;         // For eventual correction of output rpm, i found that 20 (2.0%) gives me more acurate readings according to tachometer in boat.

```
PulsesPerRev is where U specify how many pulses Your generator is delivering per revolution. As U can see U have to multiply pulses per revolution by 10, this was necessary for code to also consider decimals behind dot.

CorrectingRev is an function added for ability to adjust rpm readings if they don´t mach tachomerter. 20 = 2 % higer value output.


Allowance of pulse:

```

int lowestPulseAllowed = 1000;  // Lowes pulse allowed - At 3900 rpm pulse is 1047 micros, so shorter pulses are most lightly ripples
int highestPulseAllowed = 5500; // Highest pulse allowed - At 750 rpm pulse is 5442 micros, so longer pulses are most lightly ripples

```
I discovered that counting pulses in microseconds like I do in this code will also count all kind of noise. Some of this noise brings the pulse to low several times while pulse is supposed to be high. Therefore, by adding an allowance limit for pulses to be read I manages to filter out most of the bad pulses. Also, since the normal length of pulses is so short that I measure them in microseconds i find that there is plenty of "good" pulses to get the reading fast enough.   
U will have to do some calculations like mine to find your needs for allowance. Notice that I´m setting allowance a bit higher/lower than actual calculations.

### ATT: This document is not finish....
