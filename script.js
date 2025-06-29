
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

const rotateAnimationCheckbox = document.getElementById('rotateAnimationCheckbox');

const setColorOrderForm = document.getElementById('colorOrderForm');
const colorOrderInput = document.getElementById('colorOrderInput');

const setSpeedForm = document.getElementById('speedForm');
const speedInput = document.getElementById('speedInput');

const setScaleForm = document.getElementById('scaleForm');
const scaleInput = document.getElementById('scaleInput');

const formSliderA = document.getElementById('formSliderA');
const inputSliderA = document.getElementById('inputSliderA');

const latestValueSent = document.getElementById('valueSent');
const bleStateContainer = document.getElementById('bleState');

//Define BLE Device Specs
var deviceName ='AnimARTrix Playground';
var bleService =                '19b10000-e8f2-537e-4f6c-d104768a1214';
var AnimationCharacteristic =   '19b10001-e8f2-537e-4f6c-d104768a1214';
var ColorCharacteristic =       '19b10002-e8f2-537e-4f6c-d104768a1214';
var SpeedCharacteristic =       '19b10003-e8f2-537e-4f6c-d104768a1214';
var ScaleCharacteristic =       '19b10004-e8f2-537e-4f6c-d104768a1214';
var ControlCharacteristic =     '19b10005-e8f2-537e-4f6c-d104768a1214';
var SliderCharacteristic =      '19b10006-e8f2-537e-4f6c-d104768a1214';

//Declare Global Variables to Handle Bluetooth
var bleDevice;
var bleServer;
var bleServiceFound;
var animationCharacteristicFound;
var colorCharacteristicFound;
var speedCharacteristicFound;
var scaleCharacteristicFound;
var controlCharacteristicFound;
var sliderCharacteristicFound;

// ADD EVENT LISTENERS *************************************************************

// Connect Button (search for BLE Devices only if BLE is available)
    connectButton.addEventListener('click', (event) => {
        if (isWebBluetoothEnabled()){ connectToDevice(); }
    });

// Disconnect Button
    disconnectButton.addEventListener('click', disconnectDevice);

// Animation Characteristic Inputs (Buttons)
    polarWavesButton.addEventListener('click', () => writeAnimationCharacteristic(1));
    spiralusButton.addEventListener('click', () => writeAnimationCharacteristic(2));
    caleido1Button.addEventListener('click', () => writeAnimationCharacteristic(3));
    wavesButton.addEventListener('click', () => writeAnimationCharacteristic(4));
    chasingSpiralsButton.addEventListener('click', () => writeAnimationCharacteristic(5));
    ringsButton.addEventListener('click', () => writeAnimationCharacteristic(6));
    complexKaleidoButton.addEventListener('click', () => writeAnimationCharacteristic(7));
    experiment10Button.addEventListener('click', () => writeAnimationCharacteristic(8));
    experimentSM1Button.addEventListener('click', () => writeAnimationCharacteristic(9));
    offButton.addEventListener('click', () => writeAnimationCharacteristic(99));
        
// Color Characteristic Input (Number)
    setColorOrderForm.addEventListener('submit', function(event) {
        event.preventDefault();
        const newColorOrder = colorOrderInput.value;
        //  console.log('New color order number:', newColorOrder);
        writeColorCharacteristic(newColorOrder);
        setColorOrderForm.reset();
    });

// Speed Characteristic Input (Number..eventually->Slider)
    setSpeedForm.addEventListener('submit', function(event) {
        event.preventDefault();
        const newSpeed = speedInput.value;
        //console.log('New speed sent:', newSpeed);
        writeSpeedCharacteristic(newSpeed);
        setSpeedForm.reset();
    });

// Scale Characteristic Input (Number..eventually->Slider)
    setScaleForm.addEventListener('submit', function(event) {
        event.preventDefault();
        const newScale = scaleInput.value;
        //console.log('New scale sent:', newScale);
        writeScaleCharacteristic(newScale);
        setScaleForm.reset();
    });

// Control Characteristic Inputs	
    rotateAnimationCheckbox.addEventListener('change', () => {
        if (rotateAnimationCheckbox.checked) {
            writeControlCharacteristic(100);
        }
        else { 
            writeControlCharacteristic(101); 
        }
    });


