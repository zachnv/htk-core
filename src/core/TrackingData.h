#ifndef TRACKINGDATA_H
#define TRACKINGDATA_H

#include <cstdint>
#include <chrono>

namespace OrbitView {

    // 6DOF tracking data structure
    struct TrackingData {
        // Translation (millimeters from center position)
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        // Rotation (degrees)
        float yaw = 0.0f;    // Left/right
        float pitch = 0.0f;  // Up/down
        float roll = 0.0f;   // Tilt

        // Metadata
        uint64_t timestamp = 0;  // Microseconds since epoch
        float confidence = 0.0f; // 0.0 to 1.0 - tracking quality
        bool isValid = false;    // Is this data usable?

        // Helper to get current timestamp
        static uint64_t now() {
            auto now = std::chrono::system_clock::now();
            auto micros = std::chrono::duration_cast<std::chrono::microseconds>(
                now.time_since_epoch()
            ).count();
            return static_cast<uint64_t>(micros);
        }

        // Reset to neutral position
        void reset() {
            x = y = z = 0.0f;
            yaw = pitch = roll = 0.0f;
            confidence = 0.0f;
            isValid = false;
        }
    };

} // namespace OrbitView

#endif // TRACKINGDATA_H