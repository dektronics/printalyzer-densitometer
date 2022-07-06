# Printalyzer Densitometer Enclosure

## Introduction

This directory contains all the assets necessary to produce the physical
enclosures for the Printalyzer Densitometer. This document explains what
component each file represents, and also provides a bill-of-materials
for additional hardware needed for assembly.

The actual assembly instructions are located elsewhere.

## 3D Models

Models designed for 3D printing are provided as both STL and 3MF files,
and are listed without their file extension.

Original CAD files are provided in the native format of the CAD application.

The models that make up the construction of the device must be printed
in a material that is minimally reflective and opaque to both visible and
infrared light.

_**Note: Many of the models for components beyond the main enclosure were
created by deriving from either the main assembly CAD project or related
projects that might include non-distributable models of 3rd party
components. As such, original CAD files may not be provided for everything
listed here.**_

These files are in the `models_3d` subdirectory.

### CAD Projects

* `dens-enclosure.f3z` - Main CAD model for the enclosure, designed in Fusion 360.  
  (_Note: Does not include models of the PCBs or mounting hardware, to avoid
  accidentally redistributing component models that are under restrictive
  license terms._)

### Main Enclosure Components

* `dens-main-housing` - Main housing for the circuit board and major components
* `dens-main-cover` - Top cover for the main housing
* `dens-main-cover-dev` - Alternative top cover with a clearance hole for a debug connector
* `dens-base` - Base housing that contains the transmission LED components
* `dens-base-cover` - Bottom cover for the base housing

### Internal Components

* `diffuser-spacer` - Spacer that sits between the diffuser material and the UV/IR filter
* `filter-ring` - Ring that sits between the UV/IR filter and the main circuit board
* `sensor-board-cover` - Internal cover for the backside of the circuit board above the sensor elements

### Assembly Jigs

* `jig-dens-main-assembly` - Jig to hold the main housing during assembly
* `jig-dens-base-assembly` - Jig to hold the base, upside down, during assembly
* `jig-display-mounting` - Jig to help align the PCB and OLED display for assembly
* `jig-overlay-mounting` - Jig to help align the main cover and graphic overlay for assembly
* `jig-spring-trimmer-base` - Base portion of a jig to help hold the torsion springs
  in place while trimming them with a rotary tool
* `jig-spring-trimmer-cover` - Cover portion of a jig to help hold the torsion springs
  in place while trimming them with a rotary tool
* `jig-upside-down` - Jig to hold the complete device in an upside-down orientation,
  which is useful for mating the top and bottom halves and for installing the base cover
* `jig-cal-strip-measure` - Jig for holding a calibration reference for measurement,
  when the label markings have not yet been applied

## 2D Models

Models designed for laser cutting are provided as DXF files.
They are provided in both single-part and multi-part grid versions.

These files are in the `models_2d` subdirectory.

* `stage-plate-rings.dxf` - Components that make up the stage plate outer ring
  and transmission LED diffuser. Cut from a matte white acrylic material that
  is 3mm thick.
* `filter-ring.dxf` - Alternative version of the filter ring 3D model from
  above. Must be cut from a clean opaque material that is 3mm thick.
* `sensor-board-cover.dxf` - Alternative version of the sensor board cover
  from above. Must be cut from a material that is opaque to visible and
  infrared light and that is approximately 1mm thick. If thicker, then the
  mounting screws in the sensor head area will need to be longer.

## Graphic Overlay

The graphic overlay is a piece of custom cut polycarbonate material
that has printing and a transparent window for the display.
For all device builds so far, it has been ordered from [JRPanel](https://www.jrpanel.com/).
These files are the design artifacts that have been used when placing
that order.

These files are in the `overlay` subdirectory.

* `graphic-overlay-dims.pdf` - CAD drawing showing all the relevant
  dimensions of the overlay
* `graphic-overlay.dxf` - 2D CAD file representing the relevant
  dimensions of the overlay
* `graphic-overlay.svg` - Inkscape design for the overlay, annotated as appropriate.
* `graphic-overlay.pdf` - PDF export of the design, for ease of exchange.

## Product Labels

* TBD

## Additional Components

### Non-PCB Electronic Components

These are components that are not included in the PCB's bill-of-materials,
but are ordered from the same kinds of vendors and are necessary to assemble
a complete device.

* Button caps (x4)
  * **E-Switch 1SWHT**
  * Mouser PN: 612-1S-WHT
* 1.3" OLED Display Screen
  * **ER-OLED013A1-1W**
  * https://www.buydisplay.com/white-1-3-inch-oled-i2c-arduino-ssd1306-display-module-connector-fpc
* Cable to connect Transmission LED Board to Main Board
  * **JST JUMPER 02SR-3S -02SR-3S 4"**
  * Manufacturer PN: A02SR02SR30K102A
  * Digi-Key PN: 455-3619-ND

### Non-Electronic Components

These components are not necessarily sourced from electronics vendors, but
are still necessary for the assembly of the device.

* Diffuser material, cut to slightly smaller than 15x15mm
  * **LGT188-T** (0.21mm thick, matte & glossy, 90.60% opacity)
  * Sourced from AliExpress
  * https://www.aliexpress.com/item/4000604712643.html
* UV/IR Filter, 14mm diameter
  * Sourced from AliExpress
  * https://www.aliexpress.com/item/4001222435297.html

## Assembly Hardware

* M2 x 4mm thread forming screws (x2)
  * McMaster-Carr PN: 90380A324
* M2 x 5mm thread forming screws (x6)
  * McMaster-Carr PN: 90380A325
* M2 x 8mm thread forming screws (x4)
  * McMaster-Carr PN: 90380A327
* M2 x 8mm flat head black screws (x4)
  * McMaster-Carr PN: 91698A204
* M3 x 20mm low-profile socket head partially threaded screws (x2)
  * McMaster-Carr PN: 93070A076
* Torsion Spring, 180 deg Right-Hand wound
  * McMaster-Carr PN: 9271K97
* Torsion Spring, 180 deg Left-Hand wound
  * McMaster-Carr PN: 9271K98
* Flanged sleeve bearings for 3mm shaft, 5mm long (x2)
  * McMaster-Carr PN: 2705T112
* Rubber bumbers (x4)
  * **3M BUMPER CYLIN 0.315" DIA BLACK**
  * Manufacturer PN: SJ5076
  * Digi-Key PN: 3M156065-ND
* Adhesive wheel weights, 1/4 oz (x6)
  * Available from a variety of vendors
* Silicone adhesive
  * **Chipquik NCS10G**
* Foam mounting tape (3/4"x 15ft)
  * **3M VHB 3/4-5-4991**
  * McMaster-Carr PN: 8127A88
