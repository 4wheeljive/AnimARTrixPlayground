
const connectButton = document.getElementById('connectBleButton');
const disconnectButton = document.getElementById('disconnectBleButton');

const polarWavesButton = document.getElementById('polarWavesButton');
const spiralusButton = document.getElementById('spiralusButton');
const caleido1Button = document.getElementById('caleido1Button');
const wavesButton = document.getElementById('wavesButton');
const chasingSpiralsButton = document.getElementById('chasingSpiralsButton');
const ringsButton = document.getElementById('ringsButton');
const complexKaleidoButton = document.getElementById('complexKaleidoButton');
const experiment10Button = document.getElementById('experiment10Button');
const experimentSM1Button = document.getElementById('experimentSM1Button');
const offButton = document.getElementById('offButton');
const valueAnimation = document.getElementById('valueAnimation');

const rotateAnimationCheckbox = document.getElementById('rotateAnimationCheckbox');

const formBrightness = document.getElementById('formBrightness');
const inputBrightness = document.getElementById('inputBrightness');
const valueBrightness = document.getElementById('valueBrightness');

const formSpeed = document.getElementById('formSpeed');
const inputSpeed = document.getElementById('inputSpeed');
const valueSpeed = document.getElementById('valueSpeed');
const defaultValueSpeed = valueSpeed.defaultValue;

const formColorOrder = document.getElementById('formColorOrder');
const inputColorOrder = document.getElementById('inputColorOrder');
const valueColorOrder = document.getElementById('valueColorOrder');

const formRed = document.getElementById('formRed');
const inputRed = document.getElementById('inputRed');
const valueRed = document.getElementById('valueRed');
const defaultValueRed = valueRed.defaultValue;

const formGreen = document.getElementById('formGreen');
const inputGreen = document.getElementById('inputGreen');
const valueGreen = document.getElementById('valueGreen');
const defaultValueGreen = valueGreen.defaultValue;

const formBlue = document.getElementById('formBlue');
const inputBlue = document.getElementById('inputBlue');
const valueBlue = document.getElementById('valueBlue');
const defaultValueBlue = valueBlue.defaultValue;

const formScale = document.getElementById('formScale');
const inputScale = document.getElementById('inputScale');
const valueScale = document.getElementById('valueScale');
const defaultValueScale = inputScale.defaultValue;
const resetScaleButton = document.getElementById('resetScaleButton');

const formAngle = document.getElementById('formAngle');
const inputAngle = document.getElementById('inputAngle');
const valueAngle = document.getElementById('valueAngle');
const defaultValueAngle = valueAngle.defaultValue;

const formRadiusA = document.getElementById('formRadiusA');
const inputRadiusA = document.getElementById('inputRadiusA');
const valueRadiusA = document.getElementById('valueRadiusA');
const defaultValueRadiusA = valueRadiusA.defaultValue;

const formZ = document.getElementById('formZ');
const inputZ = document.getElementById('inputZ');
const valueZ = document.getElementById('valueZ');
const defaultValueZ = valueZ.defaultValue;

const formRatiosBase = document.getElementById('formRatiosBase');
const inputRatiosBase = document.getElementById('inputRatiosBase');
const valueRatiosBase = document.getElementById('valueRatiosBase');
const defaultValueRatiosBase = valueRatiosBase.defaultValue;

const formRatiosDiff = document.getElementById('formRatiosDiff');
const inputRatiosDiff = document.getElementById('inputRatiosDiff');
const valueRatiosDiff = document.getElementById('valueRatiosDiff');
const defaultValueRatiosDiff = valueRatiosDiff.defaultValue;

const formOffsetsBase = document.getElementById('formOffsetsBase');
const inputOffsetsBase = document.getElementById('inputOffsetsBase');
const valueOffsetsBase = document.getElementById('valueOffsetsBase');
const defaultValueOffsetsBase = valueOffsetsBase.defaultValue;

const formOffsetsDiff = document.getElementById('formOffsetsDiff');
const inputOffsetsDiff = document.getElementById('inputOffsetsDiff');
const valueOffsetsDiff = document.getElementById('valueOffsetsDiff');
const defaultValueOffsetsDiff = valueOffsetsDiff.defaultValue;

const latestValueSent = document.getElementById('valueSent');
const bleStateContainer = document.getElementById('bleState');

const debounceDelay = 300;
var switchNumber = 0;

//Define BLE Device Specs
var deviceName ='AnimARTrix Playground';
var bleService =                '19b10000-e8f2-537e-4f6c-d104768a1214';
var ButtonCharacteristic =      '19b10001-e8f2-537e-4f6c-d104768a1214';
var CheckboxCharacteristic =    '19b10002-e8f2-537e-4f6c-d104768a1214';
var NumberCharacteristic =      '19b10003-e8f2-537e-4f6c-d104768a1214';

