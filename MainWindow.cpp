#include "MainWindow.h"
#include <QApplication>
#include <QNetworkDatagram>
#include <QHeaderView>
#include <QMessageBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QStatusBar>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QTimer>
#include <QUdpSocket>
#include <QHostAddress>
#include <QRegularExpression>
#include <QDebug>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ppiWidget(nullptr)
    , m_fftWidget(nullptr)
    , m_trackTable(nullptr)
    , m_udpSocket(nullptr)
    , m_updateTimer(nullptr)
    , m_simulationEnabled(false)
    , m_randomEngine(std::random_device{}())
    , m_rangeDist(100.0f, 500.0f)  // 100-500 m
    , m_azimuthDist(-90.0f, 90.0f)    // -90 to +90 degrees
    , m_speedDist(-50.0f, 50.0f)      // -50 to +50 m/s
    , m_levelDist(10.0f, 100.0f)      // 10-100 dB
    , m_numTargetsDist(3, 8)          // 3-8 targets
    , m_frameCount(0)
    , m_targetCount(0)
{
    setupUI();
    setupNetworking();
    setupTimer();
}

MainWindow::~MainWindow()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("Radar Data Visualization - Qt C++");
    setMinimumSize(1200, 800);

    // Apply blue theme stylesheet
    QString blueTheme = R"(
        QMainWindow {
            background-color: #1e3a5f;
            color: #ffffff;
        }

        QWidget {
            background-color: #1e3a5f;
            color: #ffffff;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        QGroupBox {
            font-weight: bold;
            font-size: 12px;
            color: #87ceeb;
            border: 2px solid #4682b4;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            background-color: #2c5282;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 10px 0 10px;
            background-color: #2c5282;
            color: #87ceeb;
        }

        QLabel {
            color: #b3d9ff;
            font-weight: normal;
            background-color: transparent;
        }

        QLineEdit {
            background-color: #ffffff;
            border: 2px solid #4682b4;
            border-radius: 4px;
            padding: 4px;
            color: #000000;
            font-weight: bold;
            selection-background-color: #87ceeb;
        }

        QLineEdit:focus {
            border: 2px solid #87ceeb;
            background-color: #f0f8ff;
        }

        QSpinBox {
            background-color: #ffffff;
            border: 2px solid #4682b4;
            border-radius: 4px;
            padding: 4px;
            color: #000000;
            font-weight: bold;
            selection-background-color: #87ceeb;
        }

        QSpinBox:focus {
            border: 2px solid #87ceeb;
            background-color: #f0f8ff;
        }

        QPushButton {
            background-color: #4682b4;
            border: 2px solid #5a9fd4;
            border-radius: 6px;
            color: #ffffff;
            font-weight: bold;
            padding: 6px 12px;
            min-height: 20px;
        }

        QPushButton:hover {
            background-color: #5a9fd4;
            border: 2px solid #87ceeb;
        }

        QPushButton:pressed {
            background-color: #2e5984;
            border: 2px solid #4682b4;
        }

        QPushButton:checked {
            background-color: #1e3a5f;
            border: 2px solid #87ceeb;
            color: #87ceeb;
        }

        QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #f0f8ff;
            color: #000000;
            gridline-color: #4682b4;
            border: 2px solid #4682b4;
            border-radius: 4px;
            selection-background-color: #87ceeb;
        }

        QTableWidget::item {
            padding: 4px;
            border-bottom: 1px solid #b3d9ff;
        }

        QTableWidget::item:selected {
            background-color: #4682b4;
            color: #ffffff;
        }

        QHeaderView::section {
            background-color: #4682b4;
            color: #ffffff;
            font-weight: bold;
            padding: 6px;
            border: 1px solid #5a9fd4;
        }

        QStatusBar {
            background-color: #2c5282;
            color: #b3d9ff;
            border-top: 1px solid #4682b4;
        }

        QSplitter::handle {
            background-color: #4682b4;
            border: 1px solid #5a9fd4;
        }

        QSplitter::handle:horizontal {
            width: 4px;
        }

        QSplitter::handle:vertical {
            height: 4px;
        }
    )";

    this->setStyleSheet(blueTheme);

    // Create central widget with splitter layout
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Main horizontal splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);

    // Left side: PPI Display
    QGroupBox* ppiGroup = new QGroupBox("PPI Display (Target Tracks)");
    QVBoxLayout* ppiLayout = new QVBoxLayout(ppiGroup);

    m_ppiWidget = new PPIWidget();
    ppiLayout->addWidget(m_ppiWidget);

    // PPI Settings Panel
    QGroupBox* settingsGroup = new QGroupBox("Radar Settings");
    QGridLayout* settingsLayout = new QGridLayout(settingsGroup);

    // Max Range setting
    settingsLayout->addWidget(new QLabel("Max Range (m):"), 0, 0);
    m_rangeSpinBox = new QSpinBox();
    m_rangeSpinBox->setRange(1, 50);
    m_rangeSpinBox->setValue(50);
    m_rangeSpinBox->setSuffix(" m");
    m_rangeSpinBox->setMaximumWidth(80);
    connect(m_rangeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onRangeChanged);
    settingsLayout->addWidget(m_rangeSpinBox, 0, 1);
    settingsLayout->addWidget(new QLabel("(1-300)"), 0, 2);

    // Min Range setting
    settingsLayout->addWidget(new QLabel("Min Range (m):"), 1, 0);
    m_minRangeLineEdit = new QLineEdit();
    m_minRangeLineEdit->setText("1");
    m_minRangeLineEdit->setValidator(new QDoubleValidator(0.1, 299.0, 2, this));
    m_minRangeLineEdit->setMaximumWidth(80);
    m_minRangeLineEdit->setMinimumWidth(60);
    connect(m_minRangeLineEdit, &QLineEdit::textChanged, this, &MainWindow::onMinRangeChanged);
    settingsLayout->addWidget(m_minRangeLineEdit, 1, 1);
    settingsLayout->addWidget(new QLabel("(0.1-299)"), 1, 2);

    // Chirp setting
    settingsLayout->addWidget(new QLabel("Chirp (MHz):"), 2, 0);
    m_chirpLineEdit = new QLineEdit();
    m_chirpLineEdit->setText("100");
    m_chirpLineEdit->setValidator(new QDoubleValidator(1.0, 1000.0, 2, this));
    m_chirpLineEdit->setMaximumWidth(80);
    m_chirpLineEdit->setMinimumWidth(60);
    connect(m_chirpLineEdit, &QLineEdit::textChanged, this, &MainWindow::onChirpChanged);
    settingsLayout->addWidget(m_chirpLineEdit, 2, 1);
    settingsLayout->addWidget(new QLabel("(1-1000)"), 2, 2);

    // Bandwidth setting
    settingsLayout->addWidget(new QLabel("Bandwidth (MHz):"), 3, 0);
    m_bandwidthLineEdit = new QLineEdit();
    m_bandwidthLineEdit->setText("50");
    m_bandwidthLineEdit->setValidator(new QDoubleValidator(1.0, 500.0, 2, this));
    m_bandwidthLineEdit->setMaximumWidth(80);
    m_bandwidthLineEdit->setMinimumWidth(60);
    connect(m_bandwidthLineEdit, &QLineEdit::textChanged, this, &MainWindow::onBandwidthChanged);
    settingsLayout->addWidget(m_bandwidthLineEdit, 3, 1);
    settingsLayout->addWidget(new QLabel("(1-500)"), 3, 2);

    // Min Speed setting
    settingsLayout->addWidget(new QLabel("Min Speed (m/s):"), 4, 0);
    m_minSpeedLineEdit = new QLineEdit();
    m_minSpeedLineEdit->setText("-50");
    m_minSpeedLineEdit->setValidator(new QDoubleValidator(-100.0, 99.0, 2, this));
    m_minSpeedLineEdit->setMaximumWidth(80);
    m_minSpeedLineEdit->setMinimumWidth(60);
    connect(m_minSpeedLineEdit, &QLineEdit::textChanged, this, &MainWindow::onMinSpeedChanged);
    settingsLayout->addWidget(m_minSpeedLineEdit, 4, 1);
    settingsLayout->addWidget(new QLabel("(-100 to 99)"), 4, 2);

    // Max Speed setting
    settingsLayout->addWidget(new QLabel("Max Speed (m/s):"), 5, 0);
    m_maxSpeedLineEdit = new QLineEdit();
    m_maxSpeedLineEdit->setText("50");
    m_maxSpeedLineEdit->setValidator(new QDoubleValidator(-99.0, 100.0, 2, this));
    m_maxSpeedLineEdit->setMaximumWidth(80);
    m_maxSpeedLineEdit->setMinimumWidth(60);
    connect(m_maxSpeedLineEdit, &QLineEdit::textChanged, this, &MainWindow::onMaxSpeedChanged);
    settingsLayout->addWidget(m_maxSpeedLineEdit, 5, 1);
    settingsLayout->addWidget(new QLabel("(-99 to 100)"), 5, 2);

    // Chirps per Frame setting
    settingsLayout->addWidget(new QLabel("Chirps per Frame:"), 6, 0);
    m_chirpsPerFrameLineEdit = new QLineEdit();
    m_chirpsPerFrameLineEdit->setText("4");
    m_chirpsPerFrameLineEdit->setValidator(new QIntValidator(1, 128, this));
    m_chirpsPerFrameLineEdit->setMaximumWidth(80);
    m_chirpsPerFrameLineEdit->setMinimumWidth(60);
    connect(m_chirpsPerFrameLineEdit, &QLineEdit::textChanged, this, &MainWindow::onChirpsPerFrameChanged);
    settingsLayout->addWidget(m_chirpsPerFrameLineEdit, 6, 1);
    settingsLayout->addWidget(new QLabel("(1-128)"), 6, 2);

    // Samples per Chirp setting
    settingsLayout->addWidget(new QLabel("Samples per Chirp:"), 7, 0);
    m_samplesPerChirpLineEdit = new QLineEdit();
    m_samplesPerChirpLineEdit->setText("512");
    m_samplesPerChirpLineEdit->setValidator(new QIntValidator(32, 4096, this));
    m_samplesPerChirpLineEdit->setMaximumWidth(80);
    m_samplesPerChirpLineEdit->setMinimumWidth(60);
    connect(m_samplesPerChirpLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSamplesPerChirpChanged);
    settingsLayout->addWidget(m_samplesPerChirpLineEdit, 7, 1);
    settingsLayout->addWidget(new QLabel("(32-4096)"), 7, 2);

    // Apply/Reset buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_applyButton = new QPushButton("Apply Settings");
    m_resetButton = new QPushButton("Reset to Default");
    m_applyButton->setMaximumWidth(120);
    m_resetButton->setMaximumWidth(120);
    connect(m_applyButton, &QPushButton::clicked, this, &MainWindow::onApplySettings);
    connect(m_resetButton, &QPushButton::clicked, this, &MainWindow::onResetSettings);
    buttonLayout->addWidget(m_applyButton);
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addStretch();

    settingsLayout->addLayout(buttonLayout, 8, 0, 1, 3);

    // Set column widths for compact layout
    settingsLayout->setColumnMinimumWidth(0, 140); // Label column
    settingsLayout->setColumnMinimumWidth(1, 80);  // Input column
    settingsLayout->setColumnMinimumWidth(2, 100); // Range column

    // Set compact spacing
    settingsLayout->setSpacing(5);
    settingsLayout->setContentsMargins(10, 10, 10, 10);

    ppiLayout->addWidget(settingsGroup);
    m_mainSplitter->addWidget(ppiGroup);

    // Right side splitter (vertical)
    m_rightSplitter = new QSplitter(Qt::Vertical);

    // FFT Display
    QGroupBox* fftGroup = new QGroupBox("FFT Spectrum (Raw ADC Data)");
    QVBoxLayout* fftLayout = new QVBoxLayout(fftGroup);

    m_fftWidget = new FFTWidget();

    // Set up radar parameters for range calculation
    float sampleRate = 100000.0f;     // 100 kHz ADC sampling rate
    float sweepTime = 0.001f;         // 1 ms chirp sweep time
    float bandwidth = 50000000.0f;    // 50 MHz chirp bandwidth
    float centerFreq = 24000000000.0f; // 24 GHz radar frequency
    m_fftWidget->setRadarParameters(sampleRate, sweepTime, bandwidth, centerFreq);
    m_fftWidget->setMaxRange(50.0f); // Initial max range

    fftLayout->addWidget(m_fftWidget);
    m_rightSplitter->addWidget(fftGroup);

    // Track Table
    QGroupBox* tableGroup = new QGroupBox("Target Track Table");
    QVBoxLayout* tableLayout = new QVBoxLayout(tableGroup);

    m_trackTable = new QTableWidget();
    m_trackTable->setColumnCount(4);
    QStringList headers;
    headers << "ID" << "Range (m)" << "Azimuth (°)" << "Radial Speed (m/s)";
    m_trackTable->setHorizontalHeaderLabels(headers);
    m_trackTable->horizontalHeader()->setStretchLastSection(true);
    m_trackTable->setAlternatingRowColors(true);
    m_trackTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    tableLayout->addWidget(m_trackTable);
    m_rightSplitter->addWidget(tableGroup);

    m_mainSplitter->addWidget(m_rightSplitter);

    // Set splitter proportions
    m_mainSplitter->setSizes({600, 600});
    m_rightSplitter->setSizes({400, 400});

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Control panel
    QHBoxLayout* controlLayout = new QHBoxLayout();

    m_simulateButton = new QPushButton("Enable Simulation");
    m_simulateButton->setCheckable(true);
    m_simulateButton->setChecked(true);
    connect(m_simulateButton, &QPushButton::toggled,
            this, &MainWindow::onSimulateDataToggled);
    controlLayout->addWidget(m_simulateButton);

    controlLayout->addStretch();

    m_frameCountLabel = new QLabel("Frames: 0");
    controlLayout->addWidget(m_frameCountLabel);

    m_statusLabel = new QLabel("Status: Simulation Active");
    controlLayout->addWidget(m_statusLabel);

    mainLayout->addWidget(m_mainSplitter);

    // Status bar
    statusBar()->showMessage("Radar Visualization Ready - Listening on UDP port 5000");
}

