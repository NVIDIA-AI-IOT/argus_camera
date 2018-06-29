#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "Argus/Argus.h"
#include "EGLStream/EGLStream.h"
#include "NvVideoConverter.h"

#define WIDTH_IDX 0
#define HEIGHT_IDX 1

#define ONE_SECOND_NANOS 1000000000
class ArgusCameraConfig
{
public:

  static ArgusCameraConfig DEFAULT_DEVKIT_CONFIG() {
    ArgusCameraConfig c;
    c.mDeviceId = 0;
    c.mSourceClipRect = { 0.0, 0.0, 1.0, 1.0 };
    c.mStreamResolution = { 640, 480 };
    c.mVideoConverterResolution = { 640, 480 };
    c.mFrameDurationRange = { ONE_SECOND_NANOS / 30, ONE_SECOND_NANOS / 30 }; // 30fps
    c.mSensorMode = 0;
    return c;
  };

  uint32_t mDeviceId;
  std::vector<float> mSourceClipRect;
  std::vector<uint32_t> mStreamResolution;
  std::vector<uint32_t> mVideoConverterResolution;
  std::vector<uint64_t> mFrameDurationRange;
  uint32_t mSensorMode;

  size_t getOutputSizeBytes();
  uint32_t getNumChannels();
};

class ArgusCamera
{
public:
  static ArgusCamera *createArgusCamera(const ArgusCameraConfig &config, int *info=nullptr);
  ~ArgusCamera();
  int read(uint8_t *rgba);

private:
  ArgusCameraConfig mConfig;

  Argus::UniqueObj<Argus::CaptureSession> mCaptureSession;
  Argus::UniqueObj<Argus::OutputStream> mStream;
  Argus::UniqueObj<EGLStream::FrameConsumer> mFrameConsumer;
  NvVideoConverter *mVideoConverter;
};
