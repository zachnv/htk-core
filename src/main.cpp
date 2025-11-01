#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include "core/HeadTracker.h"
#include "ui/PreviewWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create main window
    QMainWindow window;
    window.setWindowTitle("OrbitView - Head Tracker");
    window.resize(800, 700);

    // Create central widget with layout
    QWidget* centralWidget = new QWidget(&window);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    // Add preview widget
    OrbitView::PreviewWidget* preview = new OrbitView::PreviewWidget(centralWidget);
    layout->addWidget(preview);

    // Status label
    QLabel* statusLabel = new QLabel("Status: Not started", centralWidget);
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);

    // Start button
    QPushButton* startButton = new QPushButton("Start Tracking", centralWidget);
    layout->addWidget(startButton);

    // Stop button
    QPushButton* stopButton = new QPushButton("Stop Tracking", centralWidget);
    stopButton->setEnabled(false);
    layout->addWidget(stopButton);

    // Recenter button
    QPushButton* recenterButton = new QPushButton("Recenter (F12)", centralWidget);
    recenterButton->setEnabled(false);
    layout->addWidget(recenterButton);

    window.setCentralWidget(centralWidget);

    // Create head tracker
    OrbitView::HeadTracker tracker;
    preview->setHeadTracker(&tracker);

    // Connect buttons
    QObject::connect(startButton, &QPushButton::clicked, [&]() {
        if (tracker.initialize(0)) {
            if (tracker.start()) {
                preview->startPreview();
                statusLabel->setText("Status: Tracking active");
                startButton->setEnabled(false);
                stopButton->setEnabled(true);
                recenterButton->setEnabled(true);
            } else {
                statusLabel->setText("Status: Failed to start");
            }
        } else {
            statusLabel->setText("Status: Failed to initialize");
        }
    });

    QObject::connect(stopButton, &QPushButton::clicked, [&]() {
        tracker.stop();
        preview->stopPreview();
        statusLabel->setText("Status: Stopped");
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
        recenterButton->setEnabled(false);
    });

    QObject::connect(recenterButton, &QPushButton::clicked, [&]() {
        tracker.recenter();
        statusLabel->setText("Status: Recentered");
    });

    window.show();

    return app.exec();
}