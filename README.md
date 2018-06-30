Argus Camera interface for Jetson
=================================

This project provides a simple Python / C++ interface to CSI cameras on the Jetson using
the Tegra Multimedia API and the LibArgus.  By using the LibArgus and the Tegra
Multimedia API we take advantage of hardware accelerated image processing features onboard
the Jetson.  All images are returned in RGBA format for easy processing.

This may not work for all CSI cameras, but has been tested for those in the table below. 

Cameras
-------

| Camera Model         | Sensor Modes | Frame Rates |
|----------------------|--------------|-------------|
| Jetson DevKit Camera |  TBD         |  TBD        |

Python Usage
------------

Here we show a simple example to acquire an image from the devkit camera.

```
import numpy as np
import argus_camera

config = argus_camera.ArgusCameraConfig_createDefaultDevkitConfig()
camera = argus_camera.ArgusCamera_createArgusCamera(config)

image = np.empty(config.getOutputShape(), np.uint8)

camera.read(image.ctypes.data)
```

The configuration can be modified before creating the argus camera by calling several methods.

| Config Method       | Arg Type | Description |
|---------------------|-------------|----------|
| setDeviceId         | integer | The camera device index.  |
| setStreamResolution | tuple or list of integers | The resolution (width, height) in pixels of the intermediate image.  |
| setVideoConverterResolution | tuple or list of integers | The resolution (width, height) in pixels of the final output image (after resizing).    |
| setFrameDurationRange | tuple or list of integers | The target frame duration (min, max) in nanoseconds. |
| setSourceClipRect     | tuple or list of floats | The region to crop the source image before resizing (x0, y0, x1, y1) in normalized image coordinates [0.0, 1.0] |
| setSensorMode         | integer | The sensor mode (relevant for frame rate). |

For example, to set the resolution to 300x300 pixels.

```python
config.setStreamResolution((300, 300))
config.setVideoConverterResolution((300, 300))
```

C++ Usage
---------

```cpp
#include "ArgusCamera.h"

int main() {
  
  ArgusCameraConfig config = ArgusCameraConfig::createDefaultCameraConfig();
  ArgusCamera *camera = ArgusCamera::createArgusCamera(config);
  
  unsigned char *data;
  data = (unsigned char *) malloc(config.getOutputSizeBytes());
  
  camera.read(data);
  
  free(data);
  delete camera;
  return 0;
}
  
```

Similar to the Python API we can modify the config before camera creation.

```cpp
config.setStreamResolution({300, 300});
config.setVideoConverterResolution({300, 300});
```