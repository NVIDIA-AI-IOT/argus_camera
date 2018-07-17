#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#define WIDTH_IDX 0
#define HEIGHT_IDX 1
#define ONE_SECOND_NANOS 1000000000

class ArgusCameraConfig
{
public:

  void setDeviceId(uint32_t deviceId) { mDeviceId = deviceId; };
  uint32_t getDeviceId() { return mDeviceId; };

  void setSourceClipRect(std::vector<float> sourceClipRect) { mSourceClipRect = sourceClipRect; };
  std::vector<float> getSourceClipRect() { return mSourceClipRect; };

  void setStreamResolution(std::vector<uint32_t> streamResolution) { mStreamResolution = streamResolution; };
  std::vector<uint32_t> getStreamResolution() { return mStreamResolution; };

  void setVideoConverterResolution(std::vector<uint32_t> videoConverterResolution) { mVideoConverterResolution = videoConverterResolution; };
  std::vector<uint32_t> getVideoConverterResolution() { return mVideoConverterResolution; };

  void setFrameDurationRange(std::vector<uint64_t> frameDurationRange) { mFrameDurationRange = frameDurationRange; };
  std::vector<uint64_t> getFrameDurationRange() { return mFrameDurationRange; };

  void setSensorMode(uint32_t sensorMode) { mSensorMode = sensorMode; };
  uint32_t getSensorMode() { return mSensorMode; };

  uint32_t mDeviceId;
  std::vector<float> mSourceClipRect;
  std::vector<uint32_t> mStreamResolution;
  std::vector<uint32_t> mVideoConverterResolution;
  std::vector<uint64_t> mFrameDurationRange;
  uint32_t mSensorMode;

  std::vector<uint32_t> getOutputShape() {
    return { mVideoConverterResolution[HEIGHT_IDX], mVideoConverterResolution[WIDTH_IDX], getNumChannels() };
  };

  size_t getOutputSizeBytes();
  uint32_t getNumChannels();
};

ArgusCameraConfig DEFAULT_DEVKIT_CONFIG()
{
    ArgusCameraConfig c;
    c.mDeviceId = 0;
    c.mSourceClipRect = { 0.0, 0.0, 1.0, 1.0 };
    c.mStreamResolution = { 640, 480 };
    c.mVideoConverterResolution = { 640, 480 };
    c.mFrameDurationRange = { ONE_SECOND_NANOS / 30, ONE_SECOND_NANOS / 30 }; // 30fps
    c.mSensorMode = 0;
    return c;
}

class IArgusCamera
{
public:
  static IArgusCamera *createArgusCamera(const ArgusCameraConfig &config, int *info=nullptr);
  virtual ~IArgusCamera() {};
  virtual int read(uint8_t *data) = 0;
};

