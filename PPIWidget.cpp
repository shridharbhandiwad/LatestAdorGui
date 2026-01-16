//#include "PPIWidget.h"
//#include <QPaintEvent>
//#include <QResizeEvent>
//#include <QFont>
//#include <QFontMetrics>
//#include <cmath>
//#include <QtMath>


//PPIWidget::PPIWidget(QWidget *parent)
//    : QWidget(parent)
//    , m_maxRange(50.0f) // 500 default
//    , m_plotRadius(0)
//{
//    setMinimumSize(400, 200);
//    setBackgroundRole(QPalette::Base);
//    setAutoFillBackground(true);
//}

//void PPIWidget::updateTargets(const TargetTrackData& trackData)
//{
//    m_currentTargets = trackData;
//    update();
//}

//void PPIWidget::setMaxRange(float range)
//{
//    if (range > 0) {
//        m_maxRange = range;
//        update();
//    }
//}

//void PPIWidget::resizeEvent(QResizeEvent *event)
//{
//    QWidget::resizeEvent(event);

//    // Calculate plot area (semi-circle)
//    int margin = 40;
//    int availableWidth = width() - 2 * margin;
//    int availableHeight = height() - 2 * margin;

//    // For semi-circle, height should be at least half of width
//    int diameter = std::min(availableWidth, availableHeight * 2);
//    m_plotRadius = diameter / 2.0f;

//    m_plotRect = QRect(
//        (width() - diameter) / 2,
//        height() - margin - m_plotRadius,
//        diameter,
//        m_plotRadius
//    );

//    m_center = QPointF(width() / 2.0f, height() - margin);
//}

//void PPIWidget::paintEvent(QPaintEvent *event)
//{
//    Q_UNUSED(event)

//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing);

//    drawBackground(painter);
//    drawRangeRings(painter);
//    drawAzimuthLines(painter);
//    drawTargets(painter);
//    drawLabels(painter);
//}

//void PPIWidget::drawBackground(QPainter& painter)
//{
//    // Fill background
//    painter.fillRect(rect(), QColor(20, 20, 20));

//    // Draw semi-circular plot area
//    painter.setBrush(QColor(0, 0, 0));
//    painter.setPen(QPen(QColor(100, 100, 100), 2));

//    QRectF ellipseRect(m_center.x() - m_plotRadius, m_center.y() - m_plotRadius,
//                       2 * m_plotRadius, 2 * m_plotRadius);
//    painter.drawChord(ellipseRect, 0, 180 * 16); // Draw upper semi-circle
//}

//void PPIWidget::drawRangeRings(QPainter& painter)
//{
//    painter.setPen(QPen(QColor(100, 100, 100), 1));

//    for (int i = 1; i <= NUM_RANGE_RINGS; ++i) {
//        float radius = (float(i) / NUM_RANGE_RINGS) * m_plotRadius;
//        QRectF ellipseRect(m_center.x() - radius, m_center.y() - radius,
//                           2 * radius, 2 * radius);
//        painter.drawChord(ellipseRect, 0, 180 * 16);
//    }
//}

//void PPIWidget::drawAzimuthLines(QPainter& painter)
//{
//    painter.setPen(QPen(QColor(100, 100, 100), 1));

//    for (int i = 0; i < NUM_AZIMUTH_LINES; ++i) {
//        float azimuth = MIN_AZIMUTH + (float(i) / (NUM_AZIMUTH_LINES - 1)) * (MAX_AZIMUTH - MIN_AZIMUTH);
//        float radians = qDegreesToRadians(90.0f - azimuth); // Convert to math coordinates

//        QPointF endPoint(
//            m_center.x() + m_plotRadius * std::cos(radians),
//            m_center.y() - m_plotRadius * std::sin(radians)
//        );

//        painter.drawLine(m_center, endPoint);
//    }
//}

//void PPIWidget::drawTargets(QPainter& painter)
//{
//    for (const auto& target : m_currentTargets.targets) {
//        // Skip targets outside our azimuth range
//        if (target.azimuth < MIN_AZIMUTH || target.azimuth > MAX_AZIMUTH) {
//            continue;
//        }

//        // Skip targets outside our range
//        if (target.radius > m_maxRange) {
//            continue;
//        }

