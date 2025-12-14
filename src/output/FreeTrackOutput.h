#ifndef FREETRACKOUTPUT_H
#define FREETRACKOUTPUT_H

#include "../core/TrackingData.h"
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace htk::output {

    class FreeTrackOutput {
    public:
        FreeTrackOutput();
        ~FreeTrackOutput();

        // Initialize shared memory
        bool initialize();

        // Send tracking data to games
        bool sendData(const htk::core::TrackingData& data);

        // Cleanup
        void shutdown();

        bool isInitialized() const { return m_isInitialized; }

    private:
        bool m_isInitialized = false;

#ifdef _WIN32
        HANDLE m_hMapFile = nullptr;
        void* m_pMemory = nullptr;

        // FreeTrack shared memory structure
        struct FreeTrackData {
            uint32_t dataID;
            int32_t camWidth;
            int32_t camHeight;

            // 6DOF data
            float yaw;      // Radians
            float pitch;    // Radians
            float roll;     // Radians
            float x;        // Millimeters
            float y;        // Millimeters
            float z;        // Millimeters

            // Raw data (unfiltered)
            float rawyaw;
            float rawpitch;
            float rawroll;
            float rawx;
            float rawy;
            float rawz;

            // Additional data
            float x1, y1, x2, y2, x3, y3, x4, y4;
        };

        uint32_t m_dataID = 0;
#endif
    };

} // namespace htk::output

#endif // FREETRACKOUTPUT_H