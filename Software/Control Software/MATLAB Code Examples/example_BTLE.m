clear all;

%% Initialization

% Get a new RIS object with Device-Identifier A-3163D1
ris = RIS_BTLE_Interface("A-3163D1");
% Initialize RIS object
ris = ris.init();

%% High-level functions

% Read pattern
currentPattern = ris.readPatternHex();
fprintf("Current pattern in hex format: %s\n", currentPattern);

% Read external supply voltage
externalVoltage = ris.readExternalSupplyVoltage();
fprintf("External supply voltage: %.2f V\n", externalVoltage);

%% Raw access

% Set pattern
response = ris.sendCommand('!0XFFC00FFC00FFC00FFC00FFC00FFC00');
fprintf("Response from setting a pattern: %s\n", response);

% Read pattern
currentPattern = ris.sendCommand('?Pattern');
fprintf("Current pattern: %s\n", currentPattern);

% Read external supply voltage
externalVoltage = ris.sendCommand('?Vext');
fprintf("External supply voltage: %s\n", externalVoltage);

%% Deinitialization

% Deinitialize RIS object
ris = ris.deinit();