//        QPointF targetPos = polarToCartesian(target.radius, target.azimuth);
//        QColor targetColor = getTargetColor(target.radial_speed);

//        // Draw target as a circle
//        painter.setBrush(targetColor);
//        painter.setPen(QPen(targetColor.lighter(), 2));

//        float targetSize = 5.0f;// 6 + target.level * 0.1f; // Size based on level
//        painter.drawEllipse(targetPos, targetSize, targetSize);

//        // Draw target ID just above the red dot
//        painter.setPen(QPen(Qt::white, 1));
//        painter.setFont(QFont("Arial", 8));
//        QPointF textPos = targetPos + QPointF(0, -targetSize - 5);
//        painter.drawText(textPos, QString::number(target.target_id));
//    }
//}

//void PPIWidget::drawLabels(QPainter& painter)
//{
//    painter.setPen(QPen(Qt::white, 1));
//    painter.setFont(QFont("Arial", 10));

//    // Range labels
//    for (int i = 1; i <= NUM_RANGE_RINGS; ++i) {
//        float range = (float(i) / NUM_RANGE_RINGS) * m_maxRange;
//        float radius = (float(i) / NUM_RANGE_RINGS) * m_plotRadius;

//        QPointF labelPos(m_center.x() + radius * 0.707f, m_center.y() - radius * 0.707f);
//        QString rangeText = QString("%1").arg(range, 0, 'f', 1) + "m";
//        painter.drawText(labelPos, rangeText);
//    }

//    // Azimuth labels
//    QFont azFont("Arial", 12, QFont::Bold);
//    painter.setFont(azFont);

//    for (int i = 0; i < NUM_AZIMUTH_LINES; i += 2) { // Label every other line
//        float azimuth = MIN_AZIMUTH + (float(i) / (NUM_AZIMUTH_LINES - 1)) * (MAX_AZIMUTH - MIN_AZIMUTH);
//        float radians = qDegreesToRadians(90.0f - azimuth);

//        QPointF labelPos(
//            m_center.x() + (m_plotRadius + 20) * std::cos(radians),
//            m_center.y() - (m_plotRadius + 20) * std::sin(radians)
//        );

//        QString azText = QString("%1°").arg(azimuth, 0, 'f', 0);
//        QFontMetrics fm(azFont);
//        QRect textRect = fm.boundingRect(azText);
//        labelPos -= QPointF(textRect.width() / 2, -textRect.height() / 2);

//        painter.drawText(labelPos, azText);
//    }

//    // Title
//    painter.setFont(QFont("Arial", 14, QFont::Bold));
//    painter.drawText(QPointF(10, 25), "PPI Display - Target Tracks");
//}

//QColor PPIWidget::getTargetColor(float radialSpeed) const
//{
//    // Color coding based on radial speed
//    // Red for approaching (positive speed)
//    // Blue for receding (negative speed)
//    // Green for stationary

//    if (std::abs(radialSpeed) < 1.0f) {
//        return QColor(0, 255, 0); // Green for stationary
//    } else if (radialSpeed > 0) {
//        // Approaching - red (intensity based on speed)
//        int intensity = std::min(255, static_cast<int>(50 + std::abs(radialSpeed) * 10));
//        return QColor(intensity, 0, 0);
//    } else {
//        // Receding - blue (intensity based on speed)
//        int intensity = std::min(255, static_cast<int>(50 + std::abs(radialSpeed) * 10));
//        return QColor(0, 0, intensity);
//    }
//}

//QPointF PPIWidget::polarToCartesian(float range, float azimuth) const
//{
//    // Normalize range to plot radius
//    float normalizedRange = (range / m_maxRange) * m_plotRadius;

//    // Convert azimuth to radians (0° is up/north, positive is clockwise)
//    float radians = qDegreesToRadians(90.0f - azimuth);

//    return QPointF(
//        m_center.x() + normalizedRange * std::cos(radians),
//        m_center.y() - normalizedRange * std::sin(radians)
//    );
//}



// PPIWidget.cpp - Enhanced with FoV ±20° highlighting

#include "PPIWidget.h"
#include <QPaintEvent>
#include <QResizeEvent>
#include <QFont>
#include <QFontMetrics>
#include <cmath>
#include <QtMath>

