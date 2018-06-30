import os
from setuptools import find_packages, setup, Extension
import subprocess

this_dir = os.path.dirname(os.path.realpath(__file__))
swig_file = os.path.join(this_dir, 'argus_camera/cpp.i')
argus_include_dir = os.path.join(os.environ['HOME'], 'tegra_multimedia_api/include')

subprocess.call(['swig', '-DSWIGWORDSIZE64', '-c++', '-python', '-interface', '_argus_camera_cpp', swig_file])

argus_camera_cpp = Extension('_argus_camera_cpp',
        sources=['argus_camera/cpp_wrap.cxx'],
        include_dirs=[argus_include_dir],
        libraries=['argus_camera'],
        library_dirs=['/usr/local/lib'],
        runtime_library_dirs=['/usr/local/lib'],
        extra_compile_args=['-std=c++11'],
)

setup(
        name='argus_camera',
        version='0.0',
        description='Camera interface for NVIDIA Jetson',
        author='',
        author_email='',
        url='https://github.com/NVIDIA-Jetson/argus_camera',
        packages=find_packages(),
        ext_modules=[argus_camera_cpp]
)
