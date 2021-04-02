# Install script for directory: E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
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
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/lib/Debug/assimp-vc141-mtd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/lib/Release/assimp-vc141-mt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/lib/MinSizeRel/assimp-vc141-mt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/lib/RelWithDebInfo/assimp-vc141-mt.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/bin/Debug/assimp-vc141-mtd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/bin/Release/assimp-vc141-mt.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/bin/MinSizeRel/assimp-vc141-mt.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/bin/RelWithDebInfo/assimp-vc141-mt.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/anim.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/aabb.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/ai_assert.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/camera.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/color4.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/color4.inl"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/config.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/defs.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Defines.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/cfileio.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/light.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/material.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/material.inl"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/matrix3x3.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/matrix3x3.inl"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/matrix4x4.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/matrix4x4.inl"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/mesh.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/pbrmaterial.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/postprocess.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/quaternion.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/quaternion.inl"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/scene.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/metadata.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/texture.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/types.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/vector2.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/vector2.inl"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/vector3.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/vector3.inl"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/version.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/cimport.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/importerdesc.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Importer.hpp"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/DefaultLogger.hpp"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/ProgressHandler.hpp"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/IOStream.hpp"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/IOSystem.hpp"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Logger.hpp"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/LogStream.hpp"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/NullLogger.hpp"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/cexport.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Exporter.hpp"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/DefaultIOStream.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/DefaultIOSystem.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/ZipArchiveIOSystem.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/SceneCombiner.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/fast_atof.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/qnan.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/BaseImporter.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Hash.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/MemoryIOWrapper.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/ParsingUtils.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/StreamReader.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/StreamWriter.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/StringComparison.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/StringUtils.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/SGSpatialSort.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/GenericProperty.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/SpatialSort.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/SkeletonMeshBuilder.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/SmoothingGroups.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/SmoothingGroups.inl"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/StandardShapes.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/RemoveComments.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Subdivision.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Vertex.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/LineSplitter.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/TinyFormatter.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Profiler.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/LogAux.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Bitmap.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/XMLTools.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/IOStreamBuffer.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/CreateAnimMesh.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/irrXMLWrapper.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/BlobIOSystem.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/MathFunctions.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Macros.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Exceptional.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/ByteSwapper.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Compiler/pushpack1.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Compiler/poppack1.h"
    "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/Debug/assimp-vc141-mtd.pdb")
  endif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "E:/Git/QHEngine/Project_Win32/External/assimp-5.0.1/code/RelWithDebInfo/assimp-vc141-mt.pdb")
  endif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
endif()

