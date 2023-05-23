%% Initialization

% Get a new RIS object from serial port
ris = serialport('COM7', 115200);

% Clear input buffer
pause(0.1);
while ris.NumBytesAvailable > 0
    readline(ris);
    pause(0.01);
end

%% Control examples

% Set pattern
writeline(ris, '!0XFFC00FFC00FFC00FFC00FFC00FFC00');
% Get response
pause(0.1);
while ris.NumBytesAvailable > 0
    response = readline(ris);
    fprintf("Response from setting a pattern: %s\n", response);
end

% Read pattern
writeline(ris, '?Pattern');
% Get response
pause(0.1);
while ris.NumBytesAvailable > 0
    currentPattern = readline(ris);
    fprintf("Current pattern: %s\n", currentPattern);
end

% Read external supply voltage
writeline(ris, '?Vext');
% Get response
pause(0.1);
while ris.NumBytesAvailable > 0
    externalVoltage = readline(ris);
    fprintf("External supply voltage: %s\n", externalVoltage);
end

% Set Static Pass Key
writeline(ris, '!BT-Key=524953');
% Get response
% Wait long enough or check ris.NumBytesAvailable for becoming non-zero
pause(5);
while ris.NumBytesAvailable > 0
    response = readline(ris);
    fprintf("Response from setting a new Static Pass Key: %s\n", response);
end

%% Deinitialization
clear ris;
