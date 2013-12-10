Balsamo
=======

An interface for unwanted calls filtering, from the spanish acronym "Bloqueador Avanzado de Llamadas Sistemáticas, Automáticas, Molestas y Ocultas"

This project uses a custom PCB equipped with a dsPIC30F6014 for telephone signal sampling, FSK demodulation and system and user interfaces. It also includes an SD card interface used to load and store system configuration, allowed/restricted phone numbers and raw audio files to be played back to the caller.

Everything (source code and board files, excepting external libraries used) is GPLv3+ licensed.

External components used
========================

Balsamo uses the following external components:
- [fatfs by Elm-Chan](http://elm-chan.org/fsw/ff/00index_e.html) (unrestricted free license).
- Modified XLCD by Microchip (can be freely used with its microcontrollers).
- Runtime support and chip support files. These files come with [MPLAB C Compiler for PIC24 MCUs and dsPIC DSCs](http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en010065). Free (as in beer) version of the compiler can be used to build the project. Feel free to add support for other compilers
- dspcommon.inc and iircan.s from [dsPIC DSC DSP Algorithm Library](http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en023598). This library is usually also included with "MPLAB C Compiler for PIC24 MCUs and dsPIC DSCs" and can be freely used with dsPIC products from Microchip.
- (Optional) [Doxygen](http://www.stack.nl/~dimitri/doxygen/) if you want to build the documentation yourself.

More files to come soon!

