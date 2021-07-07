USB Control Protocol
====================

The Printalyzer Densitometer presents itself to a host computer as a USB CDC
device, which is essentially a virtual serial port. Through this interface,
it is possible to configure and control all aspects of the device.
This document covers that protocol.

Top-Level Commands
------------------
* `V` - Show device name and version string


Measurement
-----------
* `MR` - Perform reflection density measurement
* `MT` - Perform transmission density measurement


Calibration
-----------

### Gain Calibration
* `CGM` - Run the gain calibration process
* `CGP` - Print the current gain calibration values

### Reflection Calibration
* CAL-LO Target Measurement
  * `CRLMnnn` - Calibrate using low target with a known density of n.nn (e.g. 123 = 1.23)
  * `CRLP` - Print the current low target calibration values

* CAL-HI Target Measurement
  * `CRHMnnn` - Calibrate using high target with a known density of n.nn (e.g. 123 = 1.23)
  * `CRHP` - Print the current high target calibration values

### Transmission Calibration
* TBD


Diagnostic Commands
-------------------
* About System
  * `DAI` - Print device information (HAL version, MCU Rev ID, MCU Dev ID, SysClock)
  * `DAU` - Print device unique ID

* Light Source Diagnostics
  * `DLRnnn` - Set reflection light duty cycle (nnn/127)
  * `DLTnnn` - Set transmission light duty cycle (nnn/127)
  * `DLAnnn` - Set duty cycle of all light sources (nnn/127)

* Sensor Diagnostics
  * `DSR` - Take a raw sensor reading
  * `DSSnm` - Set sensor gain (n = 0-3) and integration time (m = 0-5)
    * Note: These correspond to values in the sensor's datasheet
  * `DSQ` - Query current sensor settings
