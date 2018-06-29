#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "Argus/Argus.h"
#include "EGLStream/EGLStream.h"
#include "NvVideoConverter.h"

class ArgusCameraConfig
{
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