// SliderA Input (Slider)
    formSliderA.addEventListener('input', function(event) {
        event.preventDefault();
        const elementID = inputSliderA.id;
        const elementValue = inputSliderA.value;
        const sendString = null;
        sendDoc["id"] = elementID;
        sendDoc["value"] = elementValue;
        serializeJson(sendDoc, sendString);
        writeSliderCharacteristic(sendString);
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

            service.getCharacteristic(AnimationCharacteristic)
            .then(characteristic => {
                animationCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleAnimationCharacteristicChange);
                characteristic.startNotifications();				
                return characteristic.readValue();
                })
            .then(value => {
                const decodedValue = new TextDecoder().decode(value);
                console.log("Animation: ", decodedValue);
                animationValue.innerHTML = decodedValue;
                })

            service.getCharacteristic(ColorCharacteristic)
            .then(characteristic => {
                colorCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleColorCharacteristicChange);
                characteristic.startNotifications();				
                return characteristic.readValue();
                })
            .then(value => {
                const decodedValue = new TextDecoder().decode(value);
                console.log("Color: ", decodedValue);
                colorOrderValue.innerHTML = decodedValue;
                })

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
            
            service.getCharacteristic(ControlCharacteristic)
                .then(characteristic => {
                controlCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleControlCharacteristicChange);
                characteristic.startNotifications();	
                //return characteristic.readValue();
                })
                /*
            .then(value => {
                const decodedValue = new TextDecoder().decode(value);
                console.log("Control: ", decodedValue);
                controlValue.innerHTML = decodedValue;
                })
            */

            /*service.getCharacteristic(BrightnessCharacteristic)
            .then(characteristic => {
                brightnessCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleBrightnessCharacteristicChange);
                characteristic.startNotifications();				
                return characteristic.readValue();
                })
            .then(value => {
                const decodedValue = new TextDecoder().decode(value);
                console.log("Brightness: ", decodedValue);
                brightnessValue.innerHTML = decodedValue;
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

function handleAnimationCharacteristicChange(event){
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


// WRITE TO CHARACTERISTIC FUNCTIONS *************************************************

function writeAnimationCharacteristic(value){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(AnimationCharacteristic)
        .then(characteristic => {
            const data = new Uint8Array([value]);
            return characteristic.writeValue(data);
        })
        .then(() => {
            animationValue.innerHTML = value;
            latestValueSent.innerHTML = value;
            console.log("Value written to Animation characteristic: ", value);
        })
        .catch(error => {
            console.error("Error writing to Animation characteristic: ", error);
        });
    } else {
        console.error ("Bluetooth is not connected. Cannot write to characteristic.")
        window.alert("Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!")
    }
}

function writeColorCharacteristic(value){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(ColorCharacteristic)
        .then(characteristic => {
            const data = new Uint8Array([value]);
            return characteristic.writeValue(data);
        })
        .then(() => {
            colorOrderValue.innerHTML = value;
            latestValueSent.innerHTML = value;
            console.log("Value written to Color characteristic: ", value);
        })
        .catch(error => {
            console.error("Error writing to Color characteristic: ", error);
        });
    } else {
        console.error ("Bluetooth is not connected. Cannot write to characteristic.")
        window.alert("Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!")
    }
}

function writeSpeedCharacteristic(value){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(SpeedCharacteristic)
        .then(characteristic => {
            const data = new Uint8Array([value]);
            return characteristic.writeValue(data);
        })
        .then(() => {
            speedValue.innerHTML = value;
            latestValueSent.innerHTML = value;
            console.log("Value written to Speed characteristic: ", value);
        })
        .catch(error => {
            console.error("Error writing to Speed characteristic: ", error);
        });
    } else {
        console.error ("Bluetooth is not connected. Cannot write to characteristic.")
        window.alert("Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!")
    }
}

function writeScaleCharacteristic(value){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(ScaleCharacteristic)
        .then(characteristic => {
            const data = new Uint8Array([value]);
            return characteristic.writeValue(data);
        })
        .then(() => {
            scaleValue.innerHTML = value;
            latestValueSent.innerHTML = value;
            console.log("Value written to Scale characteristic: ", value);
        })
        .catch(error => {
            console.error("Error writing to Scale characteristic: ", error);
        });
    } else {
        console.error ("Bluetooth is not connected. Cannot write to characteristic.")
        window.alert("Bluetooth is not connected. Cannot write to characteristic. \n Connect to BLE first!")
    }
}

function writeControlCharacteristic(value){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(ControlCharacteristic)
        .then(characteristic => {
            const data = new Uint8Array([value]);
            return characteristic.writeValue(data);
        })
        .then(() => {
            latestValueSent.innerHTML = value;
            console.log("Value written to Control characteristic:", value);
        })
        .catch(error => {
            console.error("Error writing to the Control characteristic: ", error);
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
            latestValueSent.innerHTML = value;
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


