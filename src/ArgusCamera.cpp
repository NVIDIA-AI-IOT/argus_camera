#include "ArgusCamera.h"

using namespace Argus;
using namespace std;

uint32_t ArgusCameraConfig::getNumChannels()
{
  return 4; // RGBA
}

size_t ArgusCameraConfig::getOutputSizeBytes()
{
  return sizeof(uint8_t) * getNumChannels() * mVideoConverterResolution[0] * mVideoConverterResolution[1];
}

ArgusCamera *ArgusCamera::createArgusCamera(const ArgusCameraConfig &config, int *info)
{
  return nullptr;
}

ArgusCamera::~ArgusCamera()
{
  auto iCaptureSession = interface_cast<ICaptureSession>(mCaptureSession);
  if (iCaptureSession) {
    iCaptureSession->stopRepeat();
    iCaptureSession->waitForIdle();
  }

  auto iStream = interface_cast<IStream>(mStream);
  iStream->disconnect();

  if (mVideoConverter) {
    mVideoConverter->capture_plane.deinitPlane();
    mVideoConverter->output_plane.deinitPlane();
    mVideoConverter->capture_plane.setStreamStatus(false);
    mVideoConverter->output_plane.setStreamStatus(false);
  }

  delete mVideoConverter;
}

int read(uint8_t *data)
{
  return 0;
}
