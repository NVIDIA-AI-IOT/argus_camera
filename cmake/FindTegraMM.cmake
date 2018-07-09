set(TegraMM_ROOT $ENV{HOME}/tegra_multimedia_api)

set(TegraMM_FOUND FALSE)

if(EXISTS ${TegraMM_ROOT})
  # set packages
  set(TegraMM_INCLUDE_DIRS ${TegraMM_ROOT}/include ${TegraMM_ROOT}/include/libjpeg-8b /usr/include/libdrm)
  set(TegraMM_INCLUDES ${TegraMM_INCLUDE_DIRS})
  set(TegraMM_LIBRARY_DIRS /usr/lib/aarch64-linux-gnu/tegra /usr/lib/aarch64-linux-gnu)
  set(TegraMM_LIBRARIES argus nvjpeg drm nvbuf_utils nvosd EGL v4l2 GLESv2 X11 pthread)
  file(GLOB TegraMM_COMMON_SOURCES ${TegraMM_ROOT}/samples/common/classes/*.cpp)

  include_directories(${TegraMM_INCLUDE_DIRS})
  link_directories(${TegraMM_LIBRARY_DIRS})
  #add_library(tegra_mm SHARED ${TegraMM_COMMON_SOURCES})
  #target_link_libraries(tegra_mm ${TegraMM_LIBRARIES})

  #  install(TARGETS tegra_mm
  #    RUNTIME DESTINATION bin
  #    LIBRARY DESTINATION lib
  #    ARCHIVE DESTINATION lib/static)


  #set(TegraMM_LIBRARIES ${TegraMM_LIBRARIES} tegra_mm)

  set(TegraMM_FOUND TRUE)
endif()

