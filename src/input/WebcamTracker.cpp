#include "WebcamTracker.h"
#include <iostream>

namespace htk::input {

WebcamTracker::WebcamTracker()
    : m_isInitialized(false)
    , m_isTracking(false)
    , m_smoothingFactor(0.5f)
{
    m_trackingData.reset();
    m_centerPosition.reset();
}

WebcamTracker::~WebcamTracker() {
    shutdown();
}

    bool WebcamTracker::initialize(int cameraIndex) {
    // Open camera
    m_camera.open(cameraIndex);
    if (!m_camera.isOpened()) {
        std::cerr << "Failed to open camera " << cameraIndex << std::endl;
        return false;
    }

    // Set camera properties
    m_camera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    m_camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    m_camera.set(cv::CAP_PROP_FPS, 30);

    // Cascade file
    std::vector<std::string> cascadePaths = {
        "resources/models/haarcascade_frontalface_default.xml",
        "../resources/models/haarcascade_frontalface_default.xml",
        "../../resources/models/haarcascade_frontalface_default.xml",
        "../../../resources/models/haarcascade_frontalface_default.xml"
    };

    bool cascadeLoaded = false;
    for (const auto& path : cascadePaths) {
        if (m_faceCascade.load(path)) {
            std::cout << "Loaded face cascade from: " << path << std::endl;
            cascadeLoaded = true;
            break;
        }
    }

    if (!cascadeLoaded) {
        std::cerr << "Failed to load face cascade from any path" << std::endl;
        std::cerr << "Tried:" << std::endl;
        for (const auto& path : cascadePaths) {
            std::cerr << "  - " << path << std::endl;
        }
        std::cerr << "Download from: https://github.com/opencv/opencv/tree/master/data/haarcascades" << std::endl;
        return false;
    }

    m_isInitialized = true;
    std::cout << "Head-Tracking Kit initialized successfully" << std::endl;
    return true;
}

bool WebcamTracker::update() {
    if (!m_isInitialized || !m_camera.isOpened()) {
        return false;
    }

    // Capture frame
    if (!m_camera.read(m_currentFrame)) {
        std::cerr << "Failed to read frame from camera" << std::endl;
        return false;
    }

    if (m_currentFrame.empty()) {
        return false;
    }

    // Detect face
    cv::Rect faceRect;
    if (detectFace(m_currentFrame, faceRect)) {
        m_lastFaceRect = faceRect;
        estimatePose(faceRect);
        m_isTracking = true;
        m_trackingData.isValid = true;
        m_trackingData.confidence = 1.0f;
    } else {
        m_isTracking = false;
        m_trackingData.isValid = false;
        m_trackingData.confidence = 0.0f;
    }

    m_trackingData.timestamp = htk::core::TrackingData::now();

    return true;
}

bool WebcamTracker::detectFace(const cv::Mat& frame, cv::Rect& faceRect) {
    // Convert to grayscale for better detection
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(gray, gray);

    // Detect faces
    std::vector<cv::Rect> faces;
    m_faceCascade.detectMultiScale(
        gray,
        faces,
        1.1,  // Scale factor
        3,    // Min neighbors
        0,    // Flags
        cv::Size(80, 80)  // Min size
    );

    if (faces.empty()) {
        return false;
    }

    // Use the largest face in view
    faceRect = faces[0];
    for (const auto& face : faces) {
        if (face.area() > faceRect.area()) {
            faceRect = face;
        }
    }

    return true;
}

void WebcamTracker::estimatePose(const cv::Rect& faceRect) {
    // Get frame dimensions
    int frameWidth = m_currentFrame.cols;
    int frameHeight = m_currentFrame.rows;

    // Calculate center of face
    float faceCenterX = faceRect.x + faceRect.width / 2.0f;
    float faceCenterY = faceRect.y + faceRect.height / 2.0f;

    // Calculate center of frame
    float frameCenterX = frameWidth / 2.0f;
    float frameCenterY = frameHeight / 2.0f;

    // Calculate deltas from center (normalized -1 to 1)
    float deltaX = (faceCenterX - frameCenterX) / frameCenterX;
    float deltaY = (faceCenterY - frameCenterY) / frameCenterY;

    // Estimate yaw (left/right) from horizontal position
    float newYaw = -deltaX * 45.0f;

    // Estimate pitch (up/down) from vertical position
    float newPitch = -deltaY * 30.0f;

    // Estimate Z (depth) from face size
    // Larger face = closer to camera = negative Z
    float referenceFaceWidth = 150.0f;
    float faceSize = static_cast<float>(faceRect.width);
    float newZ = (referenceFaceWidth - faceSize) * 2.0f;

    // Estimate X and Y translation from face position
    float newX = deltaX * 100.0f;
    float newY = deltaY * 100.0f;

    // Apply smoothing
    if (m_trackingData.isValid && m_smoothingFactor > 0.0f) {
        m_trackingData.yaw = m_trackingData.yaw * m_smoothingFactor + newYaw * (1.0f - m_smoothingFactor);
        m_trackingData.pitch = m_trackingData.pitch * m_smoothingFactor + newPitch * (1.0f - m_smoothingFactor);
        m_trackingData.x = m_trackingData.x * m_smoothingFactor + newX * (1.0f - m_smoothingFactor);
        m_trackingData.y = m_trackingData.y * m_smoothingFactor + newY * (1.0f - m_smoothingFactor);
        m_trackingData.z = m_trackingData.z * m_smoothingFactor + newZ * (1.0f - m_smoothingFactor);
    } else {
        m_trackingData.yaw = newYaw;
        m_trackingData.pitch = newPitch;
        m_trackingData.x = newX;
        m_trackingData.y = newY;
        m_trackingData.z = newZ;
    }
    m_trackingData.roll = 0.0f;
}

    htk::core::TrackingData WebcamTracker::getTrackingData() const {
    return m_trackingData;
}

cv::Mat WebcamTracker::getCurrentFrame() const {
    return m_currentFrame.clone();
}

void WebcamTracker::setSmoothing(float factor) {
    m_smoothingFactor = std::max(0.0f, std::min(1.0f, factor));
}

void WebcamTracker::shutdown() {
    if (m_camera.isOpened()) {
        m_camera.release();
    }
    m_isInitialized = false;
    m_isTracking = false;
}

} // namespace htk::input