void MainWindow::setupNetworking()
{
    m_udpSocket = new QUdpSocket(this);

    if (!m_udpSocket->bind(QHostAddress::Any, UDP_PORT)) {
        QMessageBox::warning(this, "Network Error",
                           QString("Failed to bind to UDP port %1. "
                                  "Real data reception disabled.").arg(UDP_PORT));
        m_statusLabel->setText("Status: Network Error - Simulation Only");
    } else {
        connect(m_udpSocket, &QUdpSocket::readyRead,
                this, &MainWindow::readPendingDatagrams);
        m_statusLabel->setText("Status: UDP Listening");
    }
}

void MainWindow::setupTimer()
{
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout,
            this, &MainWindow::updateDisplay);
    m_updateTimer->start(UPDATE_INTERVAL_MS);
}

void MainWindow::updateDisplay()
{
    if (m_simulationEnabled) {
        generateSimulatedTargetData();
        generateSimulatedADCData();
    }

    // Update widgets
    m_ppiWidget->updateTargets(m_currentTargets);
    m_fftWidget->updateData(m_currentADCFrame);
    m_fftWidget->updateTargets(m_currentTargets);
    updateTrackTable();

    // Update statistics
    m_frameCount++;
    m_frameCountLabel->setText(QString("Frames: %1").arg(m_frameCount));

    if (m_simulationEnabled) {
        m_statusLabel->setText(QString("Status: Simulation Active - %1 targets")
                              .arg(m_currentTargets.numTracks));
    }
}