PPIWidget::PPIWidget(QWidget *parent)
    : QWidget(parent)
    , m_maxRange(50.0f)
    , m_plotRadius(0)
    , m_fovAngle(20.0f)  // ±20 degrees FoV
{
    setMinimumSize(400, 200);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void PPIWidget::updateTargets(const TargetTrackData& trackData)
{
    m_currentTargets = trackData;
    update();
}

void PPIWidget::setMaxRange(float range)
{
    if (range > 0) {
        m_maxRange = range;
        update();
    }
}

void PPIWidget::setFoVAngle(float angle)
{
    if (angle > 0 && angle <= 90) {
        m_fovAngle = angle;
        update();
    }
}

void PPIWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // Calculate plot area (semi-circle)
    int margin = 40;
    int availableWidth = width() - 2 * margin;
    int availableHeight = height() - 2 * margin;

    // For semi-circle, height should be at least half of width
    int diameter = std::min(availableWidth, availableHeight * 2);
    m_plotRadius = diameter / 2.0f;

    m_plotRect = QRect(
        (width() - diameter) / 2,
        height() - margin - m_plotRadius,
        diameter,
        m_plotRadius
    );

    m_center = QPointF(width() / 2.0f, height() - margin);
}

void PPIWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawFoVHighlight(painter);  // Draw FoV before other elements
    drawRangeRings(painter);
    drawAzimuthLines(painter);
    drawFoVBoundaries(painter); // Draw FoV boundaries on top
    drawTargets(painter);
    drawLabels(painter);
}

void PPIWidget::drawBackground(QPainter& painter)
{
    // Fill background with dark theme
    painter.fillRect(rect(), QColor(30, 58, 95)); // Blue theme background

    // Draw semi-circular plot area
    painter.setBrush(QColor(44, 82, 130)); // Darker blue for plot area
    painter.setPen(QPen(QColor(70, 130, 180), 2));

    QRectF ellipseRect(m_center.x() - m_plotRadius, m_center.y() - m_plotRadius,
                       2 * m_plotRadius, 2 * m_plotRadius);
    painter.drawChord(ellipseRect, 0, 180 * 16); // Draw upper semi-circle
}

void PPIWidget::drawFoVHighlight(QPainter& painter)
{
    // Create a highlighted sector for the Field of View (±20°)
    painter.save();

    // Set up semi-transparent green fill for active FoV area
    QColor fovColor(0, 255, 0, 60); // Green with transparency
    painter.setBrush(QBrush(fovColor));
    painter.setPen(Qt::NoPen);

    // Calculate the angular span in Qt's coordinate system
    // Qt angles: 0° = 3 o'clock, 90° = 6 o'clock, positive = counter-clockwise
    // Our radar: 0° = 12 o'clock (north), positive = clockwise
    // Conversion: qt_angle = 90° - radar_angle

    float startAngleRadar = -m_fovAngle;  // -20°
    float endAngleRadar = m_fovAngle;     // +20°

    // Convert to Qt angles (in 16ths of a degree)
    int startAngleQt = static_cast<int>((90.0f - endAngleRadar) * 16);   // Start from +20° radar
    int spanAngleQt = static_cast<int>((endAngleRadar - startAngleRadar) * 16); // 40° span

    // Draw the FoV sector
    QRectF ellipseRect(m_center.x() - m_plotRadius, m_center.y() - m_plotRadius,
                       2 * m_plotRadius, 2 * m_plotRadius);

    // Only draw the upper half (semi-circle) within FoV
    if (startAngleQt >= 0 && startAngleQt <= 180 * 16) {
        painter.drawPie(ellipseRect, startAngleQt, spanAngleQt);
    }

    painter.restore();
}

