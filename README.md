# esp32-env-monitor

A project to record temperature and humidity with using esp32 and a sensor.

# Requirements

- [PlatformIO](https://platformio.org/) (and a PC to use it)
- A Channel of [ThingSpeak](https://thingspeak.com)
- Components (see below)

# Usage

Create your `src/env_params.h`. See [src/env_params.h.example](./src/env_params.h.example) to required params.

Then you can upload program to WROOM.
```
pio run -t upload
```

# Components

- ESP32-WROOM-32 develop board
- AM2320 (temperature and humidity sensor)
- 5-10K register * 2
- A breadboard and jumper wires

# Connection

## WROOM and AM2320

WROOM | AM2320
----- | ------
3V3   | VIN
21    | SDA
22    | SCL
GND   | GND

## WROOM and registers

Set registers like this to pull up I2C wire.

- 21 - Register - 3V3
- 22 - Register - 3V3

# References
- [ESP32 Deep Sleep のテスト （Hibernation mode)](http://d.hatena.ne.jp/wakwak_koba/20170219)
- [ESP32/examples/DeepSleep/TimerWakeUp/TimerWakeUp.ino](https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/DeepSleep/TimerWakeUp/TimerWakeUp.ino)
