#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QUdpSocket>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QStatusBar>
#include <QGroupBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <random>

#include "PPIWidget.h"
#include "FFTWidget.h"
#include "DataStructures.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
private slots:
    void updateDisplay();
    void readPendingDatagrams();
    void onSimulateDataToggled();
    void onRangeChanged(int range);
    void onMinRangeChanged(const QString& text);          // NEW
    void onChirpChanged(const QString& text);
    void onBandwidthChanged(const QString& text);
    void onMinSpeedChanged(const QString& text);          // NEW
    void onMaxSpeedChanged(const QString& text);          // NEW
    void onChirpsPerFrameChanged(const QString& text);    // NEW
    void onSamplesPerChirpChanged(const QString& text);   // NEW
    void onApplySettings();
    void onResetSettings();

private:
    void setupUI();
    void setupNetworking();
    void setupTimer();
    void updateTrackTable();
    void generateSimulatedTargetData();
    void generateSimulatedADCData();
    void parseADCMessage(const QString& message);
    void parseTrackMessage(const QString& message);
    
    // UI Components
    PPIWidget* m_ppiWidget;
    FFTWidget* m_fftWidget;
    QTableWidget* m_trackTable;
    QSplitter* m_mainSplitter;
    QSplitter* m_rightSplitter;
    
    // Controls
    QSpinBox* m_rangeSpinBox;
    QPushButton* m_simulateButton;
    QLabel* m_statusLabel;
    QLabel* m_frameCountLabel;
    
    // Networking
    QUdpSocket* m_udpSocket;
    static constexpr quint16 UDP_PORT = 5000;
    
    // Timer
    QTimer* m_updateTimer;
    static constexpr int UPDATE_INTERVAL_MS = 50;
    
    // Data
    TargetTrackData m_currentTargets;
    RawADCFrameTest m_currentADCFrame;
    
    // Simulation
    bool m_simulationEnabled;
    std::mt19937 m_randomEngine;
    std::uniform_real_distribution<float> m_rangeDist;
    std::uniform_real_distribution<float> m_azimuthDist;
    std::uniform_real_distribution<float> m_speedDist;
    std::uniform_real_distribution<float> m_levelDist;
    std::uniform_int_distribution<int> m_numTargetsDist;
    
    // Statistics
    uint64_t m_frameCount;
    uint64_t m_targetCount;

    // Add these to the private members section
    QLineEdit* m_chirpLineEdit;
    QLineEdit* m_bandwidthLineEdit;
    // Add these to the private members section
    QLineEdit* m_minRangeLineEdit;
    QLineEdit* m_minSpeedLineEdit;
    QLineEdit* m_maxSpeedLineEdit;
    QLineEdit* m_chirpsPerFrameLineEdit;
    QLineEdit* m_samplesPerChirpLineEdit;

    // Remove these old ones:
    // QLineEdit* m_dopplerLineEdit;
    // QLineEdit* m_rcsLineEdit;
    QPushButton* m_applyButton;
    QPushButton* m_resetButton;
};
