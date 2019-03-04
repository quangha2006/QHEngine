# Install script for directory: C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/examples

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/BULLET_PHYSICS")
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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/Build/examples/HelloWorld/cmake_install.cmake")
  include("C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/Build/examples/BasicDemo/cmake_install.cmake")
  include("C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/Build/examples/ExampleBrowser/cmake_install.cmake")
  include("C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/Build/examples/RobotSimulator/cmake_install.cmake")
  include("C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/Build/examples/SharedMemory/cmake_install.cmake")
  include("C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/Build/examples/ThirdPartyLibs/Gwen/cmake_install.cmake")
  include("C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/Build/examples/ThirdPartyLibs/BussIK/cmake_install.cmake")
  include("C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/Build/examples/ThirdPartyLibs/clsocket/cmake_install.cmake")
  include("C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/Build/examples/OpenGLWindow/cmake_install.cmake")
  include("C:/Users/11122/Desktop/QHEngine/Project_Win32/External/bullet3-2.88/Build/examples/TwoJoint/cmake_install.cmake")

endif()

