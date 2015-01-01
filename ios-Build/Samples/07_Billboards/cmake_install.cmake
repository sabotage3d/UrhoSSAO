# Install script for directory: /Users/sabotage3d/DEV/Urho3D-1.32/Source/Samples/07_Billboards

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
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin" TYPE DIRECTORY FILES "/Users/sabotage3d/DEV/Urho3D-1.32/ios-Bin/07_Billboards.app" USE_SOURCE_PERMISSIONS)
    IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards")
      execute_process(COMMAND /usr/bin/install_name_tool
        -add_rpath "@loader_path/../../../lib/ios/Urho3D"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards")
      execute_process(COMMAND /usr/bin/install_name_tool
        -add_rpath "/usr/local/lib/ios/Urho3D"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards")
    ENDIF()
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin" TYPE DIRECTORY FILES "/Users/sabotage3d/DEV/Urho3D-1.32/ios-Bin/07_Billboards.app" USE_SOURCE_PERMISSIONS)
    IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards")
      execute_process(COMMAND /usr/bin/install_name_tool
        -add_rpath "@loader_path/../../../lib/ios/Urho3D"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards")
      execute_process(COMMAND /usr/bin/install_name_tool
        -add_rpath "/usr/local/lib/ios/Urho3D"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards")
    ENDIF()
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin" TYPE DIRECTORY FILES "/Users/sabotage3d/DEV/Urho3D-1.32/ios-Bin/07_Billboards.app" USE_SOURCE_PERMISSIONS)
    IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards")
      execute_process(COMMAND /usr/bin/install_name_tool
        -add_rpath "@loader_path/../../../lib/ios/Urho3D"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards")
      execute_process(COMMAND /usr/bin/install_name_tool
        -add_rpath "/usr/local/lib/ios/Urho3D"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/Urho3D/Bin/07_Billboards.app/Contents/MacOS/07_Billboards")
    ENDIF()
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

