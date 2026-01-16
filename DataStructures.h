#pragma once

#include <cstdint>
#include <vector>
#include <math.h>
// Rx Data Format enumeration (placeholder)
#pragma pack(push, 1)
enum class Rx_Data_Format_t {
    COMPLEX_FLOAT,
    COMPLEX_INT16,
    REAL_FLOAT,
    REAL_INT16
};

// Target Track structure
struct TargetTrack {
    uint32_t target_id;
    float level;
    float radius;
    float azimuth;        // degrees, -90 to +90
    float elevation;      // degrees
    float radial_speed;   // m/s (positive = approaching, negative = receding)
    float azimuth_speed;  // deg/s
    float elevation_speed; // deg/s
};

// Target Track Data structure
struct TargetTrackData {
    uint32_t numTracks;
    std::vector<TargetTrack> targets;
    
    TargetTrackData() : numTracks(0) {}
    
    void resize(uint32_t size) {
        numTracks = size;
        targets.resize(size);
    }
};

// Raw ADC Frame structure
struct RawADCFrame {
    std::vector<float> sample_data;
    uint32_t frame_number;
    uint32_t num_chirps;
    uint8_t num_rx_antennas;
    uint32_t num_samples_per_chirp;
    uint8_t rx_mask;
    uint8_t adc_resolution;
    uint8_t interleaved_rx;
    Rx_Data_Format_t data_format;
    
    RawADCFrame() : frame_number(0), num_chirps(1), num_rx_antennas(1), 
                   num_samples_per_chirp(256), rx_mask(0x1), adc_resolution(16),
                   interleaved_rx(0), data_format(Rx_Data_Format_t::REAL_FLOAT) {}
};

// Add to MainWindow.h - new structure for complex samples
struct ComplexSample {
    float I;  // In-phase component
    float Q;  // Quadrature component

    float magnitude() const {
        return std::sqrt(I * I + Q * Q);
    }

    float phase() const {
        return std::atan2(Q, I);
    }
};


// Raw ADC Frame structure
struct RawADCFrameTest {
    uint32_t msgId;
    uint32_t num_samples_per_chirp;  // This should be number of complex samples (32)
    std::vector<ComplexSample> complex_data;  // Changed from sample_data
    std::vector<float> magnitude_data;        // Computed magnitudes

    void computeMagnitudes() {
        magnitude_data.clear();
        magnitude_data.reserve(complex_data.size());
        for (const auto& sample : complex_data) {
            magnitude_data.push_back(sample.magnitude());
        }
    }
};


// UDP Message types
enum class MessageType : uint8_t {
    TARGET_TRACK_DATA = 1,
    RAW_ADC_DATA = 2
};

// UDP Message header
struct MessageHeader {
    MessageType type;
    uint32_t data_size;
    uint64_t timestamp;
};

#pragma pack(pop)
