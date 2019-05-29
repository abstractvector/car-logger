# Car Logger

The OBD-II port and CAN-BUS in modern vehicles reveal a plethora of information in real-time. This project aims to access as much of that information as possible and publish it to MQTT for future processing.

## Background
We live full-time in an RV, towed behind our Ford F-150,. traveling around the US. As self-confessed data geeks, we are trying to capture as much data as possible. The truck is a veritable goldmine of rich data, but it's hidden away in the vehicle's onboard electronics.

So, using a Freematics One+ device, this project frees that data. Since our trailer has WiFi and a server running an MQTT broker, the goal is to get the data out of the truck and into the trailer.

## Hardware

This project was built to run on a Freematics One+ device. It is an ESP32-based device which plugs into the OBD-II port in the vehicle. As well as accessing OBD-II and CAN-BUS data, it also has a GPS sensor, accelerometer and more. It also has a microSD card slot, WiFi and Bluetooth.

## Architecture

Ideally, the data would be read from the device's various sensors and published directly to the MQTT broker. However, in the event that the WiFi connection or MQTT broker are unavailable, the data should be cached on the microSD card until the MQTT broker reappears.

## Usage

1. Download the libraries from [Freematics](https://github.com/stanleyhuangyc/Freematics)
1. Copy `config.h` and replace with your own settings
1. Build and upload to your Freematics One+
1. Plug into vehicle, cross your fingers and hope

# TODO
Look, there's still loads to do. Also, this is my first major Arduino project, so I'm learning lots!

- [ ] Poll and log all available OBD-II PIDs
- [ ] Sniff CAN-BUS and process the data stream
- [ ] Cache data on microSD card if MQTT is unavailable
- [ ] Update firmware via WiFi (OTA updates)
- [ ] Move configuration to user-space (initial WiFi AP)
- [ ] Improve logging

Plus I'm sure there are loads of bugs, the code isn't documented and I've written no tests. Enjoy!
