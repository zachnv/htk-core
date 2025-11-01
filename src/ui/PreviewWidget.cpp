#include "PreviewWidget.h"
#include <QPainter>
#include <QImage>

namespace OrbitView {

PreviewWidget::PreviewWidget(QWidget* parent)
    : QWidget(parent)
    , m_tracker(nullptr)
    , m_updateTimer(new QTimer(this))
{
    setMinimumSize(640, 480);
    setStyleSheet("background-color: black;");
    
    connect(m_updateTimer, &QTimer::timeout, this, &PreviewWidget::updateFrame);
}

PreviewWidget::~PreviewWidget() {
    stopPreview();
}

void PreviewWidget::setHeadTracker(HeadTracker* tracker) {
    m_tracker = tracker;
}

void PreviewWidget::startPreview() {
    if (m_tracker) {
        m_updateTimer->start(33);
    }
}

void PreviewWidget::stopPreview() {
    m_updateTimer->stop();
    m_currentImage = QImage();
    update();
}

void PreviewWidget::updateFrame() {
    if (!m_tracker || !m_tracker->isRunning()) {
        return;
    }
    
    // Get current frame from webcam tracker
    update();
}

void PreviewWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw camera feed if available
    if (!m_currentImage.isNull()) {
        QRect imageRect = m_currentImage.rect();
        QRect targetRect = rect();

        imageRect.moveCenter(targetRect.center());
        painter.drawImage(imageRect, m_currentImage);
    }
    
    // Draw tracking info overlay
    if (m_tracker && m_tracker->isRunning()) {
        drawTrackingInfo(painter);
    } else {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 16));
        painter.drawText(rect(), Qt::AlignCenter, "No tracking active");
    }
}

void PreviewWidget::drawTrackingInfo(QPainter& painter) {
    TrackingData data = m_tracker->getCurrentData();
    
    // Draw tracking status
    painter.setPen(data.isValid ? Qt::green : Qt::red);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(10, 25, data.isValid ? "TRACKING" : "NO FACE DETECTED");
    
    if (!data.isValid) {
        return;
    }
    
    // Draw 6DOF data
    painter.setPen(Qt::white);
    painter.setFont(QFont("Courier", 11));
    
    int y = 60;
    int lineHeight = 20;
    
    painter.drawText(10, y, QString("Yaw:   %1°").arg(data.yaw, 7, 'f', 2));
    y += lineHeight;
    painter.drawText(10, y, QString("Pitch: %1°").arg(data.pitch, 7, 'f', 2));
    y += lineHeight;
    painter.drawText(10, y, QString("Roll:  %1°").arg(data.roll, 7, 'f', 2));
    y += lineHeight * 1.5;
    
    painter.drawText(10, y, QString("X: %1 mm").arg(data.x, 7, 'f', 1));
    y += lineHeight;
    painter.drawText(10, y, QString("Y: %1 mm").arg(data.y, 7, 'f', 1));
    y += lineHeight;
    painter.drawText(10, y, QString("Z: %1 mm").arg(data.z, 7, 'f', 1));
    
    // Draw confidence bar
    y += lineHeight * 1.5;
    painter.drawText(10, y, "Confidence:");
    
    QRect confidenceBar(10, y + 5, 200, 15);
    painter.setPen(Qt::white);
    painter.drawRect(confidenceBar);
    
    QRect confidenceFill = confidenceBar.adjusted(2, 2, -2, -2);
    confidenceFill.setWidth((confidenceFill.width() * data.confidence));
    
    QColor fillColor = data.confidence > 0.7 ? Qt::green : 
                       data.confidence > 0.4 ? Qt::yellow : Qt::red;
    painter.fillRect(confidenceFill, fillColor);
    
    // Draw 3D head orientation indicator
    drawHeadIndicator(painter);
}

void PreviewWidget::drawHeadIndicator(QPainter& painter) {
    int centerX = width() - 100;
    int centerY = height() - 100;
    int size = 60;
    
    TrackingData data = m_tracker->getCurrentData();
    
    // Convert angles to radians
    float yawRad = data.yaw * M_PI / 180.0f;
    float pitchRad = data.pitch * M_PI / 180.0f;
    
    // Draw head circle
    painter.setPen(QPen(Qt::white, 2));
    painter.drawEllipse(QPoint(centerX, centerY), size/2, size/2);
    
    // Draw nose direction
    float noseX = centerX + (size/2) * sin(yawRad) * cos(pitchRad);
    float noseY = centerY - (size/2) * sin(pitchRad);
    
    painter.setPen(QPen(Qt::red, 3));
    painter.drawLine(centerX, centerY, noseX, noseY);
    
    // Draw label
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    painter.drawText(centerX - 30, height() - 20, "Head Pose");
}

QImage PreviewWidget::cvMatToQImage(const cv::Mat& mat) {
    if (mat.empty()) {
        return QImage();
    }
    
    switch (mat.type()) {
        case CV_8UC4: {
            return QImage(mat.data, mat.cols, mat.rows, 
                         static_cast<int>(mat.step), QImage::Format_ARGB32);
        }
        case CV_8UC3: {
            cv::Mat rgb;
            cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
            return QImage(rgb.data, rgb.cols, rgb.rows,
                         static_cast<int>(rgb.step), QImage::Format_RGB888).copy();
        }
        case CV_8UC1: {
            return QImage(mat.data, mat.cols, mat.rows,
                         static_cast<int>(mat.step), QImage::Format_Grayscale8);
        }
        default:
            return QImage();
    }
}

} // namespace OrbitView