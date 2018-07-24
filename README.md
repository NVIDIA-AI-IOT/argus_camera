Argus Camera interface for Jetson
=================================

This project provides a simple Python / C++ interface to CSI cameras on the Jetson using
the Tegra Multimedia API and the LibArgus.  All images are returned in RGBA format for easy processing.

Please note that this project experimental and is provided as a convenience for prototyping.  This may not work for all CSI cameras, but has been tested for those in the table below.  

Cameras
-------

| Camera Model         | Sensor Modes | Frame Rates |
|----------------------|--------------|-------------|
| Jetson DevKit Camera |  TBD         |  TBD        |

Setup
-----

Ensure that you have flashed your Jetson to include the Tegra Multimedia API. Then, simply clone this repository and enter the following into a terminal.

```bash
sudo apt-get install cmake python-pip swig
cd argus_camera
mkdir build
cd build
cmake ..
make -j4
sudo make install
cd ..
sudo python setup.py install
```

If you are using Python 3 you replace the last command with

```bash
sudo python3 setup.py install
```

Python Usage
------------

Here we show a simple example to acquire an image from the devkit camera.

```python
from argus_camera import ArgusCamera

camera = ArgusCamera()
image = camera.read()
```

The camera can be initialized with different configurations.  For example,
we can set the output resolution to 300x300 pixels and crop the input to use
the center of the image.

```python
camera = ArgusCamera(
  stream_resolution=(300, 300),
  video_converter_resolution=(300, 300), 
  source_clip_rect=(0.25, 0.25, 0.75, 0.75),
)
```

The video converter resolution corresponds to the output resolution.  The stream resolution
is the resolution the stream is set to before image resizing by the video converter.  For
simplicity, set both to the same value.

For more configuration options type ``help(ArgusCamera)``.
