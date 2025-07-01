
const connectButton = document.getElementById('connectBleButton');
const disconnectButton = document.getElementById('disconnectBleButton');

//const onButton = document.getElementById('onButton');
//const offButton = document.getElementById('offButton');

//const rotateAnimationCheckbox = document.getElementById('rotateAnimationCheckbox');

const formHue = document.getElementById('formHue');
const inputHue = document.getElementById('inputHue');
const valueHue = document.getElementById('valueHue');


const latestValueSent = document.getElementById('valueSent');
const bleStateContainer = document.getElementById('bleState');

var sendString = " ";

var sendDoc;
var confirmDoc;

//Define BLE Device Specs
var deviceName ='json Playground';
var bleService =                '19b10000-e8f2-537e-4f6c-d104768a1214';
//var ButtonCharacteristic =      '19b10001-e8f2-537e-4f6c-d104768a1214';
//var CheckboxCharacteristic =    '19b10002-e8f2-537e-4f6c-d104768a1214';
var NumberCharacteristic =      '19b10003-e8f2-537e-4f6c-d104768a1214';
//var SliderCharacteristic =      '19b10004-e8f2-537e-4f6c-d104768a1214';


//Declare Global Variables to Handle Bluetooth
var bleDevice;
var bleServer;
var bleServiceFound;
//var buttonCharacteristicFound;
//var checkboxCharacteristicFound;
//var sliderCharacteristicFound;
var numberCharacteristicFound;

// ADD EVENT LISTENERS *************************************************************

// Connect Button (search for BLE Devices only if BLE is available)
    connectButton.addEventListener('click', (event) => {
        if (isWebBluetoothEnabled()){ connectToDevice(); }
    });

// Disconnect Button
    disconnectButton.addEventListener('click', disconnectDevice);
/*
// Button Characteristic Inputs (Buttons)
    onButton.addEventListener('click', () => writeButtonCharacteristic(1));
    offButton.addEventListener('click', () => writeButtonCharacteristic(99));
        

// Checkbox Characteristic Inputs	
    rotateAnimationCheckbox.addEventListener('change', () => {
        if (rotateAnimationCheckbox.checked) {
            writeControlCharacteristic(100);
        }
        else { 
            writeControlCharacteristic(101); 
        }
    });

*/

// NumberA Input (Number)
    formHue.addEventListener('submit', function(event) {
        event.preventDefault();
        const elementID = inputHue.id;
        const elementValue = inputHue.value;
        sendDoc["id"] = elementID;
        sendDoc["value"] = elementValue;
        serializeJson(sendDoc, sendString);
        writeNumberCharacteristic(sendString);
    });

