This is a test "playground" for adding user controls to selected animations included in FastLED's Animartrix library:
https://github.com/FastLED/FastLED/tree/master/examples/Animartrix

This is all very rough, experimental, etc.

Background


Basic Framework
- The src folder here includes custom "myAnimartrix" versions of the standard /fx/2d/animartrix.hpp and /fx/2d/animartrix_detail.hpp files
- There are currently about 10 animations set up in the custom .hpp files (selected from the standard ~55 or so)
- There are currently about 15 variables set up for the user to control various parameters within each of the animations
- The playground can be used in two different ways:
  - To drive FastLED's WASM compiler/browser with built-in FastLED UI controls
  - To drive a physical LED panel using web BLE and a custom UI        


NOTE:  I have not used the Screen Test/ WASM functionality since very early development of the playground. Certain variables/UI elements may be broken/missing.


Uses
- To allow users to explore/create custom animations for their own projects
- To allow for "guest" access control of a display (e.g., at a party) through BLE with a mobile device

TODO
- Create framework for saving and retrieving animation presets
- Fine-tune parameter:
  - functions (how they affect an animation)
  - values (ranges/steps/etc.)
  - control (e.g., add oscilation?)
- UI feedback/indication of which parameters do/do not affect the current animation 


The Controls

The sliders are a handful of parameter variables that are included in various combinations and configurations in each of the Animation functions in myAnimartrix_detail.hpp.

Each slider has its own min/max/step values that I've mapped to be reasonably useful across various animations.

With three exceptions, the Pattern Control parameters are multiplication factors that range from 0.1 to an integer between 2 and 10.

RadiusA and RadiusB values range from -10 to +30 and are added to a radius function in several animations.

RatiosBase is a decimal value that ranges from -0.25 to + 0.50, which is added (one way or another) to certain of the animations' timings.ratios.
Depending on the animation, the RatiosBase:
(1) Is either used as is or divided by 10 or 100 (or in some cases another divisor), and
(2) Is then multiplied by the RatiosDiff parameter and another multiplication factor hard-coded in the animation so that adjustments to the RatiosBase can impact an animation's different ratios in different ways.

The same approach is used for the OffsetsBase and OffsetsDiff parameters (although OffsetsBase is a multiplication factor, unlike RatiosBase).