//Declare Global Variables to Handle Bluetooth
var bleDevice;
var bleServer;
var bleServiceFound;
var buttonCharacteristicFound;
var checkboxCharacteristicFound;
var numberCharacteristicFound;

// UTILITY FUNCTIONS *******************************************************************

// Convert between String and ArrayBuffer

function str2ab(str) {
    var buf = new ArrayBuffer(str.length*2); // 2 bytes for each char
    var bufView = new Uint8Array(buf);
    for (var i=0, strLen=str.length; i < strLen; i++) {
    bufView[i] = str.charCodeAt(i);
    }
    return buf;
}

function ab2str(buf) {
    return String.fromCharCode.apply(null, new Uint8Array(buf));
}

// Debounce sliders

    const debounce = (inputID, inputValue) => {
        let timer;
        return (inputID, inputValue) =>{
            clearTimeout(timer);
            timer = setTimeout(() => {
                sendNumberCharacteristic(inputID, inputValue);
            }, debounceDelay);
        };
    };

    const debouncedSpeed = debounce(inputSpeed.id, inputSpeed.value);
	const debouncedBrightness = debounce(inputBrightness.id, inputBrightness.value);
	const debouncedColorOrder = debounce(inputColorOrder.id, inputColorOrder.value);
    const debouncedRatiosBase = debounce(inputRatiosBase.id, inputRatiosBase.value);
    const debouncedRatiosDiff = debounce(inputRatiosDiff.id, inputRatiosDiff.value);
    const debouncedOffsetsBase = debounce(inputOffsetsBase.id, inputOffsetsBase.value);
    const debouncedOffsetsDiff = debounce(inputOffsetsDiff.id, inputOffsetsDiff.value);
    const debouncedScale = debounce(inputScale.id, inputScale.value);
    const debouncedAngle = debounce(inputAngle.id, inputAngle.value);
	const debouncedRadiusA = debounce(inputRadiusA.id, inputRadiusA.value);
	const debouncedZ = debounce(inputZ.id, inputZ.value);
	const debouncedRed = debounce(inputRed.id, inputRed.value);
	const debouncedGreen = debounce(inputGreen.id, inputGreen.value);
	const debouncedBlue = debounce(inputBlue.id, inputBlue.value);

// Create a send buffer for the NumberCharacteristic

function sendNumberCharacteristic(inputID, inputValue) {
        var sendDoc = {
            "id" : inputID,
            "value" : inputValue
        }
        sendString = JSON.stringify(sendDoc);
        sendBuffer = str2ab(sendString);        
        writeNumberCharacteristic(sendBuffer);
}

function inputSwitcher(receivedID) {
    if (receivedID == "inputSpeed") {switchNumber = 1;};
    if (receivedID == "inputBrightness") {switchNumber = 2;};
    if (receivedID == "inputColorOrder") {switchNumber = 3;};
    if (receivedID == "inputRatiosBase") {switchNumber = 4;};
    if (receivedID == "inputRatiosDiff") {switchNumber = 5;};
    if (receivedID == "inputOffsetsBase") {switchNumber = 6;};
    if (receivedID == "inputOffsetsDiff") {switchNumber = 7;};
    if (receivedID == "inputScale") {switchNumber = 8;};
    if (receivedID == "inputAngle") {switchNumber = 9;};
    if (receivedID == "inputRadiusA") {switchNumber = 10;};
    if (receivedID == "inputZ") {switchNumber = 11;};
    if (receivedID == "inputRed") {switchNumber = 12;};
    if (receivedID == "inputGreen") {switchNumber = 13;};
    if (receivedID == "inputBlue") {switchNumber = 14;};
}

// ADD EVENT LISTENERS *************************************************************

// Connect Button (search for BLE Devices only if BLE is available)
    connectButton.addEventListener('click', (event) => {
        if (isWebBluetoothEnabled()){ connectToDevice(); }
    });

// Disconnect Button
    disconnectButton.addEventListener('click', disconnectDevice);

