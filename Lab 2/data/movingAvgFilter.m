function y = movingAvgFilter(data, filterDepth);
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

format shortG;
y = filter(ones(1, filterDepth)/filterDepth, 1, data);

end

