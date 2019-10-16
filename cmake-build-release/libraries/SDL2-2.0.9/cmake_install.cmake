# Install script for directory: /Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/libraries/SDL2-2.0.9/libSDL2.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/libraries/SDL2-2.0.9/libSDL2-2.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2-2.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2-2.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2-2.0.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/libraries/SDL2-2.0.9/libSDL2main.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/SDL2.framework/Resources/SDL2Targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/SDL2.framework/Resources/SDL2Targets.cmake"
         "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/libraries/SDL2-2.0.9/CMakeFiles/Export/SDL2.framework/Resources/SDL2Targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/SDL2.framework/Resources/SDL2Targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/SDL2.framework/Resources/SDL2Targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SDL2.framework/Resources" TYPE FILE FILES "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/libraries/SDL2-2.0.9/CMakeFiles/Export/SDL2.framework/Resources/SDL2Targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SDL2.framework/Resources" TYPE FILE FILES "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/libraries/SDL2-2.0.9/CMakeFiles/Export/SDL2.framework/Resources/SDL2Targets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SDL2.framework/Resources" TYPE FILE FILES
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/SDL2Config.cmake"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/SDL2ConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_assert.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_atomic.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_audio.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_bits.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_blendmode.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_clipboard.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_config_android.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_config_iphoneos.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_config_macosx.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_config_minimal.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_config_os2.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_config_pandora.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_config_psp.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_config_windows.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_config_winrt.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_config_wiz.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_copying.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_cpuinfo.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_egl.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_endian.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_error.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_events.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_filesystem.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_gamecontroller.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_gesture.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_haptic.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_hints.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_joystick.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_keyboard.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_keycode.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_loadso.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_log.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_main.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_messagebox.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_mouse.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_mutex.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_name.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_opengl.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_opengl_glext.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_opengles.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_opengles2.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_opengles2_gl2.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_opengles2_gl2ext.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_opengles2_gl2platform.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_opengles2_khrplatform.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_pixels.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_platform.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_power.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_quit.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_rect.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_render.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_revision.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_rwops.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_scancode.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_sensor.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_shape.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_stdinc.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_surface.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_system.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_syswm.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_assert.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_common.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_compare.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_crc32.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_font.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_fuzzer.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_harness.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_images.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_log.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_md5.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_memory.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_test_random.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_thread.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_timer.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_touch.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_types.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_version.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_video.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/SDL_vulkan.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/begin_code.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/include/close_code.h"
    "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/libraries/SDL2-2.0.9/include/SDL_config.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  
          execute_process(COMMAND /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E create_symlink
            "libSDL2-2.0.dylib" "libSDL2.dylib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/libraries/SDL2-2.0.9/libSDL2.dylib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/libraries/SDL2-2.0.9/sdl2.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/cmake-build-release/libraries/SDL2-2.0.9/sdl2-config")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/share/aclocal/sdl2.m4")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/share/aclocal" TYPE FILE FILES "/Users/Kleinman/Documents/Programming/C:C++/DH2323-Computer_Graphics_and_Interaction/libraries/SDL2-2.0.9/sdl2.m4")
endif()

