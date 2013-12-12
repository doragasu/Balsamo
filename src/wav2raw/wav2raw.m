% wav2raw.m - Converts a wav signal to RAW format. Signal is normalized
% and scaled before storing it to the RAW file. Output RAW file is unsigned:
% the signal excursion will go from 0 to the specified scale value.
%
% INPUT PARAMETERS
% - infile: File name of the input WAV file.
% - outfile: File name of the output RAW file.
% - scale (optional): Signal range will go from 0 to the specified scale
%   parameter. Default value is 255.
% - precision (optional): Number of bits per sample used to write the output
%   raw file. Default is 'uint8'. Browse fwrite documentation for all the
%   allowed values.
%
% NOTES:
% - Make sure scale value 'fits' in the specified precision. Otherwise shit
%   will happen.
% - scale parameter must be a positive integer.
% - When using this function to produce RAW files for PWM usage, scale
%   parameter value must match the value used to reload the PWM timer. E.g.,
%   for a dsPIC30F6014, timval = (Fosc/4)/Fpwm. If Fosc = 22118400 Hz and
%   Fpwm = 32 kHz, scale must be set to 172.
%
% Jesús Alonso Fernández (doragasu)
% License: GPLv3+

function wav2raw(infile, outfile, scale, precision)

% Check input parameters and fill default values when necessary
if nargin < 2
	error('infile and outfile parameters are mandatory!')
end

if nargin < 3
	scale = 255;
end

if nargin < 4
	precision = 'uint8';
end

% Read input file
s = wavread(infile);
% Normalize and scale the signal
s = s / (max(abs(s) * 2)) + 0.5;
s = round(s * scale);

% Write results to output file
fid = fopen(outfile, 'w');
fwrite(fid, s, precision);
fclose(fid);

