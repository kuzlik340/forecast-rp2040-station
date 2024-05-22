<img src="https://github.com/kuzlik340/forecast-rp2040-station/assets/145336491/a7c2bccd-131b-4acd-8dc7-73c232ca1948" alt="DALLE2024-05-2201 57 33" width="300"/>

# forecast-rp2040-station

## Introduction

The forecast-rp2040-station is a project based on the Raspberry Pi RP2040 microcontroller. It aims to provide a compact and efficient meteorological station capable of monitoring and displaying temperature and atmospheric pressure. This station displays real-time data on graphics and provides users with a forecast for 10 hours ahead by simply comparing pressures.


## Functionality

The forecast-rp2040-station offers the following features:

- **Temperature Monitoring**: Continuously tracks the ambient temperature. Also every 17 minutes there will be an update of the graphic on temperature.
- **Pressure Measurement**: Measures atmospheric pressure. Also doing a graphic out of it.
- **Forecast feature**: The main feature of this station is forecast based on the atmospheric pressure.

## Wiring

<img src="https://github.com/kuzlik340/forecast-rp2040-station/assets/145336491/ede9e482-9626-4ea4-907a-657552a753c1" alt="2024-05-19 02 01 09" width="450"/>   

## How to rune code
1. Download the code from this repository and open it in arduino IDE
2. Download all adafruit libraries that have to be in this project
3. Correct the address of display to 0x3C in Adafruit_SSD1306.h
4. Correct the address of pressure sensor to 0x76 in Adafruit_BMP280.h
5. Check all wiring!
6. Run the code on your microcontroller :)

## Screens
So the first two screens will show an estimated time for data to log. The dots displayed on graphics are updated every 15 minutes for temperature and every 30 minutes for pressure. So, to see the first dot we have to wait some time.

<img src="https://github.com/kuzlik340/forecast-rp2040-station/assets/145336491/35628b00-f1be-4b99-8560-c2ffd42fa540" width="300"/>

Than when all the data will be uploaded we will see a screen with a pressure graphic and a temperature graphic.

<img src="https://github.com/kuzlik340/forecast-rp2040-station/assets/145336491/d940977c-94a4-410f-bf68-987eaf4e7262" width="300"/>
<img src="https://github.com/kuzlik340/forecast-rp2040-station/assets/145336491/0082005e-2df5-4da8-a467-f96ff906f0e7" width="300"/>

And when there will be at least 11 logs of pressure(estimated time is 7 hours) we will see the forecast. Then it will be displayed permanently. We will wait for the forecast only when starting microcontroller.

<img src="https://github.com/kuzlik340/forecast-rp2040-station/assets/145336491/9919d32f-170e-4e7f-a440-f188ddf170b3" width="300"/>

## A gif of information on display

<img src="https://github.com/kuzlik340/forecast-rp2040-station/assets/145336491/2c79f3e3-4539-4ef5-9bf4-03c7af5de424" width="300"/>





