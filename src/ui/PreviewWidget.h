#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "../core/HeadTracker.h"

namespace OrbitView {

    class PreviewWidget : public QWidget {
        Q_OBJECT

    public:
        explicit PreviewWidget(QWidget* parent = nullptr);
        ~PreviewWidget();

        void setHeadTracker(HeadTracker* tracker);
        void startPreview();
        void stopPreview();

    protected:
        void paintEvent(QPaintEvent* event) override;

    private slots:
        void updateFrame();

    private:
        HeadTracker* m_tracker;
        QTimer* m_updateTimer;
        QImage m_currentImage;

        QImage cvMatToQImage(const cv::Mat& mat);
        void drawTrackingInfo(QPainter& painter);
        void drawHeadIndicator(QPainter& painter);
    };

} // namespace OrbitView

#endif // PREVIEWWIDGET_H