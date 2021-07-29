# Printalyzer Densitometer Enclosure & Assembly

## Introduction

This directory contains all the assets necessary to produce the physical
enclosures for the Printalyzer Densitometer. This document explains how
to use those assets, and ultimately how to assemble the device.

_**Note: The enclosure is very much a work-in-progress, and is far from
finalized. Expect the contents of this document and directory to be
incomplete and subject to change.**_

## Electronics

### Main Board

The assets for the main circuit board are contained within the "main-board"
project under the top-level "hardware" directory. This board is installed
into the upper portion of the enclosure.

### Transmission LED Board

The assets for the transmission LED circuit board are contained within the
"trans-led-board" project under the top-level "hardware" directory. This board
is installed into the lower base portion of the enclosure.

## Enclosure

### CAD Models

These files constitute the main design for the enclosure, and other artifacts
may be generated from them:

* `densitometer-enclosure.f3z` - Main CAD model for the enclosure.
  (_Note: Does not include models of the PCBs or mounting hardware, to avoid
  accidentally redistributing component models under restrictive
  license terms._)
* `densitometer-dev-cover.f3z` - Derived version of the top cover, which has
  a clearance hole a debug connector.
* `reflec-std-base.f3z` - Alternative base designed to hold the main unit
  above a reflectance standard for calibration. Can also be used as a jig
  to aid in assembly.
* `button-cap.f3d` - Custom button cap, as an alternative to the stock choices.
* `stage-plate-rings.dxf` - Outline of the stage plate components, which can
  be used for manufacture.

### 3D Printable Files

These files have been generated from the CAD models, and are used to actually
create the enclosure. They must be printed in a material that is minimally
reflective and opaque to infrared light.

* Upper Enclosure
  * `dens-main-housing.stl` - Main housing for the circuit board and major components
  * `dens-main-cover.stl` - Top cover for the main housing
  * `dens-main-cover-dev.stl` - Alternative top cover with a clearance hole for a debug connector
  * `diffuser-spacer.stl` - Spacer that sits between the diffuser material and the UV/IR filter
  * `filter-ring.stl` - Ring that sits between the UV/IR filter and the main circuit board
  * `sensor-board-cover.stl` - Internal cover for the backside of the circuit board above the sensor elements
  * `button-cap.stl` - Custom button cap, as an alternative to the stock choices.
* Lower Enclosure
  * `dens-base.stl` - Base housing that contains the transmission LED components
  * `dens-base-cover.stl` - Bottom cover for the base housing
  * `stage-plate-spacer.stl` - Opaque ring that goes between the inner and outer portions of the stage plate
* Accessory components
  * `reflec-std-base.stl` - Alternate base designed to hold the main unit above a reflectance standard

### Non-PCB Electronic Components

These are components that are not included in the PCB's bill-of-materials,
but are ordered from the same kinds of vendors and are necessary to assemble
a complete device.

* 1.3" OLED Display Screen
  * **ER-OLED013A1-1W**
  * https://www.buydisplay.com/white-1-3-inch-oled-i2c-arduino-ssd1306-display-module-connector-fpc
* Cable to connect Transmission LED Board to Main Board
  * **JST JUMPER 02SR-3S -02SR-3S 8"**
  * Manufacturer PN: A02SR02SR30K203A
  * Digi-Key PN: 455-3621-ND

### Non-Electronic Components

These components are not necessarily sourced from electronics vendors, but
are still necessary for the assembly of the device.

* Diffuser material, cut to 15x15mm
  * **LGT188T** (0.21mm thick, matte & glossy, 90.60% opacity)
  * Sourced from AliExpress
  * Further details TBD
* UV/IR Filter, 14mm diameter
  * Sourced from AliExpress
  * Further details TBD

### Mounting Hardware

* TBD
