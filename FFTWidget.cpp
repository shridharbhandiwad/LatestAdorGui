#include "FFTWidget.h"
#include <QPaintEvent>
#include <QResizeEvent>
#include <QFont>
#include <QFontMetrics>
#include <cmath>
#include <algorithm>

FFTWidget::FFTWidget(QWidget *parent)
    : QWidget(parent)
    , m_maxMagnitude(0.0f)
    , m_sampleRate(100000.0f)    // Default: 100 kHz
    , m_sweepTime(0.001f)        // Default: 1 ms chirp
    , m_bandwidth(50000000.0f)   // Default: 50 MHz bandwidth
    , m_centerFreq(24000000000.0f) // Default: 24 GHz
    , m_maxRange(300.0f)         // Default: 300 meters
    , m_margin(50)
{
    setMinimumSize(400, 300);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void FFTWidget::updateData(const RawADCFrameTest& adcFrame)
{
    m_currentFrame = adcFrame;
    if (!adcFrame.sample_data.empty()) {
        performFFT(adcFrame.sample_data);
    }
    update();
}

void FFTWidget::setFrequencyRange(float minFreq, float maxFreq)
{
    m_minFrequency = minFreq;
    m_maxFrequency = maxFreq;
    update();
}

void FFTWidget::updateTargets(const TargetTrackData& targets)
{
    m_currentTargets = targets;
    update();
}

void FFTWidget::setRadarParameters(float sampleRate, float sweepTime, float bandwidth, float centerFreq)
{
    m_sampleRate = sampleRate;
    m_sweepTime = sweepTime;
    m_bandwidth = bandwidth;
    m_centerFreq = centerFreq;
    
    // Recalculate range axis if we have data
    if (!m_magnitudeSpectrum.empty()) {
        performFFT(m_currentFrame.sample_data);
    }
    update();
}

void FFTWidget::setMaxRange(float maxRange)
{
    m_maxRange = maxRange;
    update();
}

float FFTWidget::frequencyToRange(float frequency) const
{
    // FMCW radar range calculation: R = (f_beat * c * T_sweep) / (2 * B)
    // where f_beat is the beat frequency, c is speed of light, T_sweep is sweep time, B is bandwidth
    return (frequency * SPEED_OF_LIGHT * m_sweepTime) / (2.0f * m_bandwidth);
}

float FFTWidget::sampleIndexToRange(int sampleIndex) const
{
    if (m_rangeAxis.empty() || sampleIndex >= static_cast<int>(m_rangeAxis.size())) {
        return 0.0f;
    }
    return m_rangeAxis[sampleIndex];
}

void FFTWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    m_plotRect = QRect(
        m_margin,
        m_margin,
        width() - 2 * m_margin,
        height() - 2 * m_margin
    );
}

void FFTWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawGrid(painter);
    drawSpectrum(painter);
    drawTargetIndicators(painter);
    drawLabels(painter);
}

void FFTWidget::performFFT(const std::vector<float>& input)
{
    if (input.empty()) return;

    size_t n = 1;
    while (n < input.size()) {
        n *= 2;
    }

    std::vector<std::complex<float>> complexData(n);
    for (size_t i = 0; i < input.size() && i < n; ++i) {
        complexData[i] = std::complex<float>(input[i], 0.0f);
    }

    fft(complexData);

    m_magnitudeSpectrum.resize(n / 2);
    m_frequencyAxis.resize(n / 2);
    m_rangeAxis.resize(n / 2);

    m_maxMagnitude = 0.0f;
    for (size_t i = 0; i < n / 2; ++i) {
        float magnitude = std::abs(complexData[i]);
        m_magnitudeSpectrum[i] = 20.0f * std::log10(magnitude + 1e-10f);
        
        // Calculate frequency for this bin
        float frequency = (static_cast<float>(i) * m_sampleRate) / static_cast<float>(n);
        m_frequencyAxis[i] = frequency;
        
        // Calculate corresponding range
        m_rangeAxis[i] = frequencyToRange(frequency);

        if (m_magnitudeSpectrum[i] > m_maxMagnitude) {
            m_maxMagnitude = m_magnitudeSpectrum[i];
        }
    }
}

