//#pragma once

//#include <QWidget>
//#include <QPainter>
//#include <vector>
//#include <complex>
//#include "DataStructures.h"

//class FFTWidget : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit FFTWidget(QWidget *parent = nullptr);
    
//    void updateData(const RawADCFrameTest& adcFrame);
//    void setFrequencyRange(float minFreq, float maxFreq);
    
//    // New methods for radar range calculation
//    void setRadarParameters(float sampleRate, float sweepTime, float bandwidth, float centerFreq);
//    void setMaxRange(float maxRange);
    
//    // Target information methods
//    void updateTargets(const TargetTrackData& targets);

//protected:
//    void paintEvent(QPaintEvent *event) override;
//    void resizeEvent(QResizeEvent *event) override;

//private:
//    void performFFT(const std::vector<float>& input);
//    void drawBackground(QPainter& painter);
//    void drawGrid(QPainter& painter);
//    void drawSpectrum(QPainter& painter);
//    void drawTargetIndicators(QPainter& painter);
//    void drawLabels(QPainter& painter);
//    void setMinRange(float minRange);
//    void drawPeaks(QPainter& painter, const QVector<QPointF>& spectrumPoints);
//    void applyWindow(std::vector<std::complex<float>>& data, size_t validSamples);
    
//    // Range calculation helper methods
//    float frequencyToRange(float frequency) const;
//    float sampleIndexToRange(int sampleIndex) const;
    
//    // Simple FFT implementation
//    void fft(std::vector<std::complex<float>>& data);
//    void bit_reverse(std::vector<std::complex<float>>& data);
    
//    std::vector<float> m_magnitudeSpectrum;
//    std::vector<float> m_frequencyAxis;
//    std::vector<float> m_rangeAxis;  // New: range axis in meters
    
//    RawADCFrameTest m_currentFrame;
//    TargetTrackData m_currentTargets;
//    float m_minFrequency;
//    float m_maxFrequency;
//    float m_maxMagnitude;

    
//    // Radar parameters for range calculation
//    float m_sampleRate;     // ADC sampling rate (Hz)
//    float m_sweepTime;      // Chirp sweep time (seconds)
//    float m_bandwidth;      // Chirp bandwidth (Hz)
//    float m_centerFreq;     // Radar center frequency (Hz)
//    float m_maxRange;       // Maximum display range (meters)
//    float m_minRange;
    
//    QRect m_plotRect;
//    int m_margin;
    
//    // Visual settings
//    static constexpr int GRID_LINES_X = 5;
//    static constexpr int GRID_LINES_Y = 5;
//    static constexpr float MIN_MAG_DB = -20.0f;
//    static constexpr float MAX_MAG_DB = 30.0f;
    
//    // Physical constants
//    static constexpr float SPEED_OF_LIGHT = 299792458.0f; // m/s
//};


// FFTWidget.h - Updated header for I/Q complex data support

#ifndef FFTWIDGET_H
#define FFTWIDGET_H
#pragma once
#include <QWidget>
#include <QPainter>
#include <QVector>
#include <vector>
#include <complex>
#include "DataStructures.h"

//// Forward declarations (make sure these match your main structures)
//struct ComplexSample {
//    float I;  // In-phase component
//    float Q;  // Quadrature component

//    float magnitude() const {
//        return std::sqrt(I * I + Q * Q);
//    }

//    float phase() const {
//        return std::atan2(Q, I);
//    }
//};

//struct RawADCFrameTest {
//    uint32_t msgId;
//    uint32_t num_samples_per_chirp;
//    std::vector<ComplexSample> complex_data;
//    std::vector<float> magnitude_data;

//    void computeMagnitudes() {
//        magnitude_data.clear();
//        magnitude_data.reserve(complex_data.size());
//        for (const auto& sample : complex_data) {
//            magnitude_data.push_back(sample.magnitude());
//        }
//    }
//};

//struct TargetTrack {
//    uint32_t target_id;
//    float level;
//    float radius;
//    float azimuth;
//    float elevation;
//    float radial_speed;
//    float azimuth_speed;
//    float elevation_speed;
//};

//struct TargetTrackData {
//    uint32_t numTracks;
//    std::vector<TargetTrack> targets;

//    void resize(uint32_t size) {
//        numTracks = size;
//        targets.resize(size);
//    }
//};

class FFTWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FFTWidget(QWidget *parent = nullptr);

    void updateData(const RawADCFrameTest& adcFrame);
    void setFrequencyRange(float minFreq, float maxFreq);
    void updateTargets(const TargetTrackData& targets);
    void setRadarParameters(float sampleRate, float sweepTime, float bandwidth, float centerFreq);
    void setMaxRange(float maxRange);
    void setMinRange(float minRange);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // Constants for Infineon-style display
    static constexpr float SPEED_OF_LIGHT = 299792458.0f; // m/s
    static const int GRID_LINES_X = 10;
    static const int GRID_LINES_Y = 8;

    // Core FFT and data processing
    void performFFT(const std::vector<float>& input);  // Legacy function
    void performFFTFromComplexData(const std::vector<ComplexSample>& complexInput);
    void fft(std::vector<std::complex<float>>& data);
    void bit_reverse(std::vector<std::complex<float>>& data);
    void applyWindow(std::vector<std::complex<float>>& data, size_t validSamples);

    // Enhanced processing functions
    float applyWindowWithCorrection(std::vector<std::complex<float>>& data, size_t validSamples);
    void addSimpleNoiseFloor();
    void addDetectedTargetPeaks();
    void addSyntheticTarget(float range, float magnitude_db, float rcs_m2);

    // NEW: Radar-specific functions for Infineon-style display
    void addSyntheticRadarPeaks();
    void drawPeakMarkers(QPainter& painter, const QVector<QPointF>& spectrumPoints);

    // Utility functions
    float frequencyToRange(float frequency) const;
    float sampleIndexToRange(int sampleIndex) const;

    // Drawing functions
    void drawBackground(QPainter& painter);
    void drawGrid(QPainter& painter);
    void drawSpectrum(QPainter& painter);
    void drawTargetIndicators(QPainter& painter);
    void drawLabels(QPainter& painter);

    // Data storage
    RawADCFrameTest m_currentFrame;
    TargetTrackData m_currentTargets;

    std::vector<float> m_magnitudeSpectrum;
    std::vector<float> m_frequencyAxis;
    std::vector<float> m_rangeAxis;

    // Display parameters
    float m_maxMagnitude;
    float m_minFrequency = 0.0f;
    float m_maxFrequency = 50000.0f;

    // Radar parameters - updated to match Infineon defaults
    float m_sampleRate;       // ADC sampling rate
    float m_sweepTime;        // Chirp sweep time (1.5ms for Infineon)
    float m_bandwidth;        // Chirp bandwidth (100MHz for Infineon)
    float m_centerFreq;       // RF center frequency (24.125GHz)
    float m_maxRange;
    float m_minRange;

    // UI layout
    QRect m_plotRect;
    int m_margin;
};

#endif // FFTWIDGET_H
