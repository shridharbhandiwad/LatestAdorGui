# FFT Spectrum Range Axis Implementation

## Overview
The FFT spectrum plot has been updated to display **Range in meters** instead of sample index on the x-axis. This provides a more meaningful representation of radar data by showing the actual distance to targets. Additionally, the plot now displays **target range indicators** as vertical lines, synchronized with the PPI display.

## Key Changes Made

### 1. FFTWidget Header Updates (`FFTWidget.h`)
- Added radar parameters for range calculation:
  - `m_sampleRate`: ADC sampling rate (Hz)
  - `m_sweepTime`: Chirp sweep time (seconds)
  - `m_bandwidth`: Chirp bandwidth (Hz)
  - `m_centerFreq`: Radar center frequency (Hz)
  - `m_maxRange`: Maximum display range (meters)
- Added new member vectors:
  - `m_rangeAxis`: Store range values in meters
  - `m_currentTargets`: Store target track data for indicators
- Added methods:
  - `setRadarParameters()`: Configure radar system parameters
  - `setMaxRange()`: Set maximum display range
  - `updateTargets()`: Update target information for display
  - `frequencyToRange()`: Convert frequency to range using FMCW radar equation
  - `sampleIndexToRange()`: Convert sample index to range
  - `drawTargetIndicators()`: Draw target range markers

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

#### Target Indicators
- **Vertical dashed lines** at target range positions
- **Color-coded** based on radial velocity:
  - 🔴 **Red**: Approaching targets (radial speed > 5 m/s)
  - 🔵 **Blue**: Receding targets (radial speed < -5 m/s)  
  - 🟢 **Green**: Stationary targets (-5 ≤ speed ≤ 5 m/s)
- **Target information labels** showing:
  - Target ID (T1, T2, etc.)
  - Range in meters
  - Azimuth angle in degrees
- **Smart text positioning** to avoid overlaps and edge clipping
- **Semi-transparent backgrounds** for better text readability

#### Axis Labels
- X-axis labels now show range in meters (e.g., "0.0m", "60.0m", "120.0m")
- Updated title to "FFT Spectrum - Range Domain"
- Added radar parameter display showing sample rate, bandwidth, and sweep time

### 3. MainWindow Integration (`MainWindow.cpp`)
- Added radar parameter initialization in `setupUI()`
- Connected range control to both PPI and FFT widgets
- **Synchronized target updates** between PPI and FFT displays
- Enabled simulation data generation for testing
- Added `m_fftWidget->updateTargets()` call in timer callback

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

## Target Indicator Features

### Visual Design
- **Dashed vertical lines** clearly mark target ranges
- **Consistent color scheme** with PPI display
- **Compact information labels** with target details
- **Azimuth filtering** (only shows targets within -90° to +90°)
- **Range filtering** (only shows targets within display range)

### Target Information Display
Each target indicator shows:
```
T1      ← Target ID
125.3m  ← Range in meters  
45°     ← Azimuth angle
```

### Synchronization
- Target updates synchronized between PPI and FFT displays
- Range control affects both displays simultaneously
- Same color coding and filtering rules applied

## User Interface
- The range spinbox control now affects both the PPI display and FFT spectrum
- Range axis automatically scales to the selected maximum range
- Target indicators automatically update with new target data
- Radar parameters are displayed at the bottom of the FFT plot
- Grid lines and labels provide easy range reading

## Technical Benefits
1. **Meaningful Data**: Range axis shows actual target distances
2. **Target Correlation**: Visual correlation between FFT peaks and target positions
3. **Radar-Specific**: Calculation uses proper FMCW radar physics
4. **Configurable**: Radar parameters can be adjusted for different systems
5. **Synchronized**: PPI and FFT displays use consistent range scales and target data
6. **User-Friendly**: Clear labels, color coding, and parameter display
7. **Real-time**: Target indicators update automatically with data changes

## Usage
1. Build and run the application
2. Use the "Max Range" spinbox to adjust the display range
3. The FFT spectrum will show:
   - Frequency spectrum as cyan curve
   - Target range indicators as colored dashed lines
   - Target information labels above the plot
4. Target colors and positions match the PPI display
5. Range resolution depends on the configured radar parameters

## Example Display
```
FFT Spectrum - Range Domain
     T1    T2         T3
   125.3m 185.7m    245.1m
     45°   -12°       78°
     |     |          |
  ┌──┼─────┼──────────┼───┐
  │  ¦     ¦          ¦   │ ← Spectrum curve with target indicators
  │  ¦     ¦          ¦   │
  │  ¦     ¦          ¦   │
  └──────────────────────┘
  0.0m  60.0m  120.0m  180.0m  240.0m  300.0m
```

This implementation provides a professional radar-style display suitable for real FMCW radar systems, with clear correlation between spectrum data and target information.