void MainWindow::readPendingDatagrams()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        qDebug() << "Received datagram "<< datagram.size();
        QString msg = QString::fromUtf8(datagram);
        if (msg.contains("NumTargets:")) {
            parseTrackMessage(msg);  // Track data if present
        } else {
            qDebug() << "No track data";
        }
        if (msg.contains("ADC:")) {
            parseADCMessage(msg);  // Raw FFT data
            qDebug()<<"ADC size "<<sizeof(msg);
        }
    }
}

void MainWindow::parseTrackMessage(const QString& message)
{
    qDebug() << "Parsing track message";
    QStringList tokens = message.split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
    m_currentTargets.targets.clear();
    m_currentTargets.numTracks = 0;

    TargetTrack target;
    int numTargets = 0;
    int parsedTargets = 0;

    for (int i = 0; i < tokens.size(); ++i) {
        const QString& token = tokens[i];

        if (token == "NumTargets:" && i + 1 < tokens.size()) {
            numTargets = tokens[++i].toInt();
        } else if (token == "TgtId:" && i + 1 < tokens.size()) {
            if (parsedTargets > 0) {
                m_currentTargets.targets.push_back(target);
                target = TargetTrack(); // Reset
            }
            target.target_id = tokens[++i].toInt();
            ++parsedTargets;
        } else if (token == "Level:" && i + 1 < tokens.size()) {
            target.level = tokens[++i].toFloat();
        } else if (token == "Range:" && i + 1 < tokens.size()) {
            target.radius = tokens[++i].toFloat() / 100.0; // Convert from cm to m
        } else if (token == "Azimuth:" && i + 1 < tokens.size()) {
            target.azimuth = tokens[++i].toFloat();
        } else if (token == "Elevation:" && i + 1 < tokens.size()) {
            target.elevation = tokens[++i].toFloat();
        } else if (token == "RadialSpeed:" && i + 1 < tokens.size()) {
            target.radial_speed = tokens[++i].toFloat();
        } else if (token == "AzimuthSpeed:" && i + 1 < tokens.size()) {
            target.azimuth_speed = tokens[++i].toFloat();
        } else if (token == "ElevationSpeed:" && i + 1 < tokens.size()) {
            target.elevation_speed = tokens[++i].toFloat();
        }
    }

    // Append final target if one exists
    if (parsedTargets > 0) {
        m_currentTargets.targets.push_back(target);
    }

    m_currentTargets.numTracks = m_currentTargets.targets.size();
}

