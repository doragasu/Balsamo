Balsamo
=======

An interface for unwanted calls filtering. The name is derived from the Spanish acronym "Bloqueador Avanzado de Llamadas Sistemáticas, Automáticas, Molestas y Ocultas", that means something like "Advanced Blocker for Systematic, Automatic, Annoying and Hidden Calls". When it detects an incoming call, BALSAMO decodes the caller number. If the number is configured to be blocked (or if the number is hidden and hidden numbers are configured to be blocked), BALSAMO answers the call with a message pre-recorded inside a SD card.

BALSAMO uses a simple user interface, with a 16x2 characters LCD display, 5 pushbuttons (only 4 are used so far) and 4 LEDs. It also includes a microSD card slot used to load and store system configuration, allowed/restricted phone numbers and raw audio files to be played back to the caller.

This project is powered by a custom PCB equipped with a dsPIC30F6014 for telephone signal sampling, FSK demodulation and system and user interfaces. Audio messages are played using PWM technique, so no additional DAC is needed.

Everything in this project (source code, schematics, board files and documentation, excepting external libraries used) is GPLv3+ licensed.

External components used
========================

Balsamo uses the following external components:
- [fatfs by Elm-Chan](http://elm-chan.org/fsw/ff/00index_e.html) (unrestricted free license).
- Modified XLCD by Microchip (can be freely used with its microcontrollers).
- Runtime support and chip support files. These files come with [MPLAB C Compiler for PIC24 MCUs and dsPIC DSCs](http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en010065). Free (as in beer) version of the compiler can be used to build the project. Feel free to add support for other compilers.
- dspcommon.inc and iircan.s from [dsPIC DSC DSP Algorithm Library](http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en023598). This library is usually also included with "MPLAB C Compiler for PIC24 MCUs and dsPIC DSCs" and can be freely used with dsPIC products from Microchip.
- (Optional) [Doxygen](http://www.stack.nl/~dimitri/doxygen/) if you want to build the documentation yourself.

Editing Schematics and PCB
==========================

Schematics and PCB files can be found under sch/rev-B directory.

Schematics have been created using [gschem](http://www.gpleda.org/) 1.8.1. Note you will need to properly install and configure it, including the free symbols I have created you can find in my [doragasu-gpleda-lib](https://github.com/doragasu/doragasu-gpleda-lib) GitHub repository.

PCB has been routed using [PCB](http://pcb.geda-project.org) version 1.99z and it's built-in (and buggy!) toporouter. To create the teardrops in the pin to track connections, you will need the [Teardrops Plug-in by DJ Delorie](http://www.delorie.com/pcb/teardrops/). Because you cannot easily undo teardrops once created (or at least I do not know how), there are two PCB designs included in the project: Balsamo.pcb and Balsamo-tear.pcb. Balsamo-tear.pcb is the final PCB (and is the one used to generate the GERBER files). Balsamo.pcb is the same as Balsamo-tear.pcb, but without the teardrops. If you need to edit the PCB, Balsamo.pcb is the one you should use.

It's recommended to install xgsch2pcb to open the Balsamo.gsch2pcb file and easily access the schematics, the PCB and the netlist creation tool. There are also two scripts (*bom* and *drc-check*) to create a bill of materials and to check design rules.

Building the firmware
=====================

As previously stated, [MPLAB C Compiler for PIC24 MCUs and dsPIC DSCs](http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en010065) has been used to build the firmware. The compiler homepage offers a 60 day fully working trial version. After this trial, most compiler optimizations are disabled, but it should be able to generate an unoptimized but working firmware image. I have not yet tried building the firmware using the [gnu-based academic version of the compiler](http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en536656), but it may be worth a try.

Configuration file format
=========================

*TODO*

Creating RAW audio files for BALSAMO
====================================

*TODO*

Some features I might add (if I get the time and motivation)
============================================================

- [ ] Real time clock. This is partially implemented. Finishing it should be dead easy.
- [ ] Logging to a file inside the SD-card. This is also partially implemented, should be easy, but requires that the RTC is finished first.
- [ ] Call recording support. Should be easy using the free [dsPIC DSC Speex Speech Encoding/Decoding Library](http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en023610).
- [ ] Answering machine functionalities. Should also be very easy once voice recording is implemented. It will use the audio amplifier included in Rev.B PCB (but still untested) 
- [ ] Different audio message playing depending on caller number.
- [ ] Use PWM to drive LCD backlight, so the variable resistor used to set backlight intensity can be removed.

Enjoy!
