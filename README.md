#  How to use

## Set up
```cpp
// It needs to know the sample rate before it can do anything
MoogLadder moogLadder {sampleRate}; // on construction 
// or if it's not accessible use the prepare method
moogLadder.prepare(sampleRate);
```
 
## Changing Parameters
```cpp
// Critical Frequency
moogLadder.setCriticalFrequency(500.0); // Any sensible value will work

// Resonance
moogLadder.setResonance(0.4); // Expects values between 0 and 1
// watch out with 1 and higher
```

## Setting Filtertype
```cpp
// Lowpass 12 db per octave
moogLadder.setCoefficients(MoogLadder::makeLoPass12());
// Hipass 24 db per octave
moogLadder.setCoefficients(MoogLadder:makeHiPass24());
```
 