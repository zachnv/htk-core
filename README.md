# OrbitView

Actively maintained, open-source webcam-based head tracking for simulation games in C++ utilizing FreeTrack and TrackIR input and output protocols.
This is very much work-in-progress and actively being worked on and optimized.

## Features & Roadmap
- 6DOF head tracking using webcam
- Support for all simulators (mainly: DCS World & Microsoft Flight Simulator)
- Low latency tracking
- Qt6 interface
- Optimize headtracking and head-pose estimation so that, even when your head is turned 90 deg, it can still track the pose.
- Currently, the head-pose estimation and landmark detection is very rough and will lose track of the face if it's turned +45 deg.
