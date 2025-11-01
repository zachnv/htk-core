#ifndef HEADTRACKER_H
#define HEADTRACKER_H

#include "TrackingData.h"
#include "../input/WebcamTracker.h"

#ifdef _WIN32
#include "../output/FreeTrackOutput.h"
#include "../output/TrackIROutput.h"
#endif

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

namespace OrbitView {

    class HeadTracker {
    public:
        HeadTracker();
        ~HeadTracker();

        // Lifecycle
        bool initialize(int cameraIndex = 0);
        bool start();
        void stop();
        void shutdown();

        // Control
        void recenter();
        void pause();
        void resume();

        // Status
        bool isRunning() const { return m_isRunning; }
        bool isTracking() const;
        TrackingData getCurrentData() const;

        // Settings
        void setSmoothing(float factor);
        void enableFreeTrack(bool enable);
        void enableTrackIR(bool enable);

    private:
        // Components
        std::unique_ptr<WebcamTracker> m_webcamTracker;

#ifdef _WIN32
        std::unique_ptr<FreeTrackOutput> m_freeTrackOutput;
        std::unique_ptr<TrackIROutput> m_trackIROutput;
#endif

        // Threading
        std::unique_ptr<std::thread> m_updateThread;
        std::atomic<bool> m_isRunning;
        std::atomic<bool> m_isPaused;
        std::atomic<bool> m_shouldStop;

        // Data
        TrackingData m_currentData;
        TrackingData m_centerOffset;
        mutable std::mutex m_dataMutex;

        // Settings
        bool m_freeTrackEnabled;
        bool m_trackIREnabled;

        // Update loop (runs in separate thread)
        void updateLoop();

        // Apply center offset to data
        TrackingData applyCenterOffset(const TrackingData& data) const;
    };

} // namespace OrbitView

#endif // HEADTRACKER_H