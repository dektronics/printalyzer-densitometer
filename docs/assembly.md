Printalyzer Densitometer Assembly
=================================

This document describes the process of assembling a complete
Printalyzer Densitometer from its major components. It assumes that the
assembly of printed circuit boards, along with any soldered-in components,
has been performed prior to following this document. The steps laid out
herein are written with the assumption that they are performed in sequential
order. In practice, independent steps may be reordered as appropriate for
batch assembly.

The majority of the bill-of-materials for this process is contained within
the [enclosure](../enclosure/enclosure.md) document. Please refer there for
detailed descriptions and part numbers for any components listed here.


## Main Board Assembly

### Components

* Main Board (assembled PCB)
* Button caps (x4)
* OLED Display Screen
* Foam mounting tape

### Tools

* Display mounting jig (`jig-display-mounting`)
* Knife or scissors

### Steps

* Mount the display
  * Cut a 3cm piece of mounting tape, and affix to the main board over the
    recangular silkscreen markings that represent the center of a display
    component.
  * Insert the display screen, face-down, into the recessed rectangular area
    on the display mounting jig. Do not remove the protective cover.
  * Remove the outer film from the mounting tape, exposing the rest of the
    adhesive.
  * Carefully lower the main board onto the dowel pins sticking up from the
    mounting jig, until the display is affixed to the tape.
  * Remove the main board from the jig, and carefully apply additional
    pressure with your fingers to ensure everything is securely in place.
  * Insert the display flat flex cable into the socket on the opposing side
    of the main board, and lock it in place with the socket tabs.
* Press-fit the button caps onto the tactile switches on the main board.


## Base

### Components

* Base housing (`dens-base`)
* Stage Plate Outer Ring (`stage-plate-rings`)
* Stage Plate Diffuser (`stage-plate-rings`)
* Transmission Board (assembled PCB)
* Adhesive wheel weights (x6)
* Jumper cable assembly
* M2 x 4mm thread forming screws (x2)

### Tools
 * Base assembly jig (`jig-dens-base-assembly`)
 * Scrap paper
 * Toothpick
 * Wedge-shaped piece of plastic (cut zip-tie end will work)
 * Screwdriver
 * Knife
 * Silicone adhesive
 * Cyanoacrylate glue (liquid and gel)

### Steps

* Affix stage plate components
  * Squirt a blob of cyanoacrylate glue gel onto a scrap of paper
  * Use a toothpick to apply the glue gel to the inside of the diffuser
    hole in the base housing, near the ridge at the bottom of the hole.
    Be very careful, and use a minimal amount.
  * Insert the stage plate diffuser into the hole, matte side up, and
    press down firmly.
  * Squirt a bead of cyanoacrylate glue liquid around the stage plate outer
    ring recess in the base housing. Use a minimal amound, preferably in a
    circle around the midpoint.
  * Insert the stage plate outer ring, matte side up, and press down firmly.
  * Let the piece sit and allow the glue to dry.
* Place the base housing upside-down in the base assembly jig.
* Affix adhesive weights
  * Use a knife on a hard surface to cut the wheel weights into four distinct
    pieces; two singles and two pairs.
  * Remove the backing paper and press the pieces into the underside of the base.
    The singles go near the front of the recess, and the pairs go behind them.
    (see picture)
  * Squirt beads of silicone adhesive into all the crevaces surrounding the
    weights, and use the wedge-shaped plastic as a spatula to smooth it out.
  * Allow to dry for 24 hours.
* Install transmission board
  * Place the board into its spot, and secure with screws.
  * Attach the cable assembly to the board connector.
  * Loop the cable if necessary, for length.
  * Feed the cable out through the cable hole at the rear of the base.


## Base Cover

### Components

* Base cover (`dens-base-cover`)
* Product label
* Rubber bumbers (x4)
* M2 x 8mm flat head black screws (x4)

### Tools

* Base assembly jig (`jig-dens-base-assembly`)
* Screwdriver

### Steps

* Place the Base housing sub-assembly upside-down in the base assembly jig.
* Place cover over the Base housing sub-assembly, and secure with screws
* Apply rubber bumpers in the indents over the screws
* Affix the product label to its indent (can leave this step to the very end)


## Main Housing

### Components

* Main Housing (`dens-main`)
* Diffuser spacer (`diffuser-spacer`)
* Filter ring (`filter-ring`)
* Sensor Board Cover (`sensor-board-cover`)
* Diffuser material sheet
* UV-IR cut filter
* Main Board Assembly
* M2 x 5mm thread forming screws (x6)

### Tools

* Main housing assembly jig (`jig-dens-main-assembly`)
* Screwdriver
* Scissors or rotary trimmer
* Microfiber cloth

### Steps

* Cut a piece of the diffuser material that is just over 14x14mm.
  (It needs to fit into a 15x15mm recess, so some clearance is desired.)
* Place the Main Housing into the Main housing assembly jig.
* Insert the cut diffuser material into body's sensor cone recess,
  matte side up.
* Insert the UV-IR cut filter into the diffuser spacer, and then
  place the combined sub-assembly on top of the diffuser.
  (These parts are best handled with the aid of a microfiber cloth,
  to avoid smudges.)
* Insert the filter ring on top.
* Place the Main Board Assembly into the housing
* Place the Sensor Board Cover on top of the Main Board Assembly.
* Secure with screws the board with screws


## Main Cover

### Components

* Main Cover (`main-cover`)
* Graphic Overlay
* Main Housing Assembly
* M2 x 8mm thread forming screws (x4)

### Tools

* Overlay mounting jig (`jig-overlay-mounting`)
* Screwdriver

### Steps

* If the protective cover over the display window on the graphic overlay is
  encroaching on the button holes, then move it as necessary.
* Place the graphic overlay face-down into the jig
* Carefully remove the backing material from the graphic overlay.
* Align and place the main cover, face down, onto the overlay.
* Remove the main cover, and press on a hard surface to make
  sure the overlay is well adhered.
* Remove the protective cover from the mounted display, which is on
  the main board which is installed within the main housing assembly.
* Place the main cover onto the main housing assembly,
  and secure with screws

_Note: The display window cover on the outer surface of the graphic overlay
should probably be left on through shipping, and removed by the end user._


## Final Assembly

### Components

* Main Body Assembly (Main Body + Main Body Cover)
* Base Assembly (Base + Base Cover)
* RH Torsion Spring
* LH Torsion Spring
* Flange sleeve bearings (x2)
* M3 x 20mm low-profile socket head partially threaded screws (x2)

### Tools

* Spring trimmer jig (`jig-spring-trimmer-base` and `jig-spring-trimmer-cover`)
* Rotary tool with cut-off disc
* Screwdriver

### Steps

* Insert springs into the trimmer jig, and cut ~12mm off the bottom end of
  each one using a rotary tool.
* Take trimmed springs, and insert them into the base assembly hinge area
* Insert the flange sleeve bearings onto the inside-facing spring hole,
  and insert the hinge screws from the outside to hold them in place.
* Connect the transmission LED cable to the Main Body Assembly.
* Carefully install the main body assembly onto the spring tines
  and hinge screw/flange assembly.
* Tighten the hinge screws, alternating, to keep centered alignment.


## Post-Assembly

These steps are just a rough outline of the final part of the process, and
will be further refined once the device is in production.

* Install the bootloader, either pre-loading prior to assembly or via
  USB DFU after assembly.
* Install the firmware, either pre-loading prior to assembly or via
  USB DFU/UF2 after assembly.
* Load the "factory" sensor slope calibration data.
* Run the sensor gain calibration routine.
* Calibrate reflection and transmission modes againt references
  to be included with the package.
