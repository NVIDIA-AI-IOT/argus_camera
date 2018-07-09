#include "ArgusCamera.hpp"

#include "Argus/Argus.h"
#include "EGLStream/NV/ImageNativeBuffer.h"
#include "nvbuf_utils.h"
#include "NvBuffer.h"
#include "EGLStream/EGLStream.h"
#include "NvVideoConverter.h"


#define ROUND_UP_EVEN(x) 2 * ((x + 1) / 2)

using namespace Argus;
using namespace std;

class ArgusCamera : public IArgusCamera
{
public:
  static ArgusCamera *createArgusCamera(const ArgusCameraConfig &config, int *info=nullptr);
  ~ArgusCamera();
  int read(uint8_t *data) override;

private:
  ArgusCameraConfig mConfig;

  Argus::UniqueObj<Argus::CaptureSession> mCaptureSession;
  Argus::UniqueObj<Argus::OutputStream> mStream;
  Argus::UniqueObj<EGLStream::FrameConsumer> mFrameConsumer;
  NvVideoConverter *mVideoConverter;
};

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
  Argus::Status status;

  std::unique_ptr<ArgusCamera> camera;
  camera.reset(new ArgusCamera());

  camera->mConfig = config;

  // setup video converter
  camera->mVideoConverter = NvVideoConverter::createVideoConverter("videoConverter");
  if (!camera->mVideoConverter) {
    if (info) {
      *info = 1; // failed to create video converter
    }
    return nullptr;
  }

  // get camera provider
  auto cameraProvider = UniqueObj<CameraProvider>(CameraProvider::create(NULL));
  auto iCameraProvider = interface_cast<ICameraProvider>(cameraProvider);
  if (!iCameraProvider) {
    if (info) {
      *info = 2; // failed to get camera provider
    }
    return nullptr;
  }

  // get camera device
  vector<CameraDevice*> devices;
  status = iCameraProvider->getCameraDevices(&devices);
  if (Argus::STATUS_OK != status) {
    if (info) {
      *info = 3; // failed to get list of camera devices
    }
    return nullptr;
  }
  if (camera->mConfig.mDeviceId >= devices.size()) {
    if (info) {
      *info = 4; // camera device out of range
    }
    return nullptr;
  }
  auto cameraDevice = devices[camera->mConfig.mDeviceId];

  // create capture session
  camera->mCaptureSession = UniqueObj<CaptureSession>(iCameraProvider->createCaptureSession(cameraDevice));
  auto iCaptureSession = interface_cast<ICaptureSession>(camera->mCaptureSession);
  if (!iCaptureSession) {
    if (info) {
      *info = 5; // failed to create capture session
    }
    return nullptr;
  }

  // create stream settings
  auto streamSettings = UniqueObj<OutputStreamSettings>(iCaptureSession->createOutputStreamSettings(NULL));
  auto iOutputStreamSettings = interface_cast<IOutputStreamSettings>(streamSettings);
  if (!iOutputStreamSettings) {
    if (info) {
      *info = 6; // failed to create stream settings
    }
    return nullptr;
  }

  // set stream pixel format and resolution
  iOutputStreamSettings->setPixelFormat(Argus::PIXEL_FMT_YCbCr_420_888);
  auto evenResolution = Argus::Size2D<uint32_t>(
    ROUND_UP_EVEN(camera->mConfig.mStreamResolution[WIDTH_IDX]),
    ROUND_UP_EVEN(camera->mConfig.mStreamResolution[HEIGHT_IDX])
  );
  iOutputStreamSettings->setResolution(evenResolution);
  iOutputStreamSettings->setMetadataEnable(false);

  // create stream
  camera->mStream = UniqueObj<OutputStream>(iCaptureSession->createOutputStream(streamSettings.get(), &status));
  auto iStream = interface_cast<IStream>(camera->mStream);
  if (!iStream) {
    if (info) {
      *info = 7; // failed to create stream
    }
    return nullptr;
  }

  // create frame consumer
  camera->mFrameConsumer = UniqueObj<EGLStream::FrameConsumer>(EGLStream::FrameConsumer::create(camera->mStream.get()));
  auto iFrameConsumer = interface_cast<EGLStream::IFrameConsumer>(camera->mFrameConsumer);
  if (!iFrameConsumer) {
    if (info) {
      *info = 8; // failed to create frame consumer
    }
    return nullptr;
  }

  // start repeating capture request
  auto request = UniqueObj<Request>(iCaptureSession->createRequest());
  auto iRequest = interface_cast<IRequest>(request);
  if (!iRequest) {
    if (info) {
      *info = 9; // failed to create request
    }
    return nullptr;
  }

  // enable output stream
  iRequest->enableOutputStream(camera->mStream.get());

  // configure source settings in request
  // 1. set sensor mode
  auto iCameraProperties = interface_cast<ICameraProperties>(cameraDevice);
  vector<SensorMode*> sensorModes;
  status = iCameraProperties->getAllSensorModes(&sensorModes);
  if (Argus::STATUS_OK != status ||
      camera->mConfig.getSensorMode() >= sensorModes.size()) {
    if (info) {
      *info = 15;
    }
    return nullptr;
  }
  auto iSourceSettings = interface_cast<ISourceSettings>(iRequest->getSourceSettings());
  status = iSourceSettings->setSensorMode(sensorModes[camera->mConfig.getSensorMode()]);
  if (Argus::STATUS_OK != status) {
    if (info) {
      *info = 18;
    }
    return nullptr;
  }

  // 2. set frame duration
  status = iSourceSettings->setFrameDurationRange(Argus::Range<uint64_t>(
    camera->mConfig.getFrameDurationRange()[0],
    camera->mConfig.getFrameDurationRange()[1]
  ));
  if (Argus::STATUS_OK != status) {
    if (info) {
      *info = 19;
    }
    return nullptr;
  }
  
  // configure stream settings
  auto iStreamSettings = interface_cast<IStreamSettings>(iRequest->getStreamSettings(camera->mStream.get()));
  if (!iStreamSettings) {
    if (info) {
      *info = 16;
    }
    return nullptr;
  }
  // set stream resolution
  status = iStreamSettings->setSourceClipRect(Argus::Rectangle<float>(
    camera->mConfig.getSourceClipRect()[0],
    camera->mConfig.getSourceClipRect()[1],
    camera->mConfig.getSourceClipRect()[2],
    camera->mConfig.getSourceClipRect()[3]
  ));
  if (Argus::STATUS_OK != status) {
    if (info) {
      *info = 17;
    }
    return nullptr;
  }

  // start repeating capture request
  status = iCaptureSession->repeat(request.get());
  if (Argus::STATUS_OK != status) {
    if (info) {
      *info = 10; // failed to start repeating capture request
    }
    return nullptr;
  }

  // connect stream
  iStream->waitUntilConnected();

  // setup video converter
  uint32_t width = camera->mConfig.mVideoConverterResolution[WIDTH_IDX];
  uint32_t height = camera->mConfig.mVideoConverterResolution[HEIGHT_IDX];
  camera->mVideoConverter->setOutputPlaneFormat(V4L2_PIX_FMT_YUV420M, width, height, V4L2_NV_BUFFER_LAYOUT_PITCH);
  camera->mVideoConverter->setCapturePlaneFormat(V4L2_PIX_FMT_ABGR32, width, height, V4L2_NV_BUFFER_LAYOUT_PITCH);

  if (camera->mVideoConverter->output_plane.setupPlane(V4L2_MEMORY_DMABUF, 1, false, false) < 0) {
    if (info) {
      *info = 11;
    }
    return nullptr;
  }
  if (camera->mVideoConverter->capture_plane.setupPlane(V4L2_MEMORY_MMAP, 1, true, false) < 0) {
    if (info) {
      *info = 12;
    }
    return nullptr;
  }
  if (camera->mVideoConverter->output_plane.setStreamStatus(true) < 0) {
    if (info) {
      *info = 13;
    }
    return nullptr;
  }
  if (camera->mVideoConverter->capture_plane.setStreamStatus(true) < 0) {
    if (info) {
      *info = 14;
    }
    return nullptr;
  }

  if (info) {
    *info = 0;
  }

  return camera.release();
}

