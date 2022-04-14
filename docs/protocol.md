USB Control Protocol
====================

The Printalyzer Densitometer presents itself to a host computer as a USB CDC
device, which is essentially a virtual serial port. Through this interface,
it is possible to configure and control all aspects of the device.
This document covers that protocol.

## Data Formats

All command and response lines are terminated by a CRLF (`\r\n`).

### Command Format

Each command is sent in a standardized form:

`<TYPE><CATEGORY> <ACTION>[,<ARGS>]`
* **TYPE** values:
  * `S` - Set (change a value or state)
  * `G` - Get (return a value or state)
  * `I` - Invoke (perform some action on the system)
* **CATEGORY** values:
  * `S` - System (top-level system commands)
  * `M` - Measurement (commands related to density measurements)
  * `C` - Calibration (commands related to device calibration)
  * `D` - Diagnostics (general diagnostic commands)
* **ACTION** values are specific to each category, and documented in the following sections
* **ARGS** values are specific to each action, and not all actions may have them

### Response Format

Command responses typically fit a format that is a mirror of the command that
triggered them, except the `[,<ARGS>]` field is changed to represent
information specific to that particular command response.

The common formats for these response arguments are:
* `OK` - The command completed successfully, with no further information
* `ERR` - The command failed
* `NAK` - The command was either unrecognized or can't be processed in the current system state
* `A,B,C,D` - A comma-separated list of elements
  * Strings in this list may be quoted
  * Floating point values are returned in a little-endian hex encoded format
* A multi-line response as follows:
  ```
  TC ACTION,[[\r\n
  Line 1
  Line 2
  ...
  ]]\r\n  
  ```


### Density Format

Density readings are sent out-of-band whenever a reading is taken on the device,
and are not sent in response to commands. They follow the format of:

`<R/T><+/->#.##D`

An example of a density reading would be something like `R+0.20D` or `T+2.85D`.

### Logging Format

Redirected log messages have a unique prefix of `L/`, where "L" is the logging level.
By looking for this pattern, they can be filtered out from the rest of the command
protocol. By default, log messages are not redirected out the USB CDC interface.

Expected log levels include: `A`, `E`, `W`, `I`, `D`, and `V`

## Commands

Note: Commands that could conflict with the local device user interface
may require that the device first be placed into "remote control" mode.
If sent outside of this mode, they may fail with a **NAK**. Commands of
this type are marked below.

Commands that lack a documented response format will return either `OK` or `ERR`.

### System Commands

* `GS V` - Get project name and version
  * Response: `GS V,<Project name>,<Version>`
* `GS B` - Get firmware build information
  * Response: `GS B,<Build date>,<Build describe>,<Checksum>`
* `GS DEV`  - Get device information
  * Response: `GS DEV,<HAL Version>,<MCU Device ID>,<MCU Revision ID>,<SysClock Frequency>`
* `GS RTOS` - Get FreeRTOS information
  * Response: `GS RTOS,<FreeRTOS Version>,<Heap Free>,<Heap Watermark>,<Task Count>`
* `GS UID`  - Get device unique ID
  * Response: `GS UID,<UID>`
* `GS ISEN` - Internal sensor readings
  * Response: `GS ISEN,<VDDA>,<Temperature>`
  * Note: Response elements have unit suffixes appended, so it looks like "3300mV,24.5C"
* `IS REMOTE,n` - Invoke remote control mode (enable = 1, disable = 0)
  * Response: `IS REMOTE,n`
* `SS DISP,text` - Write the provided text to the display
  * Note: Line breaks are sent as the literal text "\n"
    and backslashes are sent as the literal text "\\"

### Measurement Commands

* `GM REFL` - Get last reflection measurement
  * Response: `GM REFL,<D>`
* `GM TRAN` - Get last transmission measurement
  * Response: `GM TRAN,<D>`
* `SM FORMAT,x` - Change measurement output format
  * Possible measurement formats are:
    * `BASIC` - The default format, which just includes the measurement mode
      and density value in a human-readable form, to 2 decimal places
    * `EXT` - Appends the density, zero offset, raw basic count,
      and slope corrected basic count sensor readings in the hex encoded format
  * Note: The active format will revert to **BASIC** upon disconnect
* `SM UNCAL,x` - Allow measurements without target calibration (0=false, 1=true)
  * Note: This setting will revert to false upon disconnect

### Calibration Commands

* `IC GAIN` - Invoke the sensor gain calibration process ***(remote mode)***
  * This is a long process in which the device must be held closed.
    It will automatically abort if the hinge detect switch is released
    during the process.
  * While running, there will be a series of `STATUS` responses followed by
    a single `OK` or `ERR` response.
  * Status is reported via the following responses:
    * `IC GAIN,STATUS,n` where n is an enumerated value as follows:
      * 0 = initializing
      * 1 = measuring medium gain
      * 2 = measuring high gain
      * 3 = measuring maximum gain
      * 4 = calibration process failed
      * 5 = finding gain measurement brightness
      * 6 = waiting between measurements
      * 7 = calibration process complete
    * `IC GAIN,OK` - Gain calibration process is complete
    * `IC GAIN,ERR` - Gain calibration process has failed
* `GC GAIN` - Get sensor gain calibration values
  * Response: `GC GAIN,<L0>,<L1>,<M0>,<M1>,<H0>,<H1>,<X0>,<X1>`
  * Note: `<L0>` and `<L1>` will always be equivalent to 1.0, as they represent
    the sensor's base gain value. They are only included for the sake of completeness.
* `SC GAIN,<M0>,<M1>,<H0>,<H1>,<X0>,<X1>` - Set sensor gain calibration values
  * Note: `<L0>` and `<L1>` are omitted from the set command as their value
    cannot be changed.
* `GC SLOPE` - Get sensor slope calibration values
  * Response: `GC SLOPE,<B0>,<B1>,<B2>`
* `SC SLOPE,<B0>,<B1>,<B2>` - Set sensor slope calibration values
  * _Note: There is no on-device way to perform slope calibration.
    It must be performed using the desktop application, and then
    loaded onto the device via the command interface._
* `GC REFL` - Get reflection density calibration values
  * Response: `GC REFL,<LD>,<LREADING>,<HD>,<HREADING>`
* `SC REFL,<LD>,<LREADING>,<HD>,<HREADING>` - Set reflection density calibration values
  * The reading values are assumed to be in slope corrected basic counts
* `GC TRAN` - Get transmission density calibration values
  * Response: `GC TRAN,<LD>,<LREADING>,<HD>,<HREADING>`
* `SC TRAN,<LD>,<LREADING>,<HD>,<HREADING>` - Get transmission density calibration values
  * The reading values are assumed to be in slope corrected basic counts
  * Note: `<HD>` is always zero, and only included here for the sake of consistency

### Diagnostic Commands

* `GD DISP` - Get display screenshot
  * Response is XBM data in the multi-line format described above
* `SD LR,nnn` -> Set reflection light duty cycle (nnn/127) ***(remote mode)***
  * Light sources are mutually exclusive. To turn both off, set either to 0.
    To turn on to full brightness, set to 128.
* `SD LT,nnn` -> Set transmission light duty cycle (nnn/127) ***(remote mode)***
  * Light sources are mutually exclusive. To turn both off, set either to 0.
    To turn on to full brightness, set to 128.
* `ID S,START` - Invoke sensor start ***(remote mode)***
  * When the sensor task is started via this diagnostic command, there is an
    implicit "Get Reading" command every time a result is available. Thus,
    results are periodically returned as long as the task is running.
  * Result format: `GD S,<CH0>,<CH1>,<GAIN>,<TIME>`
* `ID S,STOP` - Invoke sensor stop ***(remote mode)***
* `SD S,CFG,n,m` - Set sensor gain (n = [0-3]) and integration time (m = [0-5]) ***(remote mode)***
* `ID READ,<L>,<N>,<M>` - Perform controlled sensor target read ***(remote mode)***
  * `<L>` - Measurement light source
    * `0` - Light off
    * `R` - Reflection light, full power
    * `T` - Transmission light, full power
  * `<N>` - Sensor gain (0-3)
  * `<M>` - Sensor integration time (0-5)
  * Result format: `ID READ,<CH0>,<CH1>`
  * Note: This operation behaves similarly to a real measurement, synchronizing
    read light control to the measurement loop, and averaging across a number
    of readings. Unlike a real measurement it uses preconfigured gain and
    integration settings, rather than automatically adjusting them as part
    of the cycle, and returns raw sensor data. It is intended for use as part of
    device characterization routines where repeatable measurement conditions
    are necessary.
* `ID WIPE,<UID>,<CKSUM>` - Factory reset of configuration memory ***(remote mode)***
  * `<UIDw2>` is the last 4 bytes of the device UID, in hex format
  * `<CKSUM>` is the 4 byte checksum of the current firmware image, in hex format
  * _Note: After acknowledging this command, the device will perform the wipe
    and then reset itself. The connection will be lost in the process._
* `SD LOG,U` -> Set logging output to USB CDC device
* `SD LOG,D` -> Set logging output to debug port UART (default)
