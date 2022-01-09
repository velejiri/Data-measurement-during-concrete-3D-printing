# Data-measurement-during-concrete-3D-printing
## Basic info
This work aims to set a basic framework for data collection during concrete 3D printing. Collected data could be used for static simulation, improvement of printing process and to develop system that can control water amount mixed with mortar and accelerant amount. This work was supported by the Grant Agency of the Czech Technical University in Prague, grant No. SGS 161 - 1612103E000.
## Technology used
For measuring purposess I used Arduino and Raspberry Pi hardware. Flowmeter YF-S201 connected to NodeMCU sends data about amount of mixed water, DHT 11 measures ambient temperatre and humidity, ESP-32 camera checks extrusion. All sensors are connected to wifi and send data to the Raspberry Pi. Node-red flow on Raspberry Pi stores data on SD card and you can also access live measurings from your phone. In the second stage I would like to construct reactive system, where amont of mixed water will be controled according to ambient environment to achieve constant extrusion quality.
## Printing setup
For measuring experiments I used printing setup tested on printing first Czech 3D printed house - Prvok. This setup consist of ABB IRB 6700 industrial robot, M-tec P50 pump, M-tec D30 mixer and hose 20 m long with inner dimater 35 mm. Material used is Master Flow 3D 100, it a plastic mortar with accelerant and pp fibres.

## Variables influencing prinitng quality
Environment - Low temperature slows process of setting of concrete. Eventually it may cause colaps of printed structure. High temperature may cause concrete to set somewhere in the system (mixer, pump, hose, nozzle). Ideal temperature is around 20 degrees of celsius. Low humidity also slows process of setting of concrete. It should be below 70%

Water - Mixing concrete with warm water may speed up setting of concrete. This may help while printing in low temperature environment.
Amountof mixed water: Should react to environment temperature. In warmer places it is safer to mix more water.

Material - Material should have a room temperature. If stored outside it can freeze and next day mortar doesn´t set properly. Also it is better if accelerant is mixed directly in the nozzle. This solution is more complicated than have it already mixed in dry mixture, but offers ideal dosage according to other conditions.

Speed of Mixer - Mixer mixes material too fast, so there have to be set on/off periods. Also sometimes water input gets stuck and less or more water is mixed.

Speed of Pump - Speed can be set by potentiometer, or by hardware. There are several types of spindle and we use the slowest one. Lot of material in the pump may cause stuck.

Speed of the Robot - Each layer should have an ideal time window before another layer is put on top of it in order to hold its designated shape. Basically the bigger is objects layout the faster can robot move.

Reinforcement of the printed objects - Rebars can be added manually during the printing. Small fibres may be mixed with dry concrete.




### Consistency of material in the system
Preassure measurement can indicate, if material goes threw system without troubles, or getting either too solid or liquid. If preassure drops, too liquid material is being pushed and when it extrudes, layers will not hold its shape and setting time will be prolonged. This can lead to collaps of printed structure. If material is too dry, preassure rises, because pump has to push material harder. After extrusion you can see cracks and there is a chance that the hose will get blocked by stiffened concrete. Preassure measurement might be done by preassure sensors. One should be at the connection of the pump and the hose.




## Helpful links
Simple 3 Phase Arduino Energy Meter
https://www.instructables.com/Simple-3-Phase-Arduino-Energy-Meter/

How To Control a Stepper Motor with A4988 Driver and Arduino
https://howtomechatronics.com/tutorials/arduino/how-to-control-stepper-motor-with-a4988-driver-and-arduino/

WIDTH AND GAP MEASUREMENT
https://www.acuitylaser.com/laser-sensors/case-studies/accuprofile-820-laser-scanner/width-and-gap-measurement/

Meteorino: Extend Battery Life on NodeMCU? Just Switch It Off.
https://www.instructables.com/Meteorino-Extend-Battery-Life-on-NodeMCU-Just-Swit/

$9 RTSP Video Streamer Using the ESP32-CAM Board
https://www.instructables.com/9-RTSP-Video-Streamer-Using-the-ESP32-CAM-Board/

Raspberry Pi 4 Getting Started
https://www.youtube.com/watch?v=BpJCAafw2qE

ESP8266 and Node-RED with MQTT (Publish and Subscribe)
https://randomnerdtutorials.com/esp8266-and-node-red-with-mqtt/

Auto Logger - Store Node-Red data in daily CSV files
https://www.youtube.com/watch?v=E2aBIqssQLM