void PPIWidget::drawFoVBoundaries(QPainter& painter)
{
    // Draw FoV boundary lines
    painter.save();

    // Use bright green for FoV boundaries
    painter.setPen(QPen(QColor(0, 255, 0), 2, Qt::SolidLine));

    // Draw left boundary (-20°)
    float leftAngle = -m_fovAngle;
    float leftRadians = qDegreesToRadians(90.0f - leftAngle);
    QPointF leftEndPoint(
        m_center.x() + m_plotRadius * std::cos(leftRadians),
        m_center.y() - m_plotRadius * std::sin(leftRadians)
    );
    painter.drawLine(m_center, leftEndPoint);

    // Draw right boundary (+20°)
    float rightAngle = m_fovAngle;
    float rightRadians = qDegreesToRadians(90.0f - rightAngle);
    QPointF rightEndPoint(
        m_center.x() + m_plotRadius * std::cos(rightRadians),
        m_center.y() - m_plotRadius * std::sin(rightRadians)
    );
    painter.drawLine(m_center, rightEndPoint);

    // Draw FoV arc at the perimeter
    QRectF ellipseRect(m_center.x() - m_plotRadius, m_center.y() - m_plotRadius,
                       2 * m_plotRadius, 2 * m_plotRadius);
    int startAngleQt = static_cast<int>((90.0f - rightAngle) * 16);
    int spanAngleQt = static_cast<int>((rightAngle - leftAngle) * 16);
    painter.drawArc(ellipseRect, startAngleQt, spanAngleQt);

    painter.restore();
}

void PPIWidget::drawRangeRings(QPainter& painter)
{
    painter.setPen(QPen(QColor(90, 159, 212, 150), 1)); // Blue theme grid color

    for (int i = 1; i <= NUM_RANGE_RINGS; ++i) {
        float radius = (float(i) / NUM_RANGE_RINGS) * m_plotRadius;
        QRectF ellipseRect(m_center.x() - radius, m_center.y() - radius,
                           2 * radius, 2 * radius);
        painter.drawChord(ellipseRect, 0, 180 * 16);
    }
}

void PPIWidget::drawAzimuthLines(QPainter& painter)
{
    painter.setPen(QPen(QColor(90, 159, 212, 150), 1)); // Blue theme grid color

    for (int i = 0; i < NUM_AZIMUTH_LINES; ++i) {
        float azimuth = MIN_AZIMUTH + (float(i) / (NUM_AZIMUTH_LINES - 1)) * (MAX_AZIMUTH - MIN_AZIMUTH);
        float radians = qDegreesToRadians(90.0f - azimuth);

        QPointF endPoint(
            m_center.x() + m_plotRadius * std::cos(radians),
            m_center.y() - m_plotRadius * std::sin(radians)
        );

        // Draw different styles for lines within and outside FoV
        if (azimuth >= -m_fovAngle && azimuth <= m_fovAngle) {
            // Lines within FoV - slightly brighter
            painter.setPen(QPen(QColor(120, 179, 232, 200), 1));
        } else {
            // Lines outside FoV - dimmer
            painter.setPen(QPen(QColor(90, 159, 212, 100), 1));
        }

        painter.drawLine(m_center, endPoint);
    }
}

void PPIWidget::drawTargets(QPainter& painter)
{
    for (const auto& target : m_currentTargets.targets) {
        // Skip targets outside our azimuth range
        if (target.azimuth < MIN_AZIMUTH || target.azimuth > MAX_AZIMUTH) {
            continue;
        }

        // Skip targets outside our range
        if (target.radius > m_maxRange) {
            continue;
        }

        QPointF targetPos = polarToCartesian(target.radius, target.azimuth);
        QColor targetColor = getTargetColor(target.radial_speed);

        // Check if target is within FoV
        bool inFoV = (target.azimuth >= -m_fovAngle && target.azimuth <= m_fovAngle);

        // Draw target as a circle
        painter.setBrush(targetColor);
        if (inFoV) {
            // Targets within FoV - brighter border
            painter.setPen(QPen(targetColor.lighter(), 3));
        } else {
            // Targets outside FoV - dimmer appearance
            QColor dimmedColor = targetColor;
            dimmedColor.setAlpha(100);
            painter.setBrush(dimmedColor);
            painter.setPen(QPen(dimmedColor.lighter(), 2));
        }

        float targetSize = inFoV ? 6.0f : 4.0f; // Larger targets in FoV
        painter.drawEllipse(targetPos, targetSize, targetSize);

        // Draw target ID
        painter.setPen(QPen(Qt::white, 1));
        painter.setFont(QFont("Arial", inFoV ? 9 : 7)); // Larger text in FoV
        QPointF textPos = targetPos + QPointF(0, -targetSize - 5);
        painter.drawText(textPos, QString::number(target.target_id));
    }
}

