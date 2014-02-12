function y = compareAvgFilterData(rawDataFile, cAvgDataFile, filterDepth, display)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

format shortG;

% Load the raw data and SMA data (from C procedure)
rawData = dlmread(rawDataFile, ',');
cAvgData = dlmread(cAvgDataFile, ',');

% Run MATLAB's own SMA filter at the filterDepth
matlabAvgData = movingAvgFilter(rawData, filterDepth);

len_c = length(cAvgData);
len_m = length(matlabAvgData);
if len_c ~= len_m
    disp('Error: data array length mismatch');
end

% Compare difference in each datum
sum_percent_diff = 0;

if display == 1
    disp('i Mat  C  %Diff');
    disp('---------------------------------------------');
end
for i = 1 : len_m
    percent_diff = abs((cAvgData(i) - round(matlabAvgData(i))))/round(matlabAvgData(i));
    if display == 1
        fprintf('%5d %5d %5d %5d\n', i, ...
            round(matlabAvgData(i)), ...
            round(cAvgData(i)), ...
            percent_diff);
    end
    sum_percent_diff = sum_percent_diff + percent_diff;
end

y = sum_percent_diff ./ len_m;
end

