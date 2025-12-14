#ifndef TRACKIROUTPUT_H
#define TRACKIROUTPUT_H

#include "../core/TrackingData.h"
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace htk::output {

    class TrackIROutput {
    public:
        TrackIROutput();
        ~TrackIROutput();

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

        // TrackIR shared memory structure
        struct TrackIRData {
            uint16_t status;    // 0 = stopped, 1 = running
            uint16_t frame;     // Frame counter
            uint32_t cksum;     // Checksum (not used)

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

            // Point data
            float x1, y1;
            float x2, y2;
            float x3, y3;
        };

        uint16_t m_frameCounter = 0;
#endif
    };

} // namespace htk::output

#endif // TRACKIROUTPUT_H