void PPIWidget::drawLabels(QPainter& painter)
{
    painter.setPen(QPen(QColor(179, 217, 255), 1)); // Blue theme text color
    painter.setFont(QFont("Arial", 10));

    // Range labels
    for (int i = 1; i <= NUM_RANGE_RINGS; ++i) {
        float range = (float(i) / NUM_RANGE_RINGS) * m_maxRange;
        float radius = (float(i) / NUM_RANGE_RINGS) * m_plotRadius;

        QPointF labelPos(m_center.x() + radius * 0.707f, m_center.y() - radius * 0.707f);
        QString rangeText = QString("%1").arg(range, 0, 'f', 1) + "m";
        painter.drawText(labelPos, rangeText);
    }

    // Azimuth labels
    QFont azFont("Arial", 12, QFont::Bold);
    painter.setFont(azFont);
    painter.setPen(QPen(QColor(179, 217, 255), 1));

    for (int i = 0; i < NUM_AZIMUTH_LINES; i += 2) {
        float azimuth = MIN_AZIMUTH + (float(i) / (NUM_AZIMUTH_LINES - 1)) * (MAX_AZIMUTH - MIN_AZIMUTH);
        float radians = qDegreesToRadians(90.0f - azimuth);

        QPointF labelPos(
            m_center.x() + (m_plotRadius + 20) * std::cos(radians),
            m_center.y() - (m_plotRadius + 20) * std::sin(radians)
        );

        QString azText = QString("%1°").arg(azimuth, 0, 'f', 0);
        QFontMetrics fm(azFont);
        QRect textRect = fm.boundingRect(azText);
        labelPos -= QPointF(textRect.width() / 2, -textRect.height() / 2);

        // Highlight FoV boundary labels
        if (azimuth == -m_fovAngle || azimuth == m_fovAngle) {
            painter.setPen(QPen(QColor(0, 255, 0), 1)); // Green for FoV boundaries
        } else {
            painter.setPen(QPen(QColor(179, 217, 255), 1)); // Normal blue
        }

        painter.drawText(labelPos, azText);
    }

    // Title and FoV indicator
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.setPen(QPen(QColor(135, 206, 235), 1)); // Sky blue for title
    painter.drawText(QPointF(10, 25), "PPI Display - Target Tracks");

    // FoV information
    painter.setFont(QFont("Arial", 10));
    painter.setPen(QPen(QColor(0, 255, 0), 1)); // Green for FoV info
    QString fovText = QString("FoV: ±%1°").arg(m_fovAngle, 0, 'f', 0);
    painter.drawText(QPointF(10, 45), fovText);

    // Legend
    painter.setFont(QFont("Arial", 8));
    painter.setPen(QPen(QColor(179, 217, 255), 1));
    painter.drawText(QPointF(10, height() - 30), "Green area: Active Field of View");
    painter.drawText(QPointF(10, height() - 15), "Bright targets: Within FoV, Dim targets: Outside FoV");
}

QColor PPIWidget::getTargetColor(float radialSpeed) const
{
    // Enhanced color coding based on radial speed
    if (std::abs(radialSpeed) < 1.0f) {
        return QColor(0, 255, 0); // Green for stationary
    } else if (radialSpeed > 0) {
        // Approaching - red (intensity based on speed)
        int intensity = std::min(255, static_cast<int>(100 + std::abs(radialSpeed) * 15));
        return QColor(intensity, 50, 50);
    } else {
        // Receding - blue (intensity based on speed)
        int intensity = std::min(255, static_cast<int>(100 + std::abs(radialSpeed) * 15));
        return QColor(50, 50, intensity);
    }
}

QPointF PPIWidget::polarToCartesian(float range, float azimuth) const
{
    // Normalize range to plot radius
    float normalizedRange = (range / m_maxRange) * m_plotRadius;

    // Convert azimuth to radians (0° is up/north, positive is clockwise)
    float radians = qDegreesToRadians(90.0f - azimuth);

    return QPointF(
        m_center.x() + normalizedRange * std::cos(radians),
        m_center.y() - normalizedRange * std::sin(radians)
    );
}