// Updated parseADCMessage function
void MainWindow::parseADCMessage(const QString& message)
{
    QStringList tokens = message.split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
    qDebug() << "Parsing ADC message";
    RawADCFrameTest frame;
    frame.complex_data.clear();

    std::vector<float> raw_samples;  // Temporary storage for all samples
    qDebug()<<"tokens.size() "<<tokens.size();
    for (int i = 0; i < tokens.size(); ++i) {
        const QString& token = tokens[i];
        qDebug()<<"i "<<i;
        if (token == "MsgId:" && i + 1 < tokens.size()) {
            frame.msgId = tokens[++i].toUInt();
        } else if (token == "NumSamples:" && i + 1 < tokens.size()) {
            // This should be total samples (64), but we want complex samples (32)
            uint32_t totalSamples = tokens[++i].toUInt();
            frame.num_samples_per_chirp = totalSamples / 2;  // Convert to complex samples
        } else if (token == "ADC:" && i + 1 < tokens.size()) {
            raw_samples.push_back(tokens[++i].toFloat());
        }
    }

    // Convert raw I/Q samples to complex samples
    // Assuming format: I1, Q1, I2, Q2, I3, Q3, ...
    frame.complex_data.reserve(raw_samples.size() / 2);

    qDebug()<<"raw_samples.size() "<<raw_samples.size();

    for (size_t i = 0; i < raw_samples.size(); i += 2) {

        if (i + 1 < raw_samples.size()) {
            ComplexSample complex_sample;
            complex_sample.I = raw_samples[i];     // In-phase
            complex_sample.Q = raw_samples[i + 32]; // Quadrature
            frame.complex_data.push_back(complex_sample);
            qDebug()<<i<<" "<<complex_sample.I;
            qDebug()<<i+1<<" "<<complex_sample.Q;
        }
    }

    // Compute magnitudes
    frame.computeMagnitudes();

    // Assign to current frame
    m_currentADCFrame = frame;

    qDebug() << "Parsed" << frame.complex_data.size() << "complex samples";
    qDebug() << "First sample: I=" << frame.complex_data[0].I
             << "Q=" << frame.complex_data[0].Q
             << "Mag=" << frame.complex_data[0].magnitude();
}

