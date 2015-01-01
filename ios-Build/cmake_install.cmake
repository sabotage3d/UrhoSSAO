# Install script for directory: /Users/sabotage3d/DEV/Urho3D-1.32/Source

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin" TYPE FILE PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/Users/sabotage3d/DEV/Urho3D-1.32/Bin/Editor.sh"
    "/Users/sabotage3d/DEV/Urho3D-1.32/Bin/NinjaSnowWar.sh"
    "/Users/sabotage3d/DEV/Urho3D-1.32/Bin/UpdateDocument.sh"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin" TYPE DIRECTORY PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ DIR_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/Users/sabotage3d/DEV/Urho3D-1.32/Bin/CoreData"
    "/Users/sabotage3d/DEV/Urho3D-1.32/Bin/Data"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Urho3D/CMake" TYPE DIRECTORY PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ DIR_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "/Users/sabotage3d/DEV/Urho3D-1.32/Source/CMake/")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Urho3D/Scripts" TYPE FILE PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/Users/sabotage3d/DEV/Urho3D-1.32/.bash_helpers.sh"
    "/Users/sabotage3d/DEV/Urho3D-1.32/cmake_clean.sh"
    "/Users/sabotage3d/DEV/Urho3D-1.32/cmake_codeblocks.sh"
    "/Users/sabotage3d/DEV/Urho3D-1.32/cmake_eclipse.sh"
    "/Users/sabotage3d/DEV/Urho3D-1.32/cmake_gcc.sh"
    "/Users/sabotage3d/DEV/Urho3D-1.32/cmake_ios.sh"
    "/Users/sabotage3d/DEV/Urho3D-1.32/cmake_iosSamples.sh"
    "/Users/sabotage3d/DEV/Urho3D-1.32/cmake_macosx.sh"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/FreeType/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/JO/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/LZ4/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/PugiXml/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/SDL/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/StanHull/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/STB/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/AngelScript/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/Civetweb/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/kNet/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/Detour/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/Recast/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/Bullet/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/ThirdParty/Box2D/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/Engine/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/Tools/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/Samples/cmake_install.cmake")
  INCLUDE("/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/Docs/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/Users/sabotage3d/DEV/Urho3D-1.32/ios-Build/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
