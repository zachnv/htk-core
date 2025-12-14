#include "FreeTrackOutput.h"

#include <iostream>
#include <cmath>

#include "../core/TrackingData.h"

namespace htk::output {

FreeTrackOutput::FreeTrackOutput()
    : m_isInitialized(false)
#ifdef _WIN32
    , m_hMapFile(nullptr)
    , m_pMemory(nullptr)
    , m_dataID(0)
#endif
{
}

FreeTrackOutput::~FreeTrackOutput() {
    shutdown();
}

bool FreeTrackOutput::initialize() {
#ifdef _WIN32
    // Create shared memory for FreeTrack protocol
    m_hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(FreeTrackData),
        "FT_SharedMem"
    );

    if (m_hMapFile == NULL) {
        std::cerr << "Failed to create FreeTrack shared memory. Error: "
                  << GetLastError() << std::endl;
        return false;
    }

    m_pMemory = MapViewOfFile(
        m_hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(FreeTrackData)
    );

    if (m_pMemory == nullptr) {
        std::cerr << "Failed to map FreeTrack shared memory. Error: "
                  << GetLastError() << std::endl;
        CloseHandle(m_hMapFile);
        m_hMapFile = nullptr;
        return false;
    }

    // Initialize memory to zero
    ZeroMemory(m_pMemory, sizeof(FreeTrackData));

    m_isInitialized = true;
    std::cout << "FreeTrack output initialized successfully" << std::endl;
    return true;
#else
    std::cerr << "FreeTrack output is only supported on Windows" << std::endl;
    return false;
#endif
}

bool FreeTrackOutput::sendData(const htk::core::TrackingData& data) {
    if (!m_isInitialized) {
        return false;
    }

#ifdef _WIN32
    if (m_pMemory == nullptr) {
        return false;
    }

    auto* ftData = static_cast<FreeTrackData*>(m_pMemory);

    // Convert degrees to radians
    constexpr float degToRad = 3.14159265359f / 180.0f;

    // Fill in the data
    ftData->dataID = ++m_dataID;
    ftData->camWidth = 640;
    ftData->camHeight = 480;

    // Orientation
    ftData->yaw   = data.yaw   * degToRad;
    ftData->pitch = data.pitch * degToRad;
    ftData->roll  = data.roll  * degToRad;

    // Translation
    ftData->x = data.x;
    ftData->y = data.y;
    ftData->z = data.z;

    // Raw values
    ftData->rawyaw   = ftData->yaw;
    ftData->rawpitch = ftData->pitch;
    ftData->rawroll  = ftData->roll;
    ftData->rawx     = ftData->x;
    ftData->rawy     = ftData->y;
    ftData->rawz     = ftData->z;

    // Point data
    ftData->x1 = ftData->y1 = 0.0f;
    ftData->x2 = ftData->y2 = 0.0f;
    ftData->x3 = ftData->y3 = 0.0f;
    ftData->x4 = ftData->y4 = 0.0f;

    return true;
#else
    return false;
#endif
}

void FreeTrackOutput::shutdown() {
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