void MainWindow::onSimulateDataToggled()
{
    m_simulationEnabled = m_simulateButton->isChecked();

    if (m_simulationEnabled) {
        m_simulateButton->setText("Disable Simulation");
        m_statusLabel->setText("Status: Simulation Active");
    } else {
        m_simulateButton->setText("Enable Simulation");
        m_statusLabel->setText("Status: Simulation Disabled");
    }
}

void MainWindow::onRangeChanged(int range)
{
    float rangeMeters = range;
    float minRange = m_minRangeLineEdit->text().toFloat();

    if (rangeMeters > minRange) {
        m_ppiWidget->setMaxRange(rangeMeters);
        m_fftWidget->setMaxRange(rangeMeters);
    } else {
        // Reset to valid value if max range <= min range
        m_rangeSpinBox->setValue(static_cast<int>(minRange + 1));
    }
}

void MainWindow::onMinRangeChanged(const QString& text)
{
    bool ok;
    float minRange = text.toFloat(&ok);
    if (ok && minRange >= 0.1f && minRange <= 299.0f) {
        // Validate against max range
        float maxRange = m_rangeSpinBox->value();
        if (minRange < maxRange) {
            qDebug() << "Min Range changed to:" << minRange << "m";
            // Apply min range changes to your radar system
            //m_ppiWidget->setMinRange(minRange);
            //m_fftWidget->setMinRange(minRange);
        }
    }
}

