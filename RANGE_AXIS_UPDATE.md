# FFT Spectrum Range Axis Implementation

## Overview
The FFT spectrum plot has been updated to display **Range in meters** instead of sample index on the x-axis. This provides a more meaningful representation of radar data by showing the actual distance to targets.

## Key Changes Made

### 1. FFTWidget Header Updates (`FFTWidget.h`)
- Added radar parameters for range calculation:
  - `m_sampleRate`: ADC sampling rate (Hz)
  - `m_sweepTime`: Chirp sweep time (seconds)
  - `m_bandwidth`: Chirp bandwidth (Hz)
  - `m_centerFreq`: Radar center frequency (Hz)
  - `m_maxRange`: Maximum display range (meters)
- Added new member vector: `m_rangeAxis` to store range values
- Added methods:
  - `setRadarParameters()`: Configure radar system parameters
  - `setMaxRange()`: Set maximum display range
  - `frequencyToRange()`: Convert frequency to range using FMCW radar equation
  - `sampleIndexToRange()`: Convert sample index to range

### 2. FFTWidget Implementation Updates (`FFTWidget.cpp`)

#### Range Calculation
The range calculation uses the standard FMCW radar equation:
```cpp
Range = (beat_frequency * c * sweep_time) / (2 * bandwidth)
```
Where:
- `beat_frequency`: The frequency bin value
- `c`: Speed of light (299,792,458 m/s)
- `sweep_time`: Chirp duration
- `bandwidth`: Chirp bandwidth

#### Spectrum Plotting
- Modified `drawSpectrum()` to use range values instead of sample indices
- Only plots points within the maximum range limit
- X-axis now maps range to pixel coordinates

#### Axis Labels
- X-axis labels now show range in meters (e.g., "0.0m", "60.0m", "120.0m")
- Updated title to "FFT Spectrum - Range Domain"
- Added radar parameter display showing sample rate, bandwidth, and sweep time

### 3. MainWindow Integration (`MainWindow.cpp`)
- Added radar parameter initialization in `setupUI()`
- Connected range control to both PPI and FFT widgets
- Synchronized range settings between displays

## Default Radar Parameters
The system is configured with realistic FMCW radar parameters:
- **Sample Rate**: 100 kHz ADC sampling
- **Sweep Time**: 1 ms chirp duration
- **Bandwidth**: 50 MHz chirp bandwidth  
- **Center Frequency**: 24 GHz (K-band radar)
- **Max Range**: 300 meters (adjustable via UI)

## Range Resolution
With these parameters, the range resolution is approximately:
```
Range Resolution = (c * sweep_time) / (2 * bandwidth)
                 = (3×10⁸ * 0.001) / (2 * 50×10⁶)
                 = 3 meters
```

## User Interface
- The range spinbox control now affects both the PPI display and FFT spectrum
- Range axis automatically scales to the selected maximum range
- Radar parameters are displayed at the bottom of the FFT plot
- Grid lines and labels provide easy range reading

## Technical Benefits
1. **Meaningful Data**: Range axis shows actual target distances
2. **Radar-Specific**: Calculation uses proper FMCW radar physics
3. **Configurable**: Radar parameters can be adjusted for different systems
4. **Synchronized**: PPI and FFT displays use consistent range scales
5. **User-Friendly**: Clear labels and parameter display

## Usage
1. Build and run the application
2. Use the "Max Range" spinbox to adjust the display range
3. The FFT spectrum will show peaks at ranges corresponding to simulated targets
4. Range resolution depends on the configured radar parameters

This implementation provides a professional radar-style display suitable for real FMCW radar systems.