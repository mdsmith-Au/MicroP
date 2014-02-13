function C = convertToC( temp )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
Vref25 = 760;
Vref_int = 4095;
Vref = 3012;
slope_GLB = 2.5;

C = ((temp/Vref_int) * Vref - Vref25)/slope_GLB + 25;

end

