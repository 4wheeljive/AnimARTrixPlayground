This is a test "playground" for adding user controls to selected animations included in FastLED's Animartrix library:
https://github.com/FastLED/FastLED/tree/master/examples/Animartrix



This is all very rough, experimental, etc.

The sliders are a handful of parameter variables that are included in various combinations and configurations in each of the Animation functions in myAnimartrix_detail.hpp.

Each slider has its own min/max/step values that I've mapped to be reasonably useful across various animations.

With three exceptions, the Pattern Control parameters are multiplication factors that range from 0.1 to an integer between 2 and 10.

RadiusA and RadiusB values range from -10 to +30 and are added to a radius function in several animations.

RatiosBase is a decimal value that ranges from -0.25 to + 0.50, which is added (one way or another) to certain of the animations' timings.ratios.
Depending on the animation, the RatiosBase:
(1) Is either used as is or divided by 10 or 100 (or in some cases another divisor), and
(2) Is then multiplied by the RatiosDiff parameter and another multiplication factor hard-coded in the animation so that adjustments to the RatiosBase can impact an animation's different ratios in different ways.

The same approach is used for the OffsetsBase and OffsetsDiff parameters (although OffsetsBase is a multiplication factor, unlike RatiosBase).
