# Hybrid Ultrasonic–Doppler Radar System

ECE 414 – Embedded Systems Final Project  
Platform: Raspberry Pi Pico (RP2040)
Efe Civisoken , Jack Ewing
---

## What This Project Does

This project runs a hybrid sensing system on a Raspberry Pi Pico that:
- Measures distance using ultrasonic time-of-flight
- Detects motion using a microwave Doppler sensor
- Sweeps sensors using servo motors
- Displays a radar-style visualization on a TFT screen
- Streams telemetry over USB serial

---

## What You Need

## Hardware
- Raspberry Pi Pico (RP2040)
- 2× HC-SR04 ultrasonic sensors
- 2× HB100 Doppler radar modules
- 2× hobby servo motors
- SPI TFT display (ILI934x family)
- External 5 V USB power
- Breadboards and jumper wires

## Hardware Setup

All wiring, power connections, and signal routing are documented in the **electrical schematics provided in the `Radar_PCB/` folder**.  
These schematics were created using **KiCad** and serve as the authoritative reference for hardware setup.

To assemble the system:
- Follow the KiCad schematic files in `Radar_PCB/` for all GPIO, power, and ground connections
- Ensure the Doppler radar modules and analog conditioning circuitry are physically separated from high-current digital components (servos, display, Pico) to minimize noise
- Use a stable 5 V supply capable of sourcing sufficient current for the servos

No additional hardware configuration is required beyond what is shown in the schematics.

---

### Software
- Raspberry Pi Pico SDK
- CMake (v3.13+)
- GCC ARM Embedded Toolchain
- Make

Make sure the Pico SDK is cloned locally.

---

## Build and Flash Instructions

From the project directory, run:

```bash
rm -rf build
mkdir build
cd build
cmake .. -DPICO_SDK_PATH=../pico-sdk -DPICO_BOARD=pico2
make -j$(nproc)

```

## This creates a .uf2 firmware file in the build directory.

Flash to the Pico
Hold the BOOTSEL button on the Pico

Plug the Pico into your computer via USB

Drag and drop the .uf2 file onto the Pico drive

## Running the System
After flashing:

Servos begin sweeping automatically

Ultrasonic distance measurements run at each angle

Doppler motion detection runs continuously

A radar-style display appears on the TFT

Telemetry is sent over USB serial

View Serial Output
Use any serial terminal:

Baud rate: 115200

Output includes angle, distance, motion flag, and RMS values

## Notes
Build artifacts are ignored by .gitignore

Display and touchscreen drivers are reused from earlier ECE 414 labs

Doppler analog circuitry should be physically separated from digital components for best results

## Demo
A working demo video is available here:

<https://drive.google.com/file/d/15QqWR9_1v-_UHlbc8YgBszJ1aU8pvBwp/view?usp=sharing>





