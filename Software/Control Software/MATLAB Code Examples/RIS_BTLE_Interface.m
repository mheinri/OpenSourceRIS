classdef RIS_BTLE_Interface
    
    properties (Access = private)
        % Name of the BT device
        BT_name
        % ble object
        bleObject
        % write characteristic
        writeCharacteristic
        % read characteristic
        readCharacteristic
        % read descriptor
        readDescriptor
    end
    
    methods
        function obj = RIS_BTLE_Interface(BT_name)
            % Set BT name
            obj.BT_name = BT_name;
        end

        %
        % Initialize RIS object by
        %   - opening ble object
        %   - enabling services
        %   - subscribing to characteristics
        %
        function obj = init(obj)

            try
                % Initialize ble instance
                obj.bleObject = ble(obj.BT_name);
                % Initialize write characteristic
                obj.writeCharacteristic = characteristic(obj.bleObject, "6E400001-C352-11E5-953D-0002A5D5C51B", "6E400002-C352-11E5-953D-0002A5D5C51B");
                % Initialize read characteristic
                obj.readCharacteristic = characteristic(obj.bleObject, "6E400001-C352-11E5-953D-0002A5D5C51B", "6E400003-C352-11E5-953D-0002A5D5C51B");
                % Initialize read descriptor
                obj.readDescriptor = descriptor(obj.readCharacteristic, "2902");
                % Enable services
                write(obj.readDescriptor, [1 0]);
                % Subscribe to read characteristic
                subscribe(obj.readCharacteristic);
            catch e
                disp(e.getReport());
            end

        end

        % 
        % Read serial number
        % 
        % Return value: serial number
        %
        function serialNumber = readSerialNumber(obj)
            % Return value, 0 indicates an error
            serialNumber = 0;

            % Read serial number
            response = obj.sendCommand('?SerialNo');

            % If readout was successful
            if ~isempty(response)
                try
                    % Convert response to voltage
                    response = char(response);
                    serialNumber = str2double(response(end-2:end));
                catch e
                    disp(e.getReport());
                end
            end
        end

        % 
        % Read voltage of external power supply
        % 
        % Return value: external supply voltage / V (99.99 in error case)
        %
        function voltage = readExternalSupplyVoltage(obj)
            % Return value, 99.99 V indicates an error
            voltage = 99.99;

            % Read external voltage
            response = obj.sendCommand('?Vext');

            % If readout was successful
            if ~isempty(response)
                try
                    % Convert response to voltage
                    response = char(response);
                    voltage = str2double(response(2:6));
                catch e
                    disp(e.getReport());
                end
            end
        end

        %
        % Read the current pattern in hex format
        %
        % Return value: hex string of current pattern
        %
        function pattern = readPatternHex(obj)

            pattern = [];

            % Read pattern
            response = obj.sendCommand('?Pattern');

            % If readout was successful
            if ~isempty(response)
                try
                    % Convert response to voltage
                    response = char(response(4:end-1));
                    pattern = response;
                catch e
                    disp(e.getReport());
                end
            end

        end

        %
        % Send a command
        %
        % Return value: response from RIS
        %
        function response = sendCommand(obj, command)
            
            % Initialize response variable
            response = '';

            % Send command with preceeding start byte 0x01 and tailing
            % new-line character
            write(obj.writeCharacteristic, [0x01 command 0x0a]);
            try
                response = read(obj.readCharacteristic, 'oldest');
                response = response(2:end-1);
            catch e
                disp(e.getReport())
            end

        end

        % 
        % Reset RIS and deinitialize RIS object
        %
        function obj = resetAndDeinit(obj)
            % Reset RIS
            write(obj.writeCharacteristic, [0x01 '!Reset' 0x0a]);
            
            % Clear object properties
            try
                obj.bleObject = [];
                obj.readDescriptor = [];
                obj.readCharacteristic = [];
                obj.writeCharacteristic = [];
            catch e
                disp(e.getReport());
            end

        end

        %
        % Deinitialize RIS object by
        %   - unsubscribing from characteristics
        %   - clear object properties
        %
        function obj = deinit(obj)
            % Unsibscribe from characteristics
            try
                unsubscribe(obj.readCharacteristic);
            catch e
                disp(e.getReport());
            end

            % Clear object properties
            try
                obj.bleObject = [];
                obj.readDescriptor = [];
                obj.readCharacteristic = [];
                obj.writeCharacteristic = [];
            catch e
                disp(e.getReport());
            end
        end

    end
end
