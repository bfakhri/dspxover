# dspxover

A project to replace analog crossovers with digial crossovers, allowing for more freedom in designing and testing during loudspeaker design. 

# Standard Analog Crossover Signal Path

Using an analog crossover, the signal is paritioned into bands after amplification

![alt text](https://github.com/bfakhri/dspxover/blob/master/images/xover_analog.png "Analog Crossover Block Diagram")

# Digital Crossover Signal Path

Using a digital crossover, the signal is paritioned into bands BEFORE amplification. More amplification channels are required to accommodate the additional channels (2x for 2-way speakers and 3x for 3-way speakers). 

![alt text](https://github.com/bfakhri/dspxover/blob/master/images/xover_digital.png "Digital Crossover Block Diagram")

## Requirements
libjack-dev: sudo apt-get install libjack-dev

## Practical Implementation 

Loudspeaker:
![alt text](https://github.com/bfakhri/dspxover/blob/master/images/20161206_210647.jpg "Analog Crossover Block Diagram")
Removing drivers:
![alt text](https://github.com/bfakhri/dspxover/blob/master/images/20161206_210720.jpg "Analog Crossover Block Diagram")
Woofer terminals:
![alt text](https://github.com/bfakhri/dspxover/blob/master/images/20161206_202133.jpg "Analog Crossover Block Diagram")
Tweeter extraction:
![alt text](https://github.com/bfakhri/dspxover/blob/master/images/20161206_201337.jpg "Analog Crossover Block Diagram")
Analog crossover to be replaced:
![alt text](https://github.com/bfakhri/dspxover/blob/master/images/20161206_194026.jpg "Analog Crossover Block Diagram")
