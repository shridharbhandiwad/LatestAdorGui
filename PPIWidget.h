#pragma once

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <vector>
#include "DataStructures.h"

class PPIWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PPIWidget(QWidget *parent = nullptr);

    void updateTargets(const TargetTrackData& trackData);
    void setMaxRange(float range);
    void setFoVAngle(float angle);  // NEW: Set Field of View angle

    float getMaxRange() const { return m_maxRange; }
    float getFoVAngle() const { return m_fovAngle; }  // NEW: Get FoV angle

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // Drawing functions
    void drawBackground(QPainter& painter);
    void drawFoVHighlight(QPainter& painter);   // NEW: Draw FoV sector highlight
    void drawFoVBoundaries(QPainter& painter);  // NEW: Draw FoV boundary lines
    void drawRangeRings(QPainter& painter);
    void drawAzimuthLines(QPainter& painter);
    void drawTargets(QPainter& painter);
    void drawLabels(QPainter& painter);

    // Utility functions
    QColor getTargetColor(float radialSpeed) const;
    QPointF polarToCartesian(float range, float azimuth) const;

    // Data members
    TargetTrackData m_currentTargets;

    // Display parameters
    float m_maxRange;           // Maximum range to display (meters)
    float m_fovAngle;           // NEW: Field of View half-angle (e.g., 20° for ±20°)
    float m_plotRadius;         // Radius of the plot area in pixels
    QPointF m_center;           // Center point of the plot
    QRect m_plotRect;           // Bounding rectangle of the plot

    // Constants for radar display
    static const int NUM_RANGE_RINGS = 5;      // Number of concentric range rings
    static const int NUM_AZIMUTH_LINES = 19;   // Number of radial azimuth lines

    // Coordinate system constants
    static constexpr float MIN_AZIMUTH = -90.0f;    // Minimum azimuth angle
    static constexpr float MAX_AZIMUTH = 90.0f;     // Maximum azimuth angle
};