// Animation Selection (Buttons)
    polarWavesButton.addEventListener('click', () => writeButtonCharacteristic(1));
    spiralusButton.addEventListener('click', () => writeButtonCharacteristic(2));
    caleido1Button.addEventListener('click', () => writeButtonCharacteristic(3));
    wavesButton.addEventListener('click', () => writeButtonCharacteristic(4));
    chasingSpiralsButton.addEventListener('click', () => writeButtonCharacteristic(5));
    ringsButton.addEventListener('click', () => writeButtonCharacteristic(6));
    complexKaleidoButton.addEventListener('click', () => writeButtonCharacteristic(7));
    experiment10Button.addEventListener('click', () => writeButtonCharacteristic(8));
    experimentSM1Button.addEventListener('click', () => writeButtonCharacteristic(9));
    offButton.addEventListener('click', () => writeButtonCharacteristic(99));

// Rotate Animation Toggle (Checkbox)
    rotateAnimationCheckbox.addEventListener('change', () => {
        if (rotateAnimationCheckbox.checked) {
            writeCheckboxCharacteristic(100);
        }
        else { 
            writeCheckboxCharacteristic(101); 
        }
    });

// Sliders

    formSpeed.addEventListener('input', () => {
        debouncedSpeed(inputSpeed.id, inputSpeed.value);
    });

    formBrightness.addEventListener('input', () => {
        debouncedBrightness(inputBrightness.id, inputBrightness.value);
    });

    formColorOrder.addEventListener('input', () => {
        debouncedColorOrder(inputColorOrder.id, inputColorOrder.value);
    });

    formRed.addEventListener('input', () => {
        debouncedRed(inputRed.id, inputRed.value);
    });

    formGreen.addEventListener('input', () => {
        debouncedAngle(inputGreen.id, inputGreen.value);
    });

    formBlue.addEventListener('input', () => {
        debouncedBlue(inputBlue.id, inputBlue.value);
    });

    formScale.addEventListener('input', () => {
        debouncedScale(inputScale.id, inputScale.value);
    });

    formAngle.addEventListener('input', () => {
        debouncedAngle(inputAngle.id, inputAngle.value);
    });

    formRadiusA.addEventListener('input', () => {
        debouncedRadiusA(inputRadiusA.id, inputRadiusA.value);
    });

    formZ.addEventListener('input', () => {
        debouncedZ(inputZ.id, inputZ.value);
    });

    formRatiosBase.addEventListener('input', () => {
        debouncedRatiosBase(inputRatiosBase.id, inputRatiosBase.value);
    });

    formRatiosDiff.addEventListener('input', () => {
        debouncedRatiosDiff(inputRatiosDiff.id, inputRatiosDiff.value);
    });

    formOffsetsBase.addEventListener('input', () => {
        debouncedOffsetsBase(inputOffsetsBase.id, inputOffsetsBase.value);
    });

    formOffsetsDiff.addEventListener('input', () => {
        debouncedOffsetsDiff(inputOffsetsDiff.id, inputOffsetsDiff.value);
    });


// Reset sliders to default values

    resetScaleButton.addEventListener('click', (event) => {
        event.preventDefault();
        sendNumberCharacteristic(inputScale, defaultValueScale);
    });



// BLE CONNECTION *******************************************************************************

// Check if BLE is available in your Browser    
function isWebBluetoothEnabled() {
    if (!navigator.bluetooth) {
        console.log("Web Bluetooth API is not available in this browser!");
        bleStateContainer.innerHTML = "Web Bluetooth API is not available in this browser!";
        return false
    }
    console.log('Web Bluetooth API supported in this browser.');
    return true
}

// Connect to BLE Device and Enable Notifications

function connectToDevice(){
    console.log('Initializing Bluetooth...');
    navigator.bluetooth.requestDevice({
        filters: [{name: deviceName}],
        optionalServices: [bleService]
    })
    .then(device => {
        bleDevice = device;
        console.log('Device Selected:', device.name);
        bleStateContainer.innerHTML = 'Connected to ' + device.name;
        bleStateContainer.style.color = "#24af37";
        device.addEventListener('gattservicedisconnected', onDisconnected);
        return device.gatt.connect();
    })
    .then(gattServer =>{
        bleServer = gattServer;
        console.log("Connected to GATT Server");
        return bleServer.getPrimaryService(bleService);
    })
    .then(service => {
            bleServiceFound = service;
            console.log("Service discovered:", service.uuid);
            service.getCharacteristics().then(characteristics => {
                characteristics.forEach(characteristic => {
                console.log('Characteristic UUID:', characteristic.uuid); 
                });
            });

            service.getCharacteristic(ButtonCharacteristic)
            .then(characteristic => {
                buttonCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleButtonCharacteristicChange);
                characteristic.startNotifications();				
                //return characteristic.readValue();
                })

            service.getCharacteristic(CheckboxCharacteristic)
            .then(characteristic => {
                checkboxCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleCheckboxCharacteristicChange);
                characteristic.startNotifications();				
                //return characteristic.readValue();
                })

            service.getCharacteristic(NumberCharacteristic)
            .then(characteristic => {
                numberCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleNumberCharacteristicChange);
                characteristic.startNotifications();				
                //return characteristic.readValue();
                })
    })
}

