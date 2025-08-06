#pragma once

#include <QWidget>
#include <QPainter>
#include <vector>
#include <complex>
#include "DataStructures.h"

class FFTWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FFTWidget(QWidget *parent = nullptr);
    
    void updateData(const RawADCFrameTest& adcFrame);
    void setFrequencyRange(float minFreq, float maxFreq);
    
    // New methods for radar range calculation
    void setRadarParameters(float sampleRate, float sweepTime, float bandwidth, float centerFreq);
    void setMaxRange(float maxRange);
    
    // Target information methods
    void updateTargets(const TargetTrackData& targets);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void performFFT(const std::vector<float>& input);
    void drawBackground(QPainter& painter);
    void drawGrid(QPainter& painter);
    void drawSpectrum(QPainter& painter);
    void drawTargetIndicators(QPainter& painter);
    void drawLabels(QPainter& painter);
    
    // Range calculation helper methods
    float frequencyToRange(float frequency) const;
    float sampleIndexToRange(int sampleIndex) const;
    
    // Simple FFT implementation
    void fft(std::vector<std::complex<float>>& data);
    void bit_reverse(std::vector<std::complex<float>>& data);
    
    std::vector<float> m_magnitudeSpectrum;
    std::vector<float> m_frequencyAxis;
    std::vector<float> m_rangeAxis;  // New: range axis in meters
    
    RawADCFrameTest m_currentFrame;
    TargetTrackData m_currentTargets;
    float m_minFrequency;
    float m_maxFrequency;
    float m_maxMagnitude;
    
    // Radar parameters for range calculation
    float m_sampleRate;     // ADC sampling rate (Hz)
    float m_sweepTime;      // Chirp sweep time (seconds)
    float m_bandwidth;      // Chirp bandwidth (Hz)
    float m_centerFreq;     // Radar center frequency (Hz)
    float m_maxRange;       // Maximum display range (meters)
    
    QRect m_plotRect;
    int m_margin;
    
    // Visual settings
    static constexpr int GRID_LINES_X = 5;
    static constexpr int GRID_LINES_Y = 5;
    static constexpr float MIN_MAG_DB = -20.0f;
    static constexpr float MAX_MAG_DB = 30.0f;
    
    // Physical constants
    static constexpr float SPEED_OF_LIGHT = 299792458.0f; // m/s
};
