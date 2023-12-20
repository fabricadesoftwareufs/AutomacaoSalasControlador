<p align="center">
  <a href="" rel="noopener">
 <img width=200px height=200px src="res/logo.png" alt="Project logo"></a>
</p>

<h3 align="center">Salas UFS Project IoT</h3>

<div align="center">

[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![GitHub Issues](https://img.shields.io/github/issues/pdavidSilva/controlador-sala.svg)](https://github.com/pdavidSilva/controlador-sala/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/pdavidSilva/controlador-sala.svg)](https://github.com/pdavidSilva/controlador-sala/pulls)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](/LICENSE)

</div>

---

## 📝 Table of Contents

- [About](#about)
- [Getting Started](#getting_started)
- [Deployment](#deployment)
- [Usage](#usage)
- [Built Using](#built_using)
- [TODO](../TODO.md)
- [Contributing](../CONTRIBUTING.md)
- [Authors](#authors)
- [Acknowledgments](#acknowledgement)

## 🧐 About <a name = "about"></a>

The Federal University of Sergipe (UFS) has a high consumption of electricity, this is due to its extensive infrastructure, this fact is something financially and environmentally impacting since the public university must contribute to social good, decreasing financial and environmental impacts and contributing to the community’s socioeconomic development. To improve energy consumption efficiency at UFS and reduce expenses with electricity expenditure, this work proposes an automated system capable of monitoring equipment such as lights and air conditioners in different UFS environments in an automated and even autonomous, avoiding unnecessary and excessive use of such equipment. This solution intends to use microcontrollers, sensors, and actuators in an Internet of Things (IoT) project to achieve its objectives.

## 🏁 Getting Started <a name = "getting_started"></a>

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See [deployment](#deployment) for notes on how to deploy the project on a live system.

### Prerequisites

Which software must you be to install.

```
Arduino IDE
```

Then it must install the libraries

- Adafruit Unified Sensor v1.1.4
- ArduinoJson v6.19.4
- DHT sensor library v1.4.3
- ESP32 BLE Arduino v1.0.1
- IRremoteESP8266 v2.7.11
- NTPClient v3.2.0
- Task by Makuna v1.1.6
- PubSubClient by Nick O'Leary v2.8

Board Manager

- esp32 by Espressif Systems v1.0.6

These files can be found in the **arduino-ide-libs** folder

### Installing

A step by step series of examples that tell you how to get a development env running.

Say what the step will be

- Go to file Config.cpp and change the variables __ssid and the __password to the environment's WIFI.
- Verify if the variable __url is the endpoint correctly from the Salas UFS API.
- Choose the module which you want to compile, it's needed to change the variable __type, there are 3 types: TYPE_CONTROLLER, TYPE_ACTUATOR, and TYPE_SENSOR, the next step it's comment content from the files of (ino) from other modules when one is selected to don't conflict with the module chose to run. 

Finally, it's ready to compile and run.

## 🔧 Running the tests <a name = "tests"></a>

Explain how to run the automated tests for this system.

### Break down into end to end tests

Explain what these tests test and why

```
...
```

### And coding style tests

Explain what these tests test and why

```
...
```

## 🎈 Usage <a name="usage"></a>

Add notes about how to use the system.

## 🚀 Deployment <a name = "deployment"></a>

Add additional notes about how to deploy this on a live system.

## ⛏️ Built Using <a name = "built_using"></a>

- [C++](https://cplusplus.com/) - Programming Language
- [Arduino IDE](https://www.arduino.cc/en) - Platform Development

## ✍️ Authors <a name = "authors"></a>

- [@username](https://github.com/username) - AAAA

See also the list of [contributors]() who participated in this project.

## 🎉 Acknowledgements <a name = "acknowledgement"></a>

- Hat tip to anyone whose code was used
- Inspiration
- References
