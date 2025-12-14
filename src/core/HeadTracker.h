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

namespace htk::core {

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
        htk::core::TrackingData getCurrentData() const;

        // Settings
        void setSmoothing(float factor);
        void enableFreeTrack(bool enable);
        void enableTrackIR(bool enable);

    private:
        // Components
        std::unique_ptr<htk::input::WebcamTracker> m_webcamTracker;

#ifdef _WIN32
        std::unique_ptr<htk::output::FreeTrackOutput> m_freeTrackOutput;
        std::unique_ptr<htk::output::TrackIROutput>  m_trackIROutput;
#endif

        // Threading
        std::unique_ptr<std::thread> m_updateThread;
        std::atomic<bool> m_isRunning{false};
        std::atomic<bool> m_isPaused{false};
        std::atomic<bool> m_shouldStop{false};

        // Data
        htk::core::TrackingData m_currentData;
        htk::core::TrackingData m_centerOffset;
        mutable std::mutex m_dataMutex;

        // Settings
        bool m_freeTrackEnabled{true};
        bool m_trackIREnabled{true};

        // Update loop (runs in separate thread)
        void updateLoop();

        // Apply center offset to data
        htk::core::TrackingData applyCenterOffset(
            const htk::core::TrackingData& data
        ) const;
    };

} // namespace htk::core

#endif // HEADTRACKER_H