# app_waveshaper
Audio Programming Project


Saturator/waveshaper

Planning meeting 20.5.2026

TODO:
- Sergio: UI using Juce
- Kuura: DSP
- Vivek: DSP

Definitions:
------------

Waveshape:
- Points that define a waveshaping function
	- Constant: Number of function defining points (8)
	- Variable: Point parameter (N)
	- Variable: Slope parameter? (N-1)
- Discontinuities can generate aliasing?

Dry/Wet:
- Variable: Dry/Wet

Drive:
- Shifting all the points out away from 0
	- Variable: Input gain (dB)
	- Variable: Output gain (dB)

Bypass:
	- Variable: True/false
	
	
Template:
---------
- MRTA::BaseProcessors

	