// DISCONNECT FUNCTIONS ************************************************************

function onDisconnected(event){
    console.log('Device Disconnected:', event.target.device.name);
    bleStateContainer.innerHTML = 'Device disconnected';
    bleStateContainer.style.color = "#d13a30";
    setTimeout(() => {
        connectToDevice();
    }, 2000);
}

function disconnectDevice() {
    console.log("Disconnect Device.");
    bleServer.disconnect();
    console.log("Device Disconnected");
    bleStateContainer.innerHTML = 'Disconnected';
    bleStateContainer.style.color = "#d13a30";
}

// HANDLE CHARACTERISTIC CHANGE FUNCTIONS *************************************************************


function handleButtonCharacteristicChange(event){
    const changeReceived = new TextDecoder().decode(event.target.value);
    console.log("Server receipt: Animation - ",changeReceived);
    valueAnimation.innerHTML = changeReceived;
}

function handleCheckboxCharacteristicChange(event){
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("Server receipt: Checkbox value - ",newValueReceived);
}

function handleNumberCharacteristicChange(event){
    const changeReceived = new TextDecoder().decode(event.target.value);
    //console.log("Number received: ", changeReceived);
    const receivedDoc = JSON.parse(changeReceived);
    console.log("Server receipt: ",receivedDoc.id," - ",receivedDoc.value);
    
    inputSwitcher(receivedDoc.id)

    switch (switchNumber) {
        case 1:    valueSpeed.innerHTML = receivedDoc.value; break;
        case 2:    valueBrightness.innerHTML = receivedDoc.value; break;
        case 3:    valueColorOrder.innerHTML = receivedDoc.value; break;   
        case 4:    valueRatiosBase.innerHTML = receivedDoc.value; break;
        case 5:    valueRatiosDiff.innerHTML = receivedDoc.value; break;   
        case 6:    valueOffsetsBase.innerHTML = receivedDoc.value; break;
        case 7:    valueOffsetsDiff.innerHTML = receivedDoc.value; break;
        case 8:    valueScale.innerHTML = receivedDoc.value; break;
        case 9:    valueAngle.innerHTML = receivedDoc.value; break;
        case 10:    valueRadiusA.innerHTML = receivedDoc.value; break;
        case 11:    valueZ.innerHTML = receivedDoc.value; break;
        case 12:    valueRed.innerHTML = receivedDoc.value; break;
        case 13:    valueGreen.innerHTML = receivedDoc.value; break;
        case 14:    valueBlue.innerHTML = receivedDoc.value; break;
        default:   console.log("No valid switchNumber found. Received ID: ",receivedDoc.id);
    }
}

// WRITE TO CHARACTERISTIC FUNCTIONS *************************************************

function writeButtonCharacteristic(value){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(ButtonCharacteristic)
        .then(characteristic => {
            const data = new Uint8Array([value]);
            return characteristic.writeValue(data);
        })
        .then(() => {
            latestValueSent.innerHTML = value;
            console.log("Value written to Button characteristic: ", value);
        })
        .catch(error => {
            console.error("Error writing to Button characteristic: ", error);
        });
    } 
    else {
        console.error ("Bluetooth is not connected. Cannot write to characteristic.")
        window.alert("Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!")
    }
}

function writeCheckboxCharacteristic(value){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(CheckboxCharacteristic)
        .then(characteristic => {
            const data = new Uint8Array([value]);
            return characteristic.writeValue(data);
        })
        .then(() => {
            latestValueSent.innerHTML = value;
            console.log("Value written to Checkbox characteristic: ", value);
        })
        .catch(error => {
            console.error("Error writing to Checkbox characteristic: ", error);
        });
    } 
    else {
        console.error ("Bluetooth is not connected. Cannot write to characteristic.")
        window.alert("Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!")
    }
}

function writeNumberCharacteristic(sendBuffer){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(NumberCharacteristic)
        .then(characteristic => {
            return characteristic.writeValue(sendBuffer);
        })
        .then(() => {
            const decodedBuffer = new TextDecoder().decode(sendBuffer);
            latestValueSent.innerHTML = decodedBuffer;
            console.log("Value written to Number characteristic: ", decodedBuffer);
        })
        .catch(error => {
            console.error("Error writing to Number characteristic: ", error);
        });
    } 
    else {
        console.error ("Bluetooth is not connected. Cannot write to characteristic.")
        window.alert("Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!")
    }
}