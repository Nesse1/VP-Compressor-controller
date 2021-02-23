# Volvo Penta Compressor Controller
For reading tach signal from generator with microcontroller and with this set status on/off for compressor

## Why i started this project:

My Sealine F37 cabincruiser has two Volvo Penta KAMD/KAD 43 engines with turbo intercoolers & compressor. Compressors primary use is for boat to easier reach/pass planing threshold. The compressor activates when rpm is between 1600 & 2400 rpm, and this is not allways a good thing.
When im lowering speed/rpm and are slowing down they also activate from 2400 to 1600 rpm, this is not nessesary. Also if i want to cruise at an lower speed where rpm is between 1600-2400rpm i dont want them to be active.

The reason for that i dont want the compressors to be active in these situations: 

  1 - Unnessesery wear - Repeatly activation/deactivation of comressors means unnessesary wearing of the clutch and drive belt. Also cruising with compressor active means an hi loading pressure af air to engines wich is unnessesary wear of enginge.
  
  2 - Noise - Compressors activates by an electical signal, so there is an commond solutions among boatowners to mount an switch and relay to isolate this signal so they can run engines without comressors active. I´we eaven heard that Volvo recomends this, but what do I know. 
I thougt of mounting an switch & relay, but I´we allways ment that anything worth doing is worth overdoing.... 


## About the signal I want to read:

Searching throu manuals for generator and tachometer i found this table:

![Table of VP engines](https://github.com/Nesse1/images/raw/main/VolvoTachDiagram.png)
