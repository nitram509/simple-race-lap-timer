# Simple Race Lap Timer

A simple solution to measure your laps during drone races - for your own record or to compete with friends.

## Standalone module

### Status: Work in Progres

A simple race lap timer, based on
[RX5808](https://de.aliexpress.com/item/DIY-RX5808-5-8G-40CH-Diversity-FPV-Receiver-with-OLED-Display-for-FPV-Racer-Quad/32702074923.html)
diversity receiver.
I uses two video receivers to measure lap times for two individual race copters.
So you can measure your own time and the time of a competing pilot.

The standalone module works independently from any server ... it just requires a power-bank.

### Hardware

* RX5808 diversity module
   * https://de.aliexpress.com/item/DIY-RX5808-5-8G-40CH-Diversity-FPV-Receiver-with-OLED-Display-for-FPV-Racer-Quad/32702074923.html
   * has built-in OLED display, 128x32 pixel
   * can handle 1 or 2 racers in parallel
* 5V Power-Bank (based on 2x18680 Li-ION batteries, e.g. 3400mAh)

## Easy Race Lap Timer - Video Satellite

### Status: Idea phase

The video satellite version aims to integrate into an existing [Easy Race Lap Timer](http://www.easyracelaptimer.com/) infrastructure.
Currently Easy Race Lap Timer (ERLT) just offer the possibility to measure lap times via infrared transponders.
This requires extra hardware on your racer.
With Simple Race Lap Timer Video Satellite (SRLTVS) you can enrich an ERLT installation and add measure
your lap times via your video signal strength - hence, no extra hardware required.

### Hardware

The hardware is sketched like:
* single RX5808 module
* RAK811 Lora Module
   * https://de.aliexpress.com/item/RAK-811-LoRa-Spread-spectrum-3000-meter-wireless-communication-module-UART-serial-port-the-AT-command/32789662365.html
   * offers ultra low power consumption, perfectly for long battery
   * simplifies handling of multiple devices
   * offers security
* Nokia 5110 LCD module
   * https://de.aliexpress.com/item/1pcs-blue-84X48-Nokia-5110-LCD-Module-with-blue-backlight-with-adapter-PCB-LCD5110-for-Arduino/32734842273.html
   * ultra low power
   * eases user interaction ;-)
* 5V Power-Bank (based on 2x18680 Li-ION batteries, e.g. 3400mAh)

## Credits ...

This project was inspired by many open source and open hardware projects.
Thank you all for your great work!

* https://github.com/sheaivey/rx5808-pro-diversity
* https://github.com/voroshkov/Chorus-RF-Laptimer
