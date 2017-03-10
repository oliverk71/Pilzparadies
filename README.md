# Pilzparadies (fungi's paradise)
A fruiting chamber for cultivating fungi in a city apartment. It measures and controls humidity, temperature and carbon dioxide level. It also switches on and off light based on the clocktime.

It's mainly intended for experiments on fungi. As long you can maintain a high humidity, correct temperature and lighting conditions as well as fresh air, you should be able to use a low-tech approach, too. The main reason I started this project was that I wanted to  know the exact CO2 levels as well as temperature and humidity and log it together with a timestamp onto SD card. 

If you don't want or don't need to measure and control carbon dioxide, you may use something like the Inkbird IHC-230 to control  humidity and temperature the easy way for a reasonable price.

Most cultivators will have to grow mycelium by themselves before inducing fruiting. You can easily build a growing chamber from a big styrofoam-box, a thermostat switch 12 VDC, a heater foil ~ 20-30 W 12 VDC and a fan 12 VDC.

But now let's get started with the fruiting chamber: 

We use a 4-channel relay card to control 12 VDC devices and a RF transmitter to control 230 VAC devices via wireless sockets. Lower and upper thresholds for temperature, humidity, carbon dioxide and light time on/off are stored in the file settings.txt. These settings will be read from SD-card when the Arduino starts up. Then it will begin to read sensor values, log them onto SD-card and switch on or off heater, humidifier, fans etc. basing on the measurements.

Hardware used:

1 Arduino Pro Micro (Leonardo)
1 MH-Z19 (CO2 sensor)
1 DHT22 (temp/humidity sensor)
1 SD card reader/writer (for logging data)
1 RTC module
1 LCD 16x2 I2C
1 relay card, 4-channel (I connected 12 VDC to it, so I can use 12 VDC fans)
1 STX822 (RF transmitter to control wireless sockets)
1 USB car charger electronic (12 VDC -> 5 VDC)
1 power supply 12 VDC
1 housing
1 or more wireless sockets (to control 230 VAC humidifiers or heaters etc.)
1 socket (to connect 12 VDC power supply)
1 indoor greenhouse 
some wires

to be continued