void FFTWidget::fft(std::vector<std::complex<float>>& data)
{
    size_t n = data.size();
    if (n <= 1) return;

    bit_reverse(data);

    for (size_t len = 2; len <= n; len <<= 1) {
        float angle = -2.0f * M_PI / len;
        std::complex<float> wlen(std::cos(angle), std::sin(angle));

        for (size_t i = 0; i < n; i += len) {
            std::complex<float> w(1.0f, 0.0f);
            for (size_t j = 0; j < len / 2; ++j) {
                std::complex<float> u = data[i + j];
                std::complex<float> v = data[i + j + len / 2] * w;
                data[i + j] = u + v;
                data[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
}

void FFTWidget::bit_reverse(std::vector<std::complex<float>>& data)
{
    size_t n = data.size();
    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;

        if (i < j) {
            std::swap(data[i], data[j]);
        }
    }
}

void FFTWidget::drawBackground(QPainter& painter)
{
    painter.fillRect(rect(), QColor(20, 20, 20));
    painter.fillRect(m_plotRect, QColor(0, 0, 0));
    painter.setPen(QPen(QColor(100, 100, 100), 2));
    painter.drawRect(m_plotRect);
}

void FFTWidget::drawGrid(QPainter& painter)
{
    painter.setPen(QPen(QColor(60, 60, 60), 1));

    for (int i = 0; i <= GRID_LINES_X; ++i) {
        int x = m_plotRect.left() + (i * m_plotRect.width()) / GRID_LINES_X;
        painter.drawLine(x, m_plotRect.top(), x, m_plotRect.bottom());
    }

    for (int i = 0; i <= GRID_LINES_Y; ++i) {
        int y = m_plotRect.top() + (i * m_plotRect.height()) / GRID_LINES_Y;
        painter.drawLine(m_plotRect.left(), y, m_plotRect.right(), y);
    }
}

void FFTWidget::drawSpectrum(QPainter& painter)
{
    if (m_magnitudeSpectrum.empty() || m_rangeAxis.empty()) return;

    painter.setPen(QPen(QColor(0, 255, 255), 2));
    painter.setBrush(Qt::NoBrush);

    QPolygonF spectrum;

    for (size_t i = 0; i < m_magnitudeSpectrum.size(); ++i) {
        float range = m_rangeAxis[i];
        
        // Only plot points within our max range
        if (range > m_maxRange) continue;
        
        // Map range to x-coordinate
        float x = m_plotRect.left() + (range / m_maxRange) * m_plotRect.width();

        float magDb = m_magnitudeSpectrum[i];
        float y = m_plotRect.bottom() - ((magDb - MIN_MAG_DB) / (MAX_MAG_DB - MIN_MAG_DB)) * m_plotRect.height();
        y = std::max(float(m_plotRect.top()), std::min(float(m_plotRect.bottom()), y));

        spectrum << QPointF(x, y);
    }

    if (spectrum.size() > 1) {
        painter.drawPolyline(spectrum);
    }
}

void FFTWidget::drawTargetIndicators(QPainter& painter)
{
    if (m_currentTargets.numTracks == 0) return;

    // Set up different colors for different target types based on radial speed
    for (uint32_t i = 0; i < m_currentTargets.numTracks; ++i) {
        const TargetTrack& target = m_currentTargets.targets[i];
        
        // Skip targets outside the azimuth range (-90 to +90 degrees) 
        // to match the PPI display behavior
        if (target.azimuth < -90.0f || target.azimuth > 90.0f) {
            continue;
        }
        
        // Skip targets outside our range display
        if (target.radius > m_maxRange || target.radius < 0) {
            continue;
        }
        
        // Choose color based on radial speed (same as PPI)
        QColor targetColor;
        if (target.radial_speed > 5.0f) {
            targetColor = QColor(255, 100, 100); // Red for approaching targets
        } else if (target.radial_speed < -5.0f) {
            targetColor = QColor(100, 100, 255); // Blue for receding targets
        } else {
            targetColor = QColor(100, 255, 100); // Green for stationary targets
        }
        
        // Calculate x position for target range
        float x = m_plotRect.left() + (target.radius / m_maxRange) * m_plotRect.width();
        
        // Draw vertical line at target range
        painter.setPen(QPen(targetColor, 2, Qt::DashLine));
        painter.drawLine(QPointF(x, m_plotRect.top()), QPointF(x, m_plotRect.bottom()));
        
        // Draw target information at the top
        painter.setPen(QPen(targetColor, 1));
        painter.setFont(QFont("Arial", 8));
        
        QString targetInfo = QString("T%1\n%2m\n%3°")
                            .arg(target.target_id)
                            .arg(target.radius, 0, 'f', 1)
                            .arg(target.azimuth, 0, 'f', 0);
        
        // Calculate text position
        QFontMetrics fm(painter.font());
        QRect textRect = fm.boundingRect(targetInfo);
        
        // Position text above the plot area, adjust if too close to edges
        float textX = x - textRect.width() / 2;
        if (textX < m_plotRect.left()) textX = m_plotRect.left();
        if (textX + textRect.width() > m_plotRect.right()) textX = m_plotRect.right() - textRect.width();
        
        // Draw semi-transparent background for text
        QRect bgRect(textX - 2, m_plotRect.top() - textRect.height() - 5, 
                     textRect.width() + 4, textRect.height() + 2);
        painter.fillRect(bgRect, QColor(0, 0, 0, 150));
        
        // Draw text
        painter.drawText(QPointF(textX, m_plotRect.top() - 5), targetInfo);
    }
}

void FFTWidget::drawLabels(QPainter& painter)
{
    painter.setPen(QPen(Qt::white, 1));
    painter.setFont(QFont("Arial", 10));

    // X-axis labels (Range in meters)
    for (int i = 0; i <= GRID_LINES_X; ++i) {
        float range = (float(i) / GRID_LINES_X) * m_maxRange;
        int x = m_plotRect.left() + (i * m_plotRect.width()) / GRID_LINES_X;

        QString label = QString::number(range, 'f', 1) + "m";
        QFontMetrics fm(painter.font());
        QRect textRect = fm.boundingRect(label);
        painter.drawText(x - textRect.width() / 2, m_plotRect.bottom() + 15, label);
    }

    // Y-axis labels (Magnitude in dB)
    for (int i = 0; i <= GRID_LINES_Y; ++i) {
        float mag = MIN_MAG_DB + (float(i) / GRID_LINES_Y) * (MAX_MAG_DB - MIN_MAG_DB);
        int y = m_plotRect.bottom() - (i * m_plotRect.height()) / GRID_LINES_Y;

        QString magText = QString("%1dB").arg(mag, 0, 'f', 0);
        painter.drawText(m_plotRect.left() - 35, y + 5, magText);
    }

    // Axis labels
    painter.setFont(QFont("Arial", 12, QFont::Bold));

    QFontMetrics fm(painter.font());
    QString xLabel = "Range (meters)";
    QRect xLabelRect = fm.boundingRect(xLabel);
    painter.drawText(
        m_plotRect.center().x() - xLabelRect.width() / 2,
        height() - 10,
        xLabel
    );

    painter.save();
    painter.translate(15, m_plotRect.center().y());
    painter.rotate(-90);
    QString yLabel = "Magnitude (dB)";
    QRect yLabelRect = fm.boundingRect(yLabel);
    painter.drawText(-yLabelRect.width() / 2, 0, yLabel);
    painter.restore();

    // Title and info
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(QPointF(10, 25), "FFT Spectrum - Range Domain");

    painter.setFont(QFont("Arial", 10));
    QString frameInfo = QString("Frame: %1, Samples: %2, Max Range: %3m")
                       .arg(1)
                       .arg(m_currentFrame.sample_data.size())
                       .arg(m_maxRange, 0, 'f', 1);
    painter.drawText(QPointF(10, height() - 10), frameInfo);
    
    // Display radar parameters
    QString radarInfo = QString("SR: %1kHz, BW: %2MHz, Sweep: %3ms")
                       .arg(m_sampleRate / 1000.0f, 0, 'f', 1)
                       .arg(m_bandwidth / 1000000.0f, 0, 'f', 1)
                       .arg(m_sweepTime * 1000.0f, 0, 'f', 2);
    painter.drawText(QPointF(10, height() - 25), radarInfo);
}
