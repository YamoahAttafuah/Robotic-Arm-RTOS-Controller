# 4-DOF Robotic Arm Controller (FreeRTOS & NXP KL25Z)

An embedded firmware project controlling a 4-degree-of-freedom robotic arm using the NXP FRDM-KL25Z (ARM Cortex-M0+). The system features dual-mode control (Local/Remote), sequence recording/playback, and real-time concurrency managed via FreeRTOS.

![Status](https://img.shields.io/badge/Status-Completed-success)
![Platform](https://img.shields.io/badge/Platform-NXP%20FRDM--KL25Z-blue)
![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-orange)
![Language](https://img.shields.io/badge/Language-C%20%7C%20Assembly-red)


## Project Overview

This embedded systems project explores real-time task management and peripheral interfacing. It uses FreeRTOS to implement a responsive control loop that interprets various analog inputs, remote UART commands, and ultrasonic sensor data simultaneously, while maintaining safety via a Watchdog timer.

**Technical Features:**
* **Real-Time Operating System:** Utilized FreeRTOS to manage concurrent tasks including motor control, input polling, and status feedback.
* **Direct Register Access:** Drivers for PWM (TPM), ADC, and GPIO were written by manipulating hardware registers directly, without using SDK abstractions.
* **Signal Mapping:** Interpolation of raw ADC values from potentiometers to map to servo duty cycles.
* **Assembly Integration:** A GPIO initialization function (for the 'forwardLED') implemented in assembly.


## Repository Structure

This repository is organized to separate project code from vendor drivers:

* **`/source`**: Main application logic, custom drivers, and header files. **(Please start here)**
* **`/docs`**: Project requirements. (For hardware details, please refer to the [KL25Z Reference Manual](https://www.datasheetarchive.com/datasheet/332a0008e66affee?term=kl25p80m48sf0rm)).
* **`/freertos`**: The FreeRTOS kernel source code (NXP SDK adaptation).
* **`/drivers`**: NXP peripheral drivers.
* **`/board`**: Hardware abstraction and clock configuration.
* **`/CMSIS`**: ARM Cortex microcontroller software interface standards.


## System Architecture

### Hardware Stack
* **MCU:** NXP FRDM-KL25Z (ARM Cortex-M0+ @ 48MHz).
* **Actuators:** MG996R & SG90 Servo Motors (PWM frequency 50Hz/20ms).
* **Inputs:** 4x Potentiometers (ADC), Tactile Buttons (GPIO), HC-SR04 Ultrasonic Sensor.
* **Feedback:** LCD (4-bit mode), Piezo Buzzer, RGB LED indicators.

### Firmware Architecture (FreeRTOS)
The system uses a priority-based preemptive scheduling architecture. The following tasks are initialized in `main()`:

| Task Name | Function |
| :--- | :--- |
| `moveMotorsTask` | Updates PWM duty cycles and handles playback logic. |
| `getPosValuesTask` | Polls ADC and buttons to update global state flags.|
| `remoteControlTask` | Suspends waiting for UART interrupts to process ASCII control commands. |
| `watchdogTask` | Services the COP Watchdog to prevent system hangs. |
| `RBBBRtask` | Runs background safety light pattern (in a Red-Blue-Blue-Blue-Red pattern). |
| `motionLightsTask` | Updates directional LED indicators based on arm movement. |
| `buzzTask` | Provides audible feedback based on the direction of base movement. |

## Control Modes

The system operates in two control modes, toggled either via hardware interrupts or serial commands:

### 1. Local Mode (Manual)
Direct mapping of potentiometer resistance to servo angle. ADC values are processed through an interpolation function `interp()` to ensure smooth mapping.

### 2. Remote Mode (UART)
Accepts ASCII characters via UART (9600 baud) to trigger pre-set positions.

**Command List:**
* **Base Rotation:** `q`, `w`, `e`, `r`, `t` (0° to 180° in 45° steps) 
* **Arm Extension:** `a`, `s`, `d`, `f`, `g` (0° to 180°) 
* **Grip Rotation:** `z`, `x`, `c`, `v`, `b` (0° to 180°) 
* **Gripper Control:** `y`, `i`, `o`, `p` (0°, 90°, 135°, 180°) 
* **Mode Switching:**
    * `1`: Switch to Local Mode 
    * `k`: Switch to Remote Mode 
    * `h`: Run Self-Test Sequence

## Sequence Recording & Playback
A custom sequence recording and playback feature was implemented using a struct array in memory.
1.  **Record:** Pressing the Save button captures the current 4-axis coordinate set from the ADC.
2.  **Playback:** From any initial position the robotic arm is placed, the system iterates through the `saved_motor_pos` array, executing movements sequentially using `runMotorPositions()`.

## Technical Snippets

### PWM Generation & Motor Control
Motor control is handled by the Timer/PWM Module (TPM0). I configured the `MOD` register to `0xCCCC` to generate the specific 20ms period required for the servos.

```c
// Direct Register manipulation for PWM from motor_output.h
TPM0->MOD = 0xCCCC; // 20ms period
TPM0->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1);
```

### Signal Quantization
To solve the issue of servo jitter caused by ADC noise, a quantization function was implemented. Instead of feeding raw ADC values directly to the PWM generator, the input is categorized into stable discrete steps (0°, 45°, 90°, 135°, 180°).

```c
// Logic from getQuantizedValue in motor_output.h
if (val >= 0 && val < 10485) return 0xA3D; // 0 deg
else if (val >= 10485 && val < 20970) return 0xCCC; // 45 deg
```

### Assembly Implementation
To demonstrate low-level ABI understanding, the `initForwardLED` function was written purely in assembly:

```armasm
.syntax unified

.section initgpio, "ax"
.global initForwardLED
.type initForwardLED, "function"

initForwardLED:
	// Enabling clock for Port B
	LDR   R0, =0x40048038
	LDR   R1, [R0]
	MOVS  R2, #1
	LSLS  R2, #10
	ORRS  R1, R1, R2
	STR   R1, [R0]

	LDR   R0, =0x4004A000
	LDR   R1, [R0]
	MOVS  R2, #7
	LSLS  R2, #8
	MVNS  R2, R2
	ANDS  R1, R1, R2

	MOVS  R2, #1
	LSLS  R2, #8
	ORRS  R1, R1, R2
	STR   R1, [R0]

	// Setting PTB0 as output
	LDR   R0, =0x400FF054
	LDR   R1, [R0]
	MOVS  R2, #1

	ORRS  R1, R1, R2
	STR   R1, [R0]
 
	BX LR
.end
```