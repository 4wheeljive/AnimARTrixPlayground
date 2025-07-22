
const latestValueSent = document.getElementById('valueSent');
const bleStateContainer = document.getElementById('bleState');
const valAnim = document.getElementById('valAnim');
const debounceDelay = 300;


// Button controls ****************************************************** 

const buttons = [ '1', '2', '21', '22', '23', '24', '25', '26', '27', '28', '29', '30', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', '71', '72', '73', '74', '75', '76', '77', '78', '79', '80', '91', '95', '98', '99' ] 

function processButton(buttonVal){
    if (buttonVal == 1) { 
        if (isWebBluetoothEnabled()){ connectToDevice(); }
    }
    if (buttonVal == 2) { disconnectDevice(); }

    if (buttonVal > 20) { sendButtonCharacteristic(buttonVal); }

    if (buttonVal == 91) { updateParametersUsed(); }  // update parametersUsed when refreshing UI 

}

buttons.forEach(buttonNum => {
    const button = document.getElementById(`btn${buttonNum}`);
    button.addEventListener('click', () => {
        const buttonVal = button.dataset.myNumber;
        processButton(buttonVal)
    });
});

// Checkbox controls ****************************************************

var checkboxStatus = true;
const checkboxes = [ 'RotateAnim', 'Layer1', 'Layer2', 'Layer3', 'Layer4', 'Layer5' ]

checkboxes.forEach(name => {
    const checkbox = document.getElementById(`cx${name}`);
    checkbox.addEventListener('change', () => {
        if (checkbox.checked) {checkboxStatus=true}            
        else {checkboxStatus=false} 
        sendCheckboxCharacteristic(checkbox.id,checkboxStatus);
    });
});

// Parameter controls ********************************************** 

const parameters = [ 'Bright', 'Speed', 'ColOrd', 'Red', 'Green', 'Blue', 'Scale', 'Angle', 'Zoom', 'Twist', 'Radius', 'Edge', 'Z', 'RatBase', 'RatDiff', 'OffBase', 'OffDiff' ];

const controls = {};  
const controlsById = {};

parameters.forEach(name => {

    const form = document.getElementById(`form${name}`);
    const input = document.getElementById(`in${name}`);
    const slider = document.getElementById(`slider${name}`);
    const reset = document.getElementById(`rst${name}`);
   
    controlsById[input.id] = { input };     
    controls[name] = { form, input, slider, reset };
    
    let timer;  

    // When the user types a number (immediate send)
    input.addEventListener('keydown', (e) => {
        if (e.key === 'Enter') {
            e.preventDefault();
            const val = input.value;
            slider.value = val;
            sendNumberCharacteristic(input.id, val);
        }
    });

    // When the user moves the slider (debounced send)
    slider.addEventListener('input', () => {
        const val = slider.value;
        input.value = val;

        clearTimeout(timer);
        timer = setTimeout(() => {
            sendNumberCharacteristic(input.id, val);
        }, debounceDelay);
    });
    
    if (reset) {
        reset.addEventListener('click', (event) => {
            event.preventDefault();
            const defVal = input.defaultValue;
            input.value = defVal;
            slider.value = defVal;
            sendNumberCharacteristic(input.id, defVal);
        });
    }
});

// Function to indicate parameters used ****************************** 

let parameterUsed = [
[true, true, true, true, true, true, true, true, true, false, false],
[true, true, true, true, false, false, true, true, true, true, true],
[true, true, true, false, false, false, true, true, true, true, true],
[true, true, true, false, false, false, true, true, true, true, true],
[true, true, true, true, true, true, false, true, true, true, true],
[true, true, true, false, true, true, true, true, true, false, false],
[true, true, true, false, false, false, true, true, true, false, false],
[true, true, true, false, false, false, true, true, true, false, false],
[true, true, true, true, true, true, true, true, true, true, true],
[true, true, true, false, true, true, true, true, true, true, true]
];

function updateParametersUsed() {
    const fxIndex = parseInt(valAnim.textContent); // Get current animation index
    const forms = document.querySelectorAll('form[data-index]');

    forms.forEach(form => {
        const index = parseInt(form.getAttribute('data-index'));
        const isUsed = parameterUsed[fxIndex]?.[index]; // Optional chaining to avoid errors
        if (typeof isUsed !== 'undefined') {
            form.setAttribute('data-used', isUsed ? 'true' : 'false');
        }
    });
}

// BLE *******************************************************************************

//Define BLE Device Specs
var deviceName ='AnimARTrix Playground';
var bleService =                '19b10000-e8f2-537e-4f6c-d104768a1214';
var ButtonCharacteristic =      '19b10001-e8f2-537e-4f6c-d104768a1214';
var CheckboxCharacteristic =    '19b10002-e8f2-537e-4f6c-d104768a1214';
var NumberCharacteristic =      '19b10003-e8f2-537e-4f6c-d104768a1214';
//var ControlCharacteristic =      '19b10004-e8f2-537e-4f6c-d104768a1214';

//Declare Global Variables to Handle Bluetooth
var bleDevice;
var bleServer;
var bleServiceFound;
var buttonCharacteristicFound;
var checkboxCharacteristicFound;
var numberCharacteristicFound;
//var controlCharacteristicFound;

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

// SEND USER INPUT FUNCTIONS ***********************************************************

function sendButtonCharacteristic(buttonVal) {
    writeButtonCharacteristic(buttonVal);
}

function sendNumberCharacteristic(inputID, inputValue) {
        var sendDoc = {
            "id" : inputID,
            "val" : inputValue
        }
        sendString = JSON.stringify(sendDoc);
        sendBuffer = str2ab(sendString);        
        writeNumberCharacteristic(sendBuffer);
}

function sendCheckboxCharacteristic(inputID, inputValue) {
        var sendDoc = {
            "id" : inputID,
            "val" : inputValue
        }
        sendString = JSON.stringify(sendDoc);
        sendBuffer = str2ab(sendString);        
        writeCheckboxCharacteristic(sendBuffer);
}

// FUNCTIONS TO APPLY NOTIFICATIONS FROM PROGRAM ***************************************

function applyReceivedButton(changeReceived){
 
    // Change in fxIndex due to preset load
    if ( changeReceived < 20 ) {
        var newAnimation = changeReceived ;
        valAnim.innerHTML = newAnimation;
        updateParametersUsed();
        console.log('New animation:', newAnimation);
    }

    // Change in fxIndex due to direct selection
    if ( changeReceived > 20 && changeReceived < 41 ) {
        var newAnimation = changeReceived - 21;
        valAnim.innerHTML = newAnimation;
        updateParametersUsed();
        console.log('New animation:', newAnimation);
    }
}

//function applyReceivedCheckbox(receivedDoc) {}

function applyReceivedNumber(receivedDoc) {
  const ctrl = controlsById[receivedDoc.id];
  if (!ctrl) {
    console.warn('Unknown parameter id:', receivedDoc.id);
    return;
  }
  ctrl.input.value     	 = receivedDoc.val;
}

//function applyReceivedControl(receivedDoc) {}

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
                })

            service.getCharacteristic(CheckboxCharacteristic)
            .then(characteristic => {
                checkboxCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleCheckboxCharacteristicChange);
                characteristic.startNotifications();				
                })

            service.getCharacteristic(NumberCharacteristic)
            .then(characteristic => {
                numberCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleNumberCharacteristicChange);
                characteristic.startNotifications();				
                })      
  
            /*service.getCharacteristic(ControlCharacteristic)
            .then(characteristic => {
                controlCharacteristicFound = characteristic;
                characteristic.addEventListener('characteristicvaluechanged', handleControlCharacteristicChange);
                characteristic.startNotifications();				
                })*/      
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
    console.log("Server receipt: Button value - ",changeReceived);
    applyReceivedButton(changeReceived);
}