void MainWindow::onChirpChanged(const QString& text)
{
    bool ok;
    float chirp = text.toFloat(&ok);
    if (ok && chirp >= 1.0f && chirp <= 1000.0f) {
        qDebug() << "Chirp changed to:" << chirp << "MHz";
        // Apply chirp changes to your radar system
    }
}

void MainWindow::onBandwidthChanged(const QString& text)
{
    bool ok;
    float bandwidth = text.toFloat(&ok);
    if (ok && bandwidth >= 1.0f && bandwidth <= 500.0f) {
        qDebug() << "Bandwidth changed to:" << bandwidth << "MHz";
        // Apply bandwidth changes to your FFT widget
        m_fftWidget->setRadarParameters(100000.0f, 0.001f, bandwidth * 1000000.0f, 24000000000.0f);
    }
}

void MainWindow::onMinSpeedChanged(const QString& text)
{
    bool ok;
    float minSpeed = text.toFloat(&ok);
    if (ok && minSpeed >= -100.0f && minSpeed <= 99.0f) {
        // Validate against max speed
        float maxSpeed = m_maxSpeedLineEdit->text().toFloat();
        if (minSpeed < maxSpeed) {
            qDebug() << "Min Speed changed to:" << minSpeed << "m/s";
            // Update speed distribution for simulation
            m_speedDist = std::uniform_real_distribution<float>(minSpeed, maxSpeed);
        }
    }
}

void MainWindow::onMaxSpeedChanged(const QString& text)
{
    bool ok;
    float maxSpeed = text.toFloat(&ok);
    if (ok && maxSpeed >= -99.0f && maxSpeed <= 100.0f) {
        // Validate against min speed
        float minSpeed = m_minSpeedLineEdit->text().toFloat();
        if (maxSpeed > minSpeed) {
            qDebug() << "Max Speed changed to:" << maxSpeed << "m/s";
            // Update speed distribution for simulation
            m_speedDist = std::uniform_real_distribution<float>(minSpeed, maxSpeed);
        }
    }
}

void MainWindow::onChirpsPerFrameChanged(const QString& text)
{
    bool ok;
    int chirpsPerFrame = text.toInt(&ok);
    if (ok && chirpsPerFrame >= 1 && chirpsPerFrame <= 128) {
        qDebug() << "Chirps per Frame changed to:" << chirpsPerFrame;
        // Apply chirps per frame changes to your radar system
    }
}

void MainWindow::onSamplesPerChirpChanged(const QString& text)
{
    bool ok;
    int samplesPerChirp = text.toInt(&ok);
    if (ok && samplesPerChirp >= 32 && samplesPerChirp <= 4096) {
        // Validate that it's a power of 2 for FFT efficiency
        if ((samplesPerChirp & (samplesPerChirp - 1)) != 0) {
            qDebug() << "Warning: Samples per chirp should be a power of 2 for optimal FFT performance";
        }
        qDebug() << "Samples per Chirp changed to:" << samplesPerChirp;
        // Apply samples per chirp changes to your radar system
    }
}

void MainWindow::onApplySettings()
{
    // Apply all settings
    QString settingsSummary = QString(
        "Applied Settings:\n"
        "Max Range: %1 m\n"
        "Min Range: %2 m\n"
        "Chirp: %3 MHz\n"
        "Bandwidth: %4 MHz\n"
        "Speed Range: %5 to %6 m/s\n"
        "Chirps per Frame: %7\n"
        "Samples per Chirp: %8"
    ).arg(m_rangeSpinBox->value())
     .arg(m_minRangeLineEdit->text())
     .arg(m_chirpLineEdit->text())
     .arg(m_bandwidthLineEdit->text())
     .arg(m_minSpeedLineEdit->text())
     .arg(m_maxSpeedLineEdit->text())
     .arg(m_chirpsPerFrameLineEdit->text())
     .arg(m_samplesPerChirpLineEdit->text());

    QMessageBox::information(this, "Settings Applied", settingsSummary);
}

