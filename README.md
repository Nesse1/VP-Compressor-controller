# Volvo Penta Compressor Controller
For reading tach signal from generator with microcontroller and with this set status on/off for compressor

## Why i started this project:

My Sealine F37 cabincruiser has two Volvo Penta KAMD/KAD 43 engines with turbo intercoolers & compressor. Compressors primary use is for boat to easier reach/pass planing threshold. The compressor activates when rpm is between 1600 & 2400 rpm, and this is not allways a good thing.
When im lowering speed/rpm and are slowing down they also activate from 2400 to 1600 rpm, this is not nessesary. Also if i want to cruise at an lower speed where rpm is between 1600-2400rpm i dont want them to be active.

The reason for that i dont want the compressors to be active in these situations: 

  1 - Unnessesery wear - Repeatly activation/deactivation of comressors means unnessesary wearing of the clutch and drive belt. Also cruising with compressor active means an hi loading pressure af air to engines wich is unnessesary wear of enginge.
  
  2 - Noise - Compressors activates by an electical signal, so there is an commond solutions among boatowners to mount an switch and relay to isolate this signal so they can run engines without comressors active. I´we eaven heard that Volvo recomends this, but what do I know. 
I thougt of mounting an switch & relay, but I´we allways ment that anything worth doing is worth overdoing.... 


## First I have to look at the signal I want to read:

Searching throu manuals for generator and tachometer i found this table in an .pdf installation guide:

![Table of VP engines](https://github.com/Nesse1/images/raw/main/TachometerTable.bmp)

Following the coloumn for Marine Engines i found my engine (31-43) and the conversion factor of 14,7. So then I know that the signal from generators face W should be 14,7 "triggers" per engine revolution.
### Some issues:
At first i assumed that signals Voltage would be about 14 V as this is the voltage usually delivered from generators, but after playing around with the code and an voltage dividor and just getting strange rpm readings, i found that connecting an ocilloscope was nessesary. This reviled that there is an more or less stabile 12 Voltage signal and i had to change resistors in my voltage devidor. Further more this signal has some inteferens.
In my microcontroller code i use the pulseIn function to read how many microseconds pulse is high, then also to read microsecons pulse is low an adding them both together for finding pulse hig-low lenght. Total lenght of pulse is then used to calculate rpm. The inteferens has some ripples that are pulling the pulse to LOW and this is then ofcorse messing with my code calculations.  
I´m using an Wemos D1 mini board for this project cause i suspect that in the future i want an mesh network of these boards in my boat for controlling several stuff. This board has as i found from 2,6 to 3.3 Voltage tolerance for input signal, so voltage dividor has to be pretty accurate. There is an while since i´we used and build filters for signals so my probably rubbish attemdt to passive filter the signal with an condensator wasn´t any good. Result was good signal at low rpm (from 750 to about 1000) but higher rpm also means higher frequence of puls and voltage dropped therfore below 2,6 and no signals got registred by the card. I´m sure there is an good way to "polish" the signal so this issue dissapears, but my way around it was some coding that specify an allowance of lenght of pulse High & Low, and this seems to work pretty good. If U got an drawing of an filter thet would do an good job I´m interested :-).

## Tachometerread.ino

Tachometerread.ino contains just the code for reading tach signal and outputs rpm on serial. 

## ATT: This dokument is not finish....