function handleCheckboxCharacteristicChange(event){
    const changeReceived = new TextDecoder().decode(event.target.value);
    const receivedDoc = JSON.parse(changeReceived);
    console.log("Server receipt: ",receivedDoc.id," - ",receivedDoc.val);
	//applyReceivedCheckbox(receivedDoc);
}

function handleNumberCharacteristicChange(event){
    const changeReceived = new TextDecoder().decode(event.target.value);
    const receivedDoc = JSON.parse(changeReceived);
    console.log("Server receipt: ",receivedDoc.id," - ",receivedDoc.val);
	applyReceivedNumber(receivedDoc);
}

/*function handleControlCharacteristicChange(event){
    const changeReceived = new TextDecoder().decode(event.target.value);
    const receivedDoc = JSON.parse(changeReceived);
    console.log("Server receipt: ...");
	applyReceivedControl(receivedDoc);
}*/


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

function writeCheckboxCharacteristic(sendBuffer){
    if (bleServer && bleServer.connected) {
        bleServiceFound.getCharacteristic(CheckboxCharacteristic)
        .then(characteristic => {
            return characteristic.writeValue(sendBuffer);
        })
        .then(() => {
            const decodedBuffer = new TextDecoder().decode(sendBuffer);
            latestValueSent.innerHTML = decodedBuffer;
            console.log("Value written to Checkbox characteristic: ", decodedBuffer);
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
