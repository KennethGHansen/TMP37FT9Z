# TMP37 Temperature Sensor Driver for STM32F446RE 
With ADC, EMA & Kalman Filtering

---

# 📌 Overview
This repository provides a robust, well‑documented driver for the TMP37 analog temperature sensor running on an STM32F446RE (Nucleo‑64) board.
The project demonstrates:

Correct STM32 ADC configuration
TMP37 voltage‑to‑temperature conversion
Noise reduction using:

Exponential Moving Average (EMA)
1‑D Kalman Filter (for high stability)

Clean separation between hardware initialization and sensor logic

The design is suitable for bare‑metal or FreeRTOS‑based projects.

---

# 🧠 Why TMP37?
The TMP37 is a precision analog temperature sensor with:

20 mV / °C output slope
0 V at 0 °C (no offset)
Low noise and low power consumption

Transfer function:
- Temperature (°C) = Vout / 0.02

---

## Project Structure
```
TMP37FT9Z/
├── Core/
│   ├── Inc/
│   └── Src/
│
├── Drivers/
│   └── Custom/
│       ├── TMP37.c
│       └── TMP37.h
│
├── MDK-ARM/
│   ├── TMP37FT9Z.uvprojx
│   ├── TMP37FT9Z.uvoptx
│
├── README.md
├── LICENSE
├── .gitignore
└──TMP37FT9Z.ioc
```

---

Keil users: be sure to add  
`Drivers/Custom/`  
to the **include paths in uvision**

##  Getting Started

### 1. Clone the repository

### 2. Open the Keil `.uvprojx` project file

### 3. Make sure the include paths are correct
Keil →  
**Project → Options for Target → C/C++ → Include Paths**
Add:
../Drivers/Custom

### 4. Build and flash
Press **Build** and **Download** in Keil.

---

## Hardware

| TMP37 pin | STM32F446RE |
|---------|------------|
| V+ | 3.3V |
| Vout | PA1 (ADC1_IN1) |
| GND | GND |

---

# 🔌 ADC Configuration Summary
- ADC instance: ADC1
- Channel: Channel 1 (PA1)
- Resolution: 12‑bit (0–4095)
- ADC clock: PCLK2 / 4
- Sampling time: 56 or 144 ADC cycles (recommended for stability)

---

# 🔄 Data Processing Pipeline
1) Raw ADC value is sampled
2) Converted to voltage using Vref
3) Converted to temperature using TMP37 formula
4) Filtered using:
- EMA (lightweight smoothing) or
- Kalman Filter (maximum stability)

NOTE: For very steady temperature measurement a slower filter routine is recommended!

---

# 📉 Filtering Options
# EMA Filter
- Simple
- Low CPU cost
- Adjustable smoothing factor (α)

# Kalman Filter (Recommended)
- Excellent noise rejection
- No lag
- Adapts dynamically to noise and temperature drift

Typical Kalman tuning for stable temperature:
- Q = 0.002 – 0.01
- R = 4 – 10

---

# 🧪 Validation Tests
| Test | Expected results|
|---------|------------|
| PA1 -> GND | ADC ~ 0 |
| PA1 -> 3V3 | ADC ~ 4095 |
| Room temperature | ~20 - 25 °C) |
| Touching sensor | Temperature rises smoothly |

---

## Flow Chart
<img width="1000" height="600" alt="default_image_001" src="https://github.com/user-attachments/assets/b28ba954-2f46-4b2e-a348-a8212b65c6f0" />

---

#🔍 Detailed Flowchart Explanation

1️⃣ Start / MCU Initialization
The system boots, initializes HAL, system clock, and prepares the MCU.

2️⃣ GPIO Initialization (Analog Mode)
The ADC pin (PA1) is configured in analog mode, disabling digital drivers to reduce noise and power consumption.

3️⃣ ADC Initialization
ADC1 is configured with:
- Correct clock prescaler
- Resolution
- Sampling time
- Selected channel
This ensures accurate voltage acquisition.

4️⃣ TMP37 + Kalman Initialization
The TMP37 driver initializes its internal state:
- Kalman estimate
- Error covariance
- First measurement alignment

5️⃣ Start ADC Conversion
Each measurement cycle explicitly starts an ADC conversion via software trigger.

6️⃣ Read Raw ADC Value
The ADC returns a 12‑bit value (0–4095) representing the sampled voltage.

7️⃣ Convert Raw ADC to Voltage
Using:
- Vout = (raw / 4095) × Vref


8️⃣ Convert Voltage to Temperature
TMP37 conversion:
- Temperature = Vout / 0.02


9️⃣ Kalman Filter Processing
The measured temperature is passed through the Kalman filter:
- Predict step estimates natural temperature drift
- Update step corrects using measurement
- Noise is strongly suppressed


🔟 Stable Temperature Output
The final result is a smooth, stable, physically realistic temperature suitable for:
- Logging
- Display
- Control systems
- Alarms

---

#✅ Key Advantages of This Design
- Hardware‑accurate ADC usage
- Correct TMP37 math
- Strong noise suppression
- Easy to extend to FreeRTOS
- Educational and production‑ready

---

#🚀 Possible Extensions
- DMA‑based ADC sampling
- FreeRTOS temperature task
- UART logging
- Fixed‑point optimization
- Multi‑sensor Kalman fusion

---

## Tested On
- STM32F446RE @ 16 MHz
- HAL drivers
- TMP37 sensor

---

## License
This project is released under the MIT License.
You are free to use, modify, and share.

---

## Contributions
Pull requests and improvements are welcome.

---

## Acknowledgements
Thanks to the ST community, the SEN0137 hardware documentation,
and the open‑source embedded community.

---
