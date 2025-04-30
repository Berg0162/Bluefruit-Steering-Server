# <img src="/media/Nordic_Icon.png" width="53" height="50" align="bottom" alt="Nordic Icon"> Open Virtual Steering with BluefruitSteeringServer
The **Romans** knew it already **`"Sterzare Necesse Est"`** (free after Plutarch) and it is still valid in a **virtual cycling world**.<br>
<details><summary>What is means</summary>
    
The original quote **Navigare Necesse Est** ("Navigation is Necessary"), was humourously adapted here, by replacing **Navigare** with the Italian verb for [**steering**](https://en.wiktionary.org/wiki/sterzare), aligning it with the project's theme.

</details>

**Open Virtual Steering (OVS)** is an open-source project that explores interfacing with various **Human-Interface-Devices (HID)** for the **use case of steering control** in **virtual cycling platforms**.
It provides examples of alternative input methods for **navigating and positioning a cycling avatar** in a virtual world. 

## 📦 What Is This?

**BluefruitSteeringServer** is an Arduino-compatible C++ library that provides a ready-to-use **BLE server implementation** for **steering control in virtual cycling platforms**, built specifically for the **Adafruit Bluefruit (Nordic's nRF52) ecosystem**.

It is the **nRF52/Bluefruit sibling** of the [BLESteeringServer](https://github.com/Berg0162/BLE-Steering-Server) library, and is designed to serve as the BLE communication layer for applications using **alternative Human Interface Devices (HIDs)** — such as buttons, joysticks, motion sensors, or voice input.

The library handles BLE advertisement, pairing, and the transmission of **steering angle data** using a known **steering profile**, enabling smooth integration with virtual cycling clients that support BLE steering controllers.

It is used as a backend in projects focused on HID-based steering control.

![Open Virtual Steering Diagram](/media/OpenVirtualSteering_Overview.png)

## 📚 Related OVS Repositories

This project allows users to explore a range of input devices for **steering control in virtual cycling worlds**. The emphasis is on **enhancing interactivity and user experience** through both **accessible** and **innovative** control methods.

| HID Focus | Bluefruit Repository | NimBLE Repository |
|-----------|-------------------|----------------------|
| Buttons, Joysticks, Rotary Encoders | [`OVS-DiscreteHIDs-Bluefruit`](https://github.com/Berg0162/Open-Virtual-Steering-DiscreteHID-Bluefruit) | [`OVS-DiscreteHIDs`](https://github.com/Berg0162/Open-Virtual-Steering-DiscreteHID) | 
| Turn & lean-based steering with MPU6050 | [`OVS-MotionIMU-Bluefruit`](https://github.com/Berg0162/Open-Virtual-Steering-MotionIMU-Bluefruit) | [`OVS-MotionIMU`](https://github.com/Berg0162/Open-Virtual-Steering-MotionIMU) |
| Voice-activated steering with TinyML | ℹ️ | [`OVS-VoiceControl`](https://github.com/Berg0162/Open-Virtual-Steering-VoiceControl) | 
> ℹ️ The **VoiceControl** project is tightly coupled to the **XIAO ESP32S3 Sense**, which includes a built-in microphone and supports only **NimBLE (ESP32)**. A Bluefruit version is not applicable.

## ❓ What is Bluefruit?

**Bluefruit** is Adafruit's branding for its line of development boards and firmware built around **Nordic Semiconductor's nRF52 family** of Bluetooth Low Energy (BLE) chips.

This repository provides code that specifically targets **Adafruit's Bluefruit stack**, using their `Bluefruit.begin()`, `BLEService`, and `BLECharacteristic` APIs — separate from ESP32 or NimBLE-based implementations.

## 🔧 How the code works

With the **BluefruitSteeringServer** library installed, developers can easily create applications that act as **dedicated BLE steering controllers** for use with **virtual cycling platforms**.

The user application is responsible for interfacing with a **Human Interface Device (HID)** — such as buttons, a joystick, an IMU, or voice input — and regularly passing steering data to the BluefruitSteeringServer.

Once paired with a BLE client, the application uses `BluefruitSteeringServer::updateSteeringValue()` to transmit real-time **steering angle data** over Bluetooth.

To ensure **interoperability**, the library implements a known **BLE Steering Profile**, which defines a consistent structure of services and characteristics used in commercial BLE steering devices. BLE profiles serve as an **application-layer contract** between the controller (peripheral) and the receiving client (central), ensuring compatibility across devices and platforms.

<details>
<summary>🔍 More Info</summary>
    
[Introduction on BLE profiles, services, characteristics, device roles and network topology](https://embeddedcentric.com/lesson-2-ble-profiles-services-characteristics-device-roles-and-network-topology/)

</details>

## 🧱 Dependencies

+ [Adafruit nRF52 Arduino Core](https://github.com/adafruit/Adafruit_nRF52_Arduino)
+ [Adafruit Bluefruit Library (Source)](https://github.com/adafruit/Adafruit_nRF52_Arduino/tree/master/libraries/Bluefruit52Lib)

> 💡 This library is built specifically for **Adafruit's Bluefruit platform**, and is not compatible with NimBLE-based ESP32 boards.

### **Supported MCU Boards**
+ Nordic Semiconductor **nRF52 series**
  - 🟢 Adafruit Feather nRF52840 Express
  - 🟢 Adafruit ItsyBitsy nRF52840
  - 🟢 Adafruit CLUE, Bluefruit Sense, and other nRF52 boards with BLE support

To install the core:
- Follow Adafruit's guide: [Install Adafruit nRF52 Arduino Core](https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/arduino-bsp-setup)

## 🚴‍♂️ Quick start?

+ Install the **BluefruitSteeringServer library** from this repository. Download as `.zip` and extract to `Arduino/libraries` folder, or <br>in <b>Arduino IDE</b> from `Sketch menu` -> `Include library` -> `Add .Zip library`<br>
+ Select one of the _**Related Repositories**_ that apply this **shared backend** and offer you **ready-to-use exemplary access** to a selection of different **Human-Interface-Devices**.

## ⚠️ Disclaimer
<details>
<summary> 💡 <b>Research & Independence</b></summary>
This project is <b>not affiliated with, endorsed by, or associated with any commercial virtual cycling platform or steering device manufacturer</b>. It is a <b>research and interoperability</b> initiative designed to explore <b>alternative human interface methods</b> in the context of indoor cycling. All development is conducted independently for <b>educational and experimental purposes</b>.
</details>
<details>
<summary> ✅ <b>Compliance & Responsibility</b></summary> 
This repository does <b>not include or promote any circumvention of technological protection measures</b>, reverse engineering of proprietary software, or unauthorized access to restricted systems. Users are <b>solely responsible</b> for ensuring that their use of this code complies with <b>local laws, software licenses, and platform terms of service</b>.
</details>
<details>
<summary> 🔍 <b>Copyright & Contact</b></summary>
If you are a <b>rights holder</b> and believe that this project includes content that <b>violates your intellectual property rights</b>, please <b>open an issue</b> on this repository to initiate a respectful review. We are committed to responding promptly and, if appropriate, taking corrective action.
</details>

## 🧪 Testing & Validation
The functionality of **Open Virtual Steering with BluefruitSteeringServer** was tested using **standard BLE debugging tools** to ensure proper communication and responsiveness. One of the primary tools used was **nRF Connect for Mobile**, a widely available application for **scanning, connecting, and interacting with BLE devices**. This allowed for **verification of characteristic read/write operations, response timing, and general stability** of the BLE communication.  

## 🔧 Basic Usage

```cpp
#include <BluefruitSteeringServer.h>

void setup() {
    BluefruitSteeringServer::getInstance().begin();
}

void loop() {
    float angle = getSteeringAngleFromHID();
    BluefruitSteeringServer::getInstance().updateSteeringValue(angle);
}
```
---

<details>
<summary>⚖️ <b>Legal Notice (EU Context)</b></summary>

This project is developed and published in accordance with **EU directives** that recognize the right to study, test, and develop software components for the purpose of achieving **interoperability** (e.g., Directive 2009/24/EC on the legal protection of computer programs, Article 6).  

No part of this project is intended to **infringe upon intellectual property rights** or violate technological protection measures. All content is shared in good faith under the belief that it falls within the bounds of **legitimate research, reverse engineering for interoperability, and fair use under EU law**.  

Users must ensure their own compliance with **national implementations of EU directives**, and are responsible for how they apply or modify this code.

</details>

---
