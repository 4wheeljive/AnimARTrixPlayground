
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

const formSpeed = document.getElementById('formSpeed');
const inputSpeed = document.getElementById('inputSpeed');
const valueSpeed = document.getElementById('valueSpeed');

const formBrightness = document.getElementById('formBrightness');
const inputBrightness = document.getElementById('inputBrightness');
const valueBrightness = document.getElementById('valueBrightness');

const latestValueSent = document.getElementById('valueSent');
const bleStateContainer = document.getElementById('bleState');

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


// Utility functions to convert between String and ArrayBuffer

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

// Speed Input (Number)
    formSpeed.addEventListener('submit', function(event) {
        event.preventDefault();
        var sendDoc = {
            "id" : inputSpeed.id,
            "value" : inputSpeed.value
        }
        sendString = JSON.stringify(sendDoc);
        sendBuffer = str2ab(sendString);        
        writeNumberCharacteristic(sendBuffer);
    });

// Brightness Input (Number)
    formBrightness.addEventListener('submit', function(event) {
        event.preventDefault();
        var sendDoc = {
            "id" : inputBrightness.id,
            "value" : inputBrightness.value
        }
        sendString = JSON.stringify(sendDoc);
        sendBuffer = str2ab(sendString);        
        writeNumberCharacteristic(sendBuffer);
    });


// Rotate Animation Toggle (Checkbox)
    rotateAnimationCheckbox.addEventListener('change', () => {
        if (rotateAnimationCheckbox.checked) {
            writeCheckboxCharacteristic(100);
        }
        else { 
            writeCheckboxCharacteristic(101); 
        }
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
                //characteristic.addEventListener('characteristicvaluechanged', handleButtonCharacteristicChange);
                //characteristic.startNotifications();				
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
            /*
            .then(value => {
				receivedString = new TextDecoder().decode(value);; //ab2str(value);
				receivedDoc = JSON.parse(receivedString);
				//receivedID = receivedDoc.id;
				//receivedValue = receivedDoc.value;
				console.log("Number characteristic received: ", receivedDoc.id, ": ", receivedDoc.value);
				valueHue.innerHTML = receivedDoc.value;
                //const decodedValue = new TextDecoder().decode(value);
                //console.log("Number: ", decodedValue);
                })
            */
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
    //const newValueReceived = new TextDecoder().decode(event.target.value);
    //console.log("Number received: ", newValueReceived);
    //valueSliderA.innerHTML = newValueReceived;
    // parse and set different updates for different elements
}

function handleCheckboxCharacteristicChange(event){
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("Number received: ", newValueReceived);
    //valueSliderA.innerHTML = newValueReceived;
    // parse and set different updates for different elements
}

function handleNumberCharacteristicChange(event){
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("Number received: ", newValueReceived);
    //valueSliderA.innerHTML = newValueReceived;
    // parse and set different updates for different elements
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
            //const decodedBuffer = new TextDecoder().decode(sendBuffer);
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
            return characteristic.writeValue(value);
        })
        .then(() => {
            //const decodedBuffer = new TextDecoder().decode(sendBuffer);
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
