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
  * Certain floating point values may be returned in a little-endian hex
    encoded format if the `HEX` argument is passed as part of the command
    to get them.
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
* `IS REMOTE,n` - Invoke remote control mode (enable = 1, disable = 0)
  * Response: `IS REMOTE,n`

### Measurement Commands

* `GM REFL` - Get last reflection measurement
  * Response: `GM REFL,<D>`
  * Note: Adding `,HEX` will return the response in **HEX** format
* `GM TRAN` - Get last transmission measurement
  * Response: `GM TRAN,<D>`
  * Note: Adding `,HEX` will return the response in **HEX** format
* `SM FORMAT,x` - Change measurement output format
  * Possible measurement formats are:
    * `BASIC` - The default format, which just includes mode and density to 2 decimal places
    * `EXT` - Appends density and the raw basic count sensor reading to more decimal places
    * `EXT` - Appends density and the raw basic count sensor reading in **HEX** format
  * Note: The active format will revert to **BASIC** upon disconnect

### Calibration Commands

* `IC GAIN` - Invoke the sensor gain calibration process ***(remote mode)***
  * This is a long process in which the device must be held closed.
    Unless logging is redirected, it does not currently report progress
    via the USB CDC interface.
* `IC LR` - Invoke the reflection light source calibration process ***(remote mode)***
  * This is a long process in which the device must be held closed.
    Unless logging is redirected, it does not currently report progress
    via the USB CDC interface.
  * _Note: This process mostly exists to profile light source behavior, and may
    be removed or disabled in the future._
* `IC LT` - Invoke the transmission light source calibration process ***(remote mode)***
  * This is a long process in which the device must be held closed.
    Unless logging is redirected, it does not currently report progress
    via the USB CDC interface.
  * _Note: This process mostly exists to profile light source behavior, and may
    be removed or disabled in the future._
* `GC GAIN` - Get sensor gain calibration values
  * Response: `GC GAIN,1.00,1.00,<M0>,<M1>,<H0>,<H1>,<X0>,<X1>`
  * Note: Adding `,HEX` will return the response in **HEX** format
* `GC LR` - Get reflection light source calibration value
  * Response: `GC LR,<DROP_FACTOR>`
* `GC LT` - Get reflection light source calibration value
  * Response: `GC LT,<DROP_FACTOR>`
* `GC REFL` - Get reflection density calibration values
  * Response: `GC REFL,<D>,<READING>`
  * Note: Adding `,HEX` will return the response in **HEX** format
* `GC TRAN` - Get transmission density calibration values
  * Response: `GC TRAN,<D>,<READING>`
  * Note: Adding `,HEX` will return the response in **HEX** format

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
* `ID S,STOP` - Invoke sensor stop ***(remote mode)***
* `SD S,CFG,n,m` - Set sensor gain (n = [0-3]) and integration time (m = [0-5]) ***(remote mode)***
* `GD S,READING` - Get next sensor reading ***(remote mode)***
* `SD LOG,U` -> Set logging output to USB CDC device
* `SD LOG,D` -> Set logging output to debug port UART (default)