void MainWindow::onResetSettings()
{
    // Reset to default values
    m_rangeSpinBox->setValue(200);
    m_minRangeLineEdit->setText("1");
    m_chirpLineEdit->setText("100");
    m_bandwidthLineEdit->setText("50");
    m_minSpeedLineEdit->setText("-50");
    m_maxSpeedLineEdit->setText("50");
    m_chirpsPerFrameLineEdit->setText("4");
    m_samplesPerChirpLineEdit->setText("512");

    // Reset speed distribution
    m_speedDist = std::uniform_real_distribution<float>(-50.0f, 50.0f);

    QMessageBox::information(this, "Settings Reset",
                           "All settings have been reset to default values.");
}

void MainWindow::updateTrackTable()
{
    m_trackTable->setRowCount(m_currentTargets.numTracks);
    for (uint32_t i = 0; i < m_currentTargets.numTracks; ++i) {
        const TargetTrack& target = m_currentTargets.targets[i];
        qDebug() << target.target_id << target.radius << target.azimuth << target.radial_speed;
        m_trackTable->setItem(i, 0, new QTableWidgetItem(QString::number(target.target_id)));
        m_trackTable->setItem(i, 1, new QTableWidgetItem(QString::number(target.radius, 'f', 2))); // Changed from 0 to 2 decimal places
        m_trackTable->setItem(i, 2, new QTableWidgetItem(QString::number(target.azimuth, 'f', 1)));
        m_trackTable->setItem(i, 3, new QTableWidgetItem(QString::number(target.radial_speed, 'f', 1)));
    }

    m_trackTable->resizeColumnsToContents();
}

void MainWindow::generateSimulatedTargetData()
{
    // Generate random number of targets
    uint32_t numTargets = m_numTargetsDist(m_randomEngine);
    m_currentTargets.resize(numTargets);

    for (uint32_t i = 0; i < numTargets; ++i) {
        TargetTrack& target = m_currentTargets.targets[i];

        target.target_id = i + 1;
        target.level = m_levelDist(m_randomEngine);
        target.radius = m_rangeDist(m_randomEngine);
        target.azimuth = m_azimuthDist(m_randomEngine);
        target.elevation = std::uniform_real_distribution<float>(-30.0f, 30.0f)(m_randomEngine);
        target.radial_speed = m_speedDist(m_randomEngine);
        target.azimuth_speed = std::uniform_real_distribution<float>(-5.0f, 5.0f)(m_randomEngine);
        target.elevation_speed = std::uniform_real_distribution<float>(-2.0f, 2.0f)(m_randomEngine);
    }

    m_targetCount += numTargets;
}

// Updated simulation to generate I/Q data
void MainWindow::generateSimulatedADCData()
{
    // Get current samples per chirp from UI (this should be complex samples, e.g., 32)
    bool ok;
    uint32_t numComplexSamples = m_samplesPerChirpLineEdit->text().toUInt(&ok);
    if (!ok || numComplexSamples < 16) {
        numComplexSamples = 32; // Default fallback for 32 complex samples (64 total)
    }

    m_currentADCFrame.complex_data.resize(numComplexSamples);
    m_currentADCFrame.num_samples_per_chirp = numComplexSamples;

    // Generate I/Q signal components
    float sampleRate = 100000.0f; // 100 kHz
    float t_step = 1.0f / sampleRate;

    // Add some dominant frequency components
    float freq1 = 5000.0f;  // 5 kHz
    float freq2 = 15000.0f; // 15 kHz
    float freq3 = 25000.0f; // 25 kHz

    std::uniform_real_distribution<float> noiseDist(-0.05f, 0.05f);

    for (uint32_t i = 0; i < numComplexSamples; ++i) {
        float t = i * t_step;

        // Generate I component (in-phase)
        float I_signal = 0.8f * std::cos(2.0f * M_PI * freq1 * t) +
                        0.5f * std::cos(2.0f * M_PI * freq2 * t) +
                        0.3f * std::cos(2.0f * M_PI * freq3 * t);

        // Generate Q component (quadrature - 90 degrees phase shifted)
        float Q_signal = 0.8f * std::sin(2.0f * M_PI * freq1 * t) +
                        0.5f * std::sin(2.0f * M_PI * freq2 * t) +
                        0.3f * std::sin(2.0f * M_PI * freq3 * t);

        // Add noise to both components
        I_signal += noiseDist(m_randomEngine);
        Q_signal += noiseDist(m_randomEngine);

        m_currentADCFrame.complex_data[i].I = I_signal;
        m_currentADCFrame.complex_data[i].Q = Q_signal;
    }

    // Compute magnitudes for FFT display
    m_currentADCFrame.computeMagnitudes();
}
