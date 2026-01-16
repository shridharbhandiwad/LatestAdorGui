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

// DSP command types
typedef enum {
    DSP_CMD_SET = 1,      // Set DSP parameters
    DSP_CMD_GET = 2,      // Get DSP parameters
    DSP_CMD_RESPONSE = 3  // Response with DSP parameters
} dsp_command_type_t;

// DSP settings payload for UDP communication
typedef struct {
    uint8_t range_mvg_avg_length;  // Moving average length
    uint16_t min_range_cm;         // Minimum range (cm)
    uint16_t max_range_cm;         // Maximum range (cm)
    uint16_t min_speed_kmh;        // Minimum speed (km/h)
    uint16_t max_speed_kmh;        // Maximum speed (km/h)
    uint16_t min_angle_degree;     // Minimum angle (degrees)
    uint16_t max_angle_degree;     // Maximum angle (degrees)
    uint16_t range_threshold;      // Range FFT threshold
    uint16_t speed_threshold;      // Doppler FFT threshold
    uint8_t enable_tracking;       // Enable tracking
    uint8_t num_of_tracks;         // Number of active tracks
    uint8_t median_filter_length;  // Depth of median filter
    uint8_t enable_mti_filter;     // Enable MTI filter
    uint16_t mti_filter_length;    // MTI filter length
} DSP_Settings_t;

// DSP message structure for UDP communication
typedef struct {
    uint32_t msg_type;      // DSP_CMD_SET, DSP_CMD_GET, DSP_CMD_RESPONSE
    DSP_Settings_t settings;
} dsp_message_t;

#pragma pack(pop)
