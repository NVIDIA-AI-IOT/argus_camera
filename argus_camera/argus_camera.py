import numpy as np
from .cpp import *
from past.builtins import long

class ArgusCamera:

    def __init__(self, 
        device_id=0, 
        stream_resolution=(640, 480), 
        video_converter_resolution=(640, 480),
        frame_duration_range=(long(1e9//30), long(1e9//30)),
        exposure_time_range=(long(0),long(999999999)),
        source_clip_rect=(0.0, 0.0, 1.0, 1.0),
        gain_range=(0.,0.),
        sensor_mode=0):

        self.device_id = device_id

        self.video_converter_resolution = video_converter_resolution

        self.config = DEFAULT_DEVKIT_CONFIG()
        self.config.setDeviceId(device_id)
        self.config.setStreamResolution(stream_resolution)
        self.config.setVideoConverterResolution(video_converter_resolution)
        self.config.setFrameDurationRange(frame_duration_range)
        self.config.setExposureTimeRange(exposure_time_range)
        self.config.setGainRange(gain_range)
        self.config.setSourceClipRect(source_clip_rect)
        self.config.setExposureCompensation(0)
        self.config.setAeLock(True)
        #self.config.setSensorMode(sensor_mode)
        self.channels = 4

        self.camera = IArgusCamera_createArgusCamera(self.config)

    def read(self):
        image = np.empty(list(self.video_converter_resolution)[::-1] + [self.channels], np.uint8)
        self.camera.read(image.ctypes.data)
        return image
    
    def get_gain_range(self):
        return self.config.getGainRange()
        
