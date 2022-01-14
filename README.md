# Printalyzer Densitometer

![Printalyzer Densitometer Logo](docs/images/dens-logo.png)

![Printalyzer Densitometer Device](docs/images/dens-device.png)

## Introduction

The Printalyzer Densitometer is a project whose goal is the creation of an
affordable reflection and transmission densitometer for photographic darkroom
use.

It was initially conceived as an accessory to complement the
[Printalyzer Enlarging Timer & Exposure Meter](https://github.com/dektronics/printalyzer-timer)
to ease the process of creating paper profiles. However, given the general
utility of such a device, it has been spun off as a separate project in its
own right.

A good overview of the project can be found in these blog posts:
* [The Printalyzer Densitometer Project](https://hecgeek.blogspot.com/2021/07/the-printalyzer-densitometer-project.html)

_**Note: This project is currently a work-in-progress, and may still go through
major changes before it can be considered to be stable.**_

## Project Layout

### Hardware
The "hardware" directory contains [KiCad](https://www.kicad.org/) projects
with the complete circuit schematics, bill-of-materials, and
PCB layouts.

_**The hardware design is currently marked as Revision C, and has a variety
of changes intended to go into the final pre-production device.**_

### Software
The "software" directory contains all the source code for the firmware
that runs on the hardware.

_**The software is still designed to run on Revision B hardware, because
Revision C prototypes have not yet been constructed. Once they are available,
the software will be updated accordingly.**_

### Enclosure
The "enclosure" directory contains any CAD models and related resources
necessary to physically assemble the device.

## License
Individual directories will contain LICENSE files as needed, with relevant
details. Generally, the hardware will be CC BY-SA and the software will be
BSD 3-Clause. However, external resources may have their own individual license terms.
