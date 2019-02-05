#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#define WIDTH_IDX 0
#define HEIGHT_IDX 1
#define ONE_SECOND_NANOS 1000000000
#define EXPOSURE_DEFAULT_LOW 0 
#define EXPOSURE_DEFAULT_HIGH 9999999999999

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

  void setGainRange(std::vector<float> gainRange) { mGainRange = gainRange; };
  std::vector<float> getGainRange() { return mGainRange; };

  void setExposureCompensation(float ExposureCompensation) { mExposureCompensation = ExposureCompensation; };
  float getExposureCompensation() { return mExposureCompensation; };

  void setAeLock(bool AeLock) { mAeLock = AeLock; };
  bool getAeLock() { return mAeLock; };


  void setExposureTimeRange(std::vector<uint64_t> exposureTimeRange) { mExposureTimeRange = exposureTimeRange; };
  std::vector<uint64_t> getExposureTimeRange() { return mExposureTimeRange; };

  void setSensorMode(uint32_t sensorMode) { mSensorMode = sensorMode; };
  uint32_t getSensorMode() { return mSensorMode; };

  uint32_t mDeviceId;
  std::vector<float> mSourceClipRect;
  std::vector<uint32_t> mStreamResolution;
  std::vector<uint32_t> mVideoConverterResolution;
  std::vector<uint64_t> mFrameDurationRange;
  std::vector<uint64_t> mExposureTimeRange;
  std::vector<float> mGainRange;
  bool mAeLock;
  float mExposureCompensation;
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
    c.mExposureTimeRange = { EXPOSURE_DEFAULT_LOW,EXPOSURE_DEFAULT_HIGH };
    c.mGainRange = {0.0, 300.0};
    c.mSensorMode = 0;
    c.mExposureCompensation = 0.;
    c.mAeLock = false;
    return c;
}

class IArgusCamera
{
public:
  static IArgusCamera *createArgusCamera(const ArgusCameraConfig &config, int *info=nullptr);
  virtual ~IArgusCamera() {};
  virtual int read(uint8_t *data) = 0;
};

