#pragma once

#include <cstdint>
#include <cstddef>

namespace Pilot {

struct PilotCapturedFrame {
    const uint8_t* data;      // raw ARGB pixel data (4 bytes per pixel)
    size_t dataSize;
    int width;
    int height;
    int sourceWidth;          // original window/screen width before scaling
    int sourceHeight;         // original window/screen height before scaling
};

class PilotScreenCaptureProvider {
public:
    virtual ~PilotScreenCaptureProvider() = default;

    // Called when live settings change (resolution, fps).
    // Provider should adjust its capture parameters accordingly.
    virtual void onLiveSettingsChanged(int maxDimension, int fps) = 0;

    // Capture a frame. Fills `frame` with raw ARGB pixel data.
    // Returns true if a frame was successfully captured.
    // The data pointer must remain valid until the next call to captureFrame().
    virtual bool captureFrame(PilotCapturedFrame& frame) = 0;
};

} // namespace Pilot