/*
// SliderA Input (Slider)
    formSliderA.addEventListener('input', function(event) {
        event.preventDefault();
        const elementID = inputHue.id;
        const elementValue = inputHue.value;
        sendDoc["id"] = elementID;
        sendDoc["value"] = elementValue;
        serializeJson(sendDoc, sendString);
        writeSliderCharacteristic(sendString);
    });
*/

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

            /*
            service.getCharacteristic(ButtonCharacteristic)
            .then(characteristic => {
                buttonCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleButtonCharacteristicChange);
                characteristic.startNotifications();				
                return characteristic.readValue();
                })
            //.then(value => {
            //    const decodedValue = new TextDecoder().decode(value);
            //    console.log("Animation: ", decodedValue);
            //    animationValue.innerHTML = decodedValue;
            //    })
            */
            
            service.getCharacteristic(NumberCharacteristic)
            .then(characteristic => {
                numberCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleNumberCharacteristicChange);
                characteristic.startNotifications();				
                return characteristic.readValue();
                })
            .then(value => {
                const decodedValue = new TextDecoder().decode(value);
                console.log("Number: ", decodedValue);
                valueHue.innerHTML = decodedValue;
                })

            /*
            service.getCharacteristic(CheckboxCharacteristic)
            .then(characteristic => {
                checkboxCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleCheckboxCharacteristicChange);
                characteristic.startNotifications();				
                return characteristic.readValue();
                })
            //.then(value => {
            //    const decodedValue = new TextDecoder().decode(value);
            //    console.log("Color: ", decodedValue);
            //    colorOrderValue.innerHTML = decodedValue;
            //    })

            service.getCharacteristic(SpeedCharacteristic)
                .then(characteristic => {
                speedCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleSpeedCharacteristicChange);
                characteristic.startNotifications();	
                return characteristic.readValue();
                })
                .then(value => {
                const decodedValue = new TextDecoder().decode(value);
                console.log("Speed: ", decodedValue);
                speedValue.innerHTML = decodedValue;
                })

            service.getCharacteristic(ScaleCharacteristic)
                .then(characteristic => {
                scaleCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleScaleCharacteristicChange);
                characteristic.startNotifications();	
                return characteristic.readValue();
                })
                .then(value => {
                const decodedValue = new TextDecoder().decode(value);
                console.log("Scale: ", decodedValue);
                scaleValue.innerHTML = decodedValue;
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

/*function handleAnimationCharacteristicChange(event){
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("New fxIndex: ", newValueReceived);
    animationValue.innerHTML = newValueReceived;
}

function handleColorCharacteristicChange(event){
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("New color order: ", newValueReceived);
    colorOrderValue.innerHTML = newValueReceived;
}

function handleSpeedCharacteristicChange(event){
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("New speed: ", newValueReceived);
    speedValue.innerHTML = newValueReceived;
}

function handleScaleCharacteristicChange(event){
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("New scale: ", newValueReceived);
    scaleValue.innerHTML = newValueReceived;
}

function handleControlCharacteristicChange(event){
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("Control value received: ", newValueReceived);
    controlValue.innerHTML = newValueReceived;
}


function handleSliderCharacteristicChange(event){
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("New slider: ", newValueReceived);
    valueSliderA.innerHTML = newValueReceived;
}
*/

function handleNumberCharacteristicChange(event){
    const newValueReceived = new TextDecoder().decode(event.target.value);
    console.log("Number received: ", newValueReceived);
    //valueSliderA.innerHTML = newValueReceived;

    // parse and set different updates for different elements
}





// WRITE TO CHARACTERISTIC FUNCTIONS *************************************************
/*
function writeButtonCharacteristic(value){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(ButtonCharacteristic)
        .then(characteristic => {
            const data = new Uint8Array([value]);
            return characteristic.writeValue(data);
        })
        .then(() => {
        //    animationValue.innerHTML = value;
        //    latestValueSent.innerHTML = value;
            console.log("Value written to Button characteristic: ", value);
        })
        .catch(error => {
            console.error("Error writing to Button characteristic: ", error);
        });
    } else {
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
        //    latestValueSent.innerHTML = value;
            console.log("Value written to Checkbox characteristic:", value);
        })
        .catch(error => {
            console.error("Error writing to the Checkbox characteristic: ", error);
        });
    } else {
        console.error ("Bluetooth is not connected. Cannot write to characteristic.")
        window.alert("Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!")
    }
}

function writeSliderCharacteristic(value){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(SliderCharacteristic)
        .then(characteristic => {
            return characteristic.writeValue(value);
        })
        .then(() => {
            //testValue.innerHTML = value;
            //latestValueSent.innerHTML = value;
            console.log("Value written to Slider characteristic: ", value);
        })
        .catch(error => {
            console.error("Error writing to Slider characteristic: ", error);
        });
    } else {
        console.error ("Bluetooth is not connected. Cannot write to characteristic.")
        window.alert("Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!")
    }
}
*/

function writeNumberCharacteristic(value){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(NumberCharacteristic)
        .then(characteristic => {
            return characteristic.writeValue(value);
        })
        .then(() => {
            //testValue.innerHTML = value;
            //latestValueSent.innerHTML = value;
            console.log("Value written to Number characteristic: ", value);
        })
        .catch(error => {
            console.error("Error writing to Number characteristic: ", error);
        });
    } else {
        console.error ("Bluetooth is not connected. Cannot write to characteristic.")
        window.alert("Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!")
    }
}