ArgusCamera::~ArgusCamera()
{
  auto iCaptureSession = interface_cast<ICaptureSession>(mCaptureSession);
  if (iCaptureSession) {
    iCaptureSession->stopRepeat();
    iCaptureSession->waitForIdle();
  }

  auto iStream = interface_cast<IStream>(mStream);
  if (iStream) {
    iStream->disconnect();
  }

  if (mVideoConverter) {
    mVideoConverter->capture_plane.deinitPlane();
    mVideoConverter->output_plane.deinitPlane();
    mVideoConverter->capture_plane.setStreamStatus(false);
    mVideoConverter->output_plane.setStreamStatus(false);
  }

  delete mVideoConverter;
}

int ArgusCamera::read(uint8_t *data)
{
  Argus::Status status;

  auto iStream = interface_cast<IStream>(mStream);
  if (!iStream) {
    return 1; // failed to create stream interface
  }

  auto iFrameConsumer = interface_cast<EGLStream::IFrameConsumer>(mFrameConsumer);
  if (!iFrameConsumer) {
    return 2; // failed to create frame consumer
  }

  auto frame = UniqueObj<EGLStream::Frame>(iFrameConsumer->acquireFrame());
  auto iFrame = interface_cast<EGLStream::IFrame>(frame);
  if (!iFrame) {
    return 3; // failed to get frame
  }

  auto image = iFrame->getImage();
  auto iImage = interface_cast<EGLStream::IImage>(image);
  auto iImage2D = interface_cast<EGLStream::IImage2D>(image);
  if (!iImage) {
    return 4;
  }
  if (!iImage2D) {
    return 5;
  }

  // copy to native buffer converting resolution, color format, and layout
  int fd = -1;
  auto iNativeBuffer = interface_cast<EGLStream::NV::IImageNativeBuffer>(image);
  auto resolution = Argus::Size2D<uint32_t>(
      mConfig.mVideoConverterResolution[WIDTH_IDX],
      mConfig.mVideoConverterResolution[HEIGHT_IDX]
  );
  fd = iNativeBuffer->createNvBuffer(resolution, 
    NvBufferColorFormat::NvBufferColorFormat_YUV420,
    NvBufferLayout_Pitch, &status);
  if (Argus::STATUS_OK != status) {
    return 6; // failed to create native buffer
  }

  NvBufferParams nativeBufferParams;
  NvBufferGetParams(fd, &nativeBufferParams);

  struct v4l2_buffer v4l2_buf;
  struct v4l2_plane planes[3];

  // enqueue capture plane
  memset(&v4l2_buf, 0, sizeof(v4l2_buf));
  memset(planes, 0, 3 * sizeof(struct v4l2_plane));
  v4l2_buf.index = 0;
  v4l2_buf.m.planes = planes;
  if (mVideoConverter->capture_plane.qBuffer(v4l2_buf, NULL) < 0) {
    return 7; // failed to queue capture plane
  }

  // enqueue output plane
  memset(&v4l2_buf, 0, sizeof(v4l2_buf));
  memset(planes, 0, 3 * sizeof(struct v4l2_plane));
  v4l2_buf.index = 0;
  v4l2_buf.m.planes = planes;
  planes[0].m.fd = fd;
  planes[0].bytesused = 1234;
  if (mVideoConverter->output_plane.qBuffer(v4l2_buf, NULL) < 0) {
    return 8; // failed to queue output plane
  }

  NvBuffer *nativeCaptureBuffer;
  if (mVideoConverter->capture_plane.dqBuffer(v4l2_buf, &nativeCaptureBuffer, NULL, 1) < 0) {
    return 9; // failed to deqeue capture plane
  }
  mVideoConverter->output_plane.dqBuffer(v4l2_buf, NULL, NULL, 1);

  // copy to destination (pitched -> block linear)
  auto plane = nativeCaptureBuffer->planes[0];
  uint32_t rowBytes = plane.fmt.bytesperpixel * plane.fmt.width;
  uint8_t *srcData = plane.data;
  for (uint32_t i = 0; i < plane.fmt.height; i++) {
    memcpy(data, srcData, rowBytes);
    data += rowBytes;
    srcData += plane.fmt.stride;
  }

  NvBufferDestroy(fd);

  return 0;
}

IArgusCamera * IArgusCamera::createArgusCamera(const ArgusCameraConfig &config, int *info)
{
  return (IArgusCamera*) ArgusCamera::createArgusCamera(config, info);
}
