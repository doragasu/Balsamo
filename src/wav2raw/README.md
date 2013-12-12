wav2raw
=======

Octave/Matlab script for converting wav files to the RAW format used by BALSAMO to play audio files (and maybe also useful for other tasks).

General Usage
=====

You will need to have either GNU Octave or Matlab installed to use the script. GNU Octave is free and is more than enough for this task. Once you have lauched GNU Octave/Matlab, change to the directory where the script is located (with the 'cd' command) and call the wav2raw function. It will do the work and generate the RAW file requested. The function returns nothing, and takes 2 mandatory and 2 optional parameters:
- infile: File name of the input WAV file.
- outfile: File name of the output RAW file.
- scale (optional): Signal range will go from 0 to the specified scale parameter. Default value is 255.
- precision (optional): Number of bits per sample used to write the output RAW file. Default is 'uint8'. Browse fwrite GNU Octave/Matlab documentation for all the allowed values.

When using this script, you must make sure *scale* parameter fits inside *precision*, or the resulting RAW signal will be saturated. Also make sure *scale* is a positive integer.

Using wav2raw for Balsamo
=========================

To use this function for Balsamo, *scale* must be set to 172 (2218400 / 4 / 32000), and default *precision* ('uint8') is OK. It is important that the input wav file is MONO and sampled at 8 kHz. A typical call to wav2raw to generate a file playable by Balsamo is for example:

		wav2raw('audio.wav', 'audio.raw', 172)

As long as `audio.wav` is MONO and sampled at 8 kHz, the resulting `audio.raw` file will be played nicely by Balsamo.

