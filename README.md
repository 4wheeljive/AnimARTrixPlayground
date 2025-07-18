<img width="2702" height="1347" alt="Screenshot 2025-07-15 101004" src="https://github.com/user-attachments/assets/3cc94aef-4ea8-49b0-926b-136e1fc244db" />

This is a test "playground" for adding user controls to selected animations included in FastLED's AnimARTrix library:
https://github.com/FastLED/FastLED/tree/master/examples/Animartrix

This is all very rough, experimental, etc.

Following are links to a couple of low-tech demos:
- This one shows some of the different animation modes and roughly demonstrates the major adjustment factors:
  https://youtu.be/bHqvAAaftNA
- This one shows more deliberate fine-tuning of a particular animation (particularly after around 3:30):  
  https://youtu.be/l7NDfyx3RAU

BASIC FRAMEWORK:
- The src folder here includes custom "myAnimartrix" versions of the standard /fx/2d/animartrix.hpp and /fx/2d/animartrix_detail.hpp files
- There are currently about 10 animations set up in the custom .hpp files (selected from the standard ~55 or so)
- There are currently about 15 variables set up for the user to control various parameters within each of the animations
- The playground can be used in two different ways:
  - To drive a physical LED panel from MCU using web BLE and a custom UI        
  - To drive FastLED's WASM compiler/browser with built-in FastLED UI controls

LED PANEL MODE:
    
1.  Ensure your MCU is BLE capable and enabled. 

2.  Comment/uncomment the SCREEN_TEST defs in main.cpp as follows:

    //#define SCREEN_TEST
    
    #undef SCREEN_TEST

3.  Adjust your data pins, mappings and other LED settings as needed in main.cpp

4.  Load https://4wheeljive.github.io/AnimARTrixPlayground/ from a Web BLE-capable browser.

    NOTE:   On iOS devices, standard browsers (e.g., Safari, Chrome) do not currently support Web BLE.
            I've had success using the Bluefy browser:
            https://apps.apple.com/us/app/bluefy-web-ble-browser/id1492822055
            If Bluefy doesn't connect to your device on its own, the nRF Connect app might help:
            https://apps.apple.com/us/app/nrf-connect-for-mobile/id1054362403

When running in the actual LED panel mode, this sketch can accommodate two different setups, 
selected by defining/undefining BIG_BOARD in main.cpp as follows:

When BIG_BOARD is defined:
- Separate FastLED controllers are enabled for multiple pins to drive multiple panels/strips
- LED mapping will likely need to be done by custom XYMap functions and arrays, which are enabled by default

When BIG_BOARD is undefined:
- One FastLED controller is used for single pin/panel/strip
- Custom LED mapping may be unnecessary

FASTLED COMPILER MODE:

I have not used the Screen Test/ WASM functionality since very early development of the playground. Certain variables/UI elements may be broken/missing/disconnected. Known current/potential issues for using the compiler:
  - The UINumberField controls for fxIndex and colorOrder were producing floats. It seemed to work better for me for those paramaters to be uint8_t, so that's how everything else is currently set up.
  - I have not updated the min/max/step values for the UISliders to reflect the current settings I have in the index.html file for BLE control


PARAMETER CONTROLS

Please see the Wiki (https://github.com/4wheeljive/AnimARTrixPlayground/wiki) for a basic explanation of what the parameter sliders do. 

Also, as an early-stage, poor-man's "preset" system, the Wiki includes a page with screenshots of some parameter combinations that produced cool results.     


USES
- To allow users to explore/create custom animations for their own projects
- To allow for "guest" access control of a display (e.g., at a party) through BLE with a mobile device
- I think this may be along the lines of what Stefan Petrick has mentioned in the past: https://www.reddit.com/r/FastLED/comments/1amlnvn/comment/kpx4199/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button


TODO
- Get UI to reflect current controller parameters on BLE reconnect
- Create framework for saving and retrieving animation presets
- Fine-tune parameter:
  - functions (how they affect an animation)
  - values (ranges/steps/etc.)
  - control (e.g., add oscilation?)
- UI feedback/indication of which parameters do/do not affect the current animation
- Identify/resolve issues to get SCREEN_TEST mode working with current program structure   


DISCUSSION

Since there is some apparently-unsolvable problem with my Reddit account, I've enabled Discussions for this repo (https://github.com/4wheeljive/AnimARTrixPlayground/discussions) so that I can, you know, participate like a normal human.

