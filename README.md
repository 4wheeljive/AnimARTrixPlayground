This is a test "playground" for adding user controls to selected animations included in FastLED's AnimARTrix library:
https://github.com/FastLED/FastLED/tree/master/examples/Animartrix

This is all very rough, experimental, etc.


Basic Framework
- The src folder here includes custom "myAnimartrix" versions of the standard /fx/2d/animartrix.hpp and /fx/2d/animartrix_detail.hpp files
- There are currently about 10 animations set up in the custom .hpp files (selected from the standard ~55 or so)
- There are currently about 15 variables set up for the user to control various parameters within each of the animations
- The playground can be used in two different ways:
  - To drive FastLED's WASM compiler/browser with built-in FastLED UI controls
  - To drive a physical LED panel using web BLE and a custom UI        

NOTE:  I have not used the Screen Test/ WASM functionality since very early development of the playground. Certain variables/UI elements may be broken/missing/disconnected. Known current/potential issues for using the compiler:
  - The UINumberField controls for fxIndex and colorOrder were producing floats. It seemed to work better for me for those paramaters to be uint8_t, so that's how everything else is currently set up.
  - I have not updated the min/max/step values for the UISliders to reflect the current settings I have in the index.html file for BLE control

Please see the Wiki for a basic explanation of what the parameter sliders do. As an early-stage, poor-man's "preset" system, the Wiki also includes screenshots of some parameter combinations that produced cool results.     

Uses
- To allow users to explore/create custom animations for their own projects
- To allow for "guest" access control of a display (e.g., at a party) through BLE with a mobile device
- I think this may be along the lines of what Stefan Petrick has mentioned in the past: https://www.reddit.com/r/FastLED/comments/1amlnvn/comment/kpx4199/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button


TODO
- Create framework for saving and retrieving animation presets
- Fine-tune parameter:
  - functions (how they affect an animation)
  - values (ranges/steps/etc.)
  - control (e.g., add oscilation?)
- UI feedback/indication of which parameters do/do not affect the current animation 


