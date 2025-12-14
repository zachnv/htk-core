#include "TrackIROutput.h"

#include <iostream>
#include <cmath>

#include "../core/TrackingData.h"

namespace htk::output {

TrackIROutput::TrackIROutput()
    : m_isInitialized(false)
#ifdef _WIN32
    , m_hMapFile(nullptr)
    , m_pMemory(nullptr)
    , m_frameCounter(0)
#endif
{
}

TrackIROutput::~TrackIROutput() {
    shutdown();
}

bool TrackIROutput::initialize() {
#ifdef _WIN32
    // Create shared memory for TrackIR protocol
    m_hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(TrackIRData),
        "TrackIR5"  // TrackIR 5 shared memory name
    );

    if (m_hMapFile == NULL) {
        std::cerr << "Failed to create TrackIR shared memory. Error: "
                  << GetLastError() << std::endl;
        return false;
    }

    m_pMemory = MapViewOfFile(
        m_hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(TrackIRData)
    );

    if (m_pMemory == nullptr) {
        std::cerr << "Failed to map TrackIR shared memory. Error: "
                  << GetLastError() << std::endl;
        CloseHandle(m_hMapFile);
        m_hMapFile = nullptr;
        return false;
    }

    // Initialize memory to zero
    ZeroMemory(m_pMemory, sizeof(TrackIRData));

    m_isInitialized = true;
    std::cout << "TrackIR output initialized successfully" << std::endl;
    return true;
#else
    std::cerr << "TrackIR output is only supported on Windows" << std::endl;
    return false;
#endif
}

bool TrackIROutput::sendData(const htk::core::TrackingData& data) {
    if (!m_isInitialized) {
        return false;
    }

#ifdef _WIN32
    if (m_pMemory == nullptr) {
        return false;
    }

    auto* tirData = static_cast<TrackIRData*>(m_pMemory);

    // Convert degrees to radians
    constexpr float degToRad = 3.14159265359f / 180.0f;

    // Fill in the data
    tirData->status = data.isValid ? 1 : 0;
    tirData->frame  = ++m_frameCounter;
    tirData->cksum  = 0;  // Not used

    // Convert tracking data (degrees to radians)
    tirData->yaw   = data.yaw   * degToRad;
    tirData->pitch = data.pitch * degToRad;
    tirData->roll  = data.roll  * degToRad;
    tirData->x     = data.x;
    tirData->y     = data.y;
    tirData->z     = data.z;

    // Copy to raw data
    tirData->rawyaw   = tirData->yaw;
    tirData->rawpitch = tirData->pitch;
    tirData->rawroll  = tirData->roll;
    tirData->rawx     = tirData->x;
    tirData->rawy     = tirData->y;
    tirData->rawz     = tirData->z;

    // Point data
    tirData->x1 = tirData->y1 = 0.0f;
    tirData->x2 = tirData->y2 = 0.0f;
    tirData->x3 = tirData->y3 = 0.0f;

    return true;
#else
    (void)data;
    return false;
#endif
}

void TrackIROutput::shutdown() {
#ifdef _WIN32
    if (m_pMemory != nullptr) {
        UnmapViewOfFile(m_pMemory);
        m_pMemory = nullptr;
    }

    if (m_hMapFile != nullptr) {
        CloseHandle(m_hMapFile);
        m_hMapFile = nullptr;
    }
#endif

    m_isInitialized = false;
}

} // namespace htk::output