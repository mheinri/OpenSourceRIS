%% Initialization

% Get a new RIS object from serial port
ris = serialport('COM7', 115200);

% Clear input buffer
pause(0.1);
while ris.NumBytesAvailable > 0
    readline(ris);
    pause(0.1);
end

%% Control examples

% Set pattern
writeline(ris, '!0XFFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000');
% Get response
response = readline(ris);
fprintf("Response from setting a pattern: %s\n", response);

% Read pattern
writeline(ris, '?Pattern');
% Get response
currentPattern = readline(ris);
fprintf("Current pattern: %s\n", currentPattern);

% Read external supply voltage
writeline(ris, '?Vext');
% Get response
externalVoltage = readline(ris);
fprintf("External supply voltage: %s\n", externalVoltage);

% Set Static Pass Key
writeline(ris, '!BT-Key=524953');
% Get response
% Wait long enough or check ris.NumBytesAvailable for becoming non-zero
pause(5);
response = readline(ris);
fprintf("Response from setting a new Static Pass Key: %s\n", response);

% Reset RIS
writeline(ris, '!Reset');
% Wait long enough or check ris.NumBytesAvailable for becoming non-zero
pause(1);
while ris.NumBytesAvailable > 0
    response = readline(ris);
    fprintf("Response from resetting RIS: %s\n", response);
    pause(0.1);
end

%% Deinitialization
clear ris;
