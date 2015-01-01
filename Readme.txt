Urho3D - cross-platform 2D and 3D game engine
---------------------------------------------

http://urho3d.github.io/

Licensed under the MIT license, see License.txt for details.


Credits
-------

Urho3D development, contributions and bugfixes by:
- Lasse Öörni (loorni@gmail.com, AgentC at GameDev.net)
- Wei Tjong Yao
- Aster Jian
- Colin Barrett
- Erik Beran
- Danny Boisvert
- Carlo Carollo
- Pete Chown
- Sebastian Delatorre (primitivewaste)
- Josh Engebretson
- Chris Friesen
- Alex Fuller
- Mika Heinonen
- Graham King
- Jason Kinzer
- Gunnar Kriik
- Ali Kämäräinen
- Pete Leigh
- Jonne Nauha
- Paul Noome
- David Palacios
- Alex Parlett
- Jordan Patterson
- Vladimir Pobedinsky
- Nick Royer
- Miika Santala
- Joshua Tippetts
- Daniel Wiberg
- Steven Zhang
- AGreatFish
- Firegorilla
- Magic.Lixin
- Mike3D
- OvermindDL1
- andmar1x
- amadeus_osa
- atship
- att
- celeron55
- hdunderscore
- mightyCelu
- nemerle
- ninjastone
- rasteron
- reattiva
- rifai
- skaiware
- szamq
- thebluefish

Urho3D is greatly inspired by OGRE (http://www.ogre3d.org) and Horde3D
(http://www.horde3d.org). Additional inspiration & research used:
- Rectangle packing by Jukka Jylänki (clb)
  http://clb.demon.fi/projects/rectangle-bin-packing
- Tangent generation from Terathon
  http://www.terathon.com/code/tangent.html
- Fast, Minimum Storage Ray/Triangle Intersection by Möller & Trumbore
  http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
- Linear-Speed Vertex Cache Optimisation by Tom Forsyth
  http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html
- Software rasterization of triangles based on Chris Hecker's
  Perspective Texture Mapping series in the Game Developer magazine
  http://chrishecker.com/Miscellaneous_Technical_Articles
- Networked Physics by Glenn Fiedler
  http://gafferongames.com/game-physics/networked-physics/
- Euler Angle Formulas by David Eberly
  http://www.geometrictools.com/Documentation/EulerAngles.pdf
- Red Black Trees by Julienne Walker
  http://eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx
- Comparison of several sorting algorithms by Juha Nieminen
  http://warp.povusers.org/SortComparison/

Urho3D uses the following third-party libraries:
- AngelScript 2.29.1 (http://www.angelcode.com/angelscript/)
- Box2D 2.3.0 (http://box2d.org/)
- Bullet 2.82 (http://www.bulletphysics.org/)
- Civetweb (http://sourceforge.net/projects/civetweb/)
- FreeType 2.5.0 (http://www.freetype.org/)
- GLEW 1.9.0 (http://glew.sourceforge.net/)
- jo_jpeg 1.52 (http://www.jonolick.com/uploads/7/9/2/1/7921194/jo_jpeg.cpp)
- kNet (https://github.com/juj/kNet)
- libcpuid 0.2.0 (http://libcpuid.sourceforge.net/)
- Lua 5.1 (http://www.lua.org)
- LuaJIT 2.0.3 (http://www.luajit.org)
- LZ4 (http://code.google.com/p/lz4/)
- MojoShader (http://icculus.org/mojoshader/)
- Open Asset Import Library (http://assimp.sourceforge.net/)
- pugixml 1.0 (http://pugixml.org/)
- rapidjson 0.11 (https://code.google.com/p/rapidjson/)
- Recast/Detour (https://github.com/memononen/recastnavigation/)
- SDL 2.0.3 (http://www.libsdl.org/)
- StanHull (http://codesuppository.blogspot.com/2006/03/
  john-ratcliffs-code-suppository-blog.html)
- stb_image 1.29 (http://nothings.org/)
- stb_vorbis 0.99996 (http://nothings.org/)
- tolua++ 1.0.93 (http://www.codenix.com/~tolua)

DXT / ETC1 / PVRTC decompression code based on the Squish library and the Oolong
Engine.
Jack and mushroom models from the realXtend project. (http://www.realxtend.org)
Ninja model and terrain, water, smoke, flare and status bar textures from OGRE.
BlueHighway font from Larabie Fonts.
Anonymous Pro font by Mark Simonson.
NinjaSnowWar sounds by Veli-Pekka Tätilä.


Documentation
-------------

Urho3D classes have been sparsely documented using Doxygen notation. To
generate documentation into the "Docs" subdirectory, open the Doxyfile in the
"Docs" subdirectory with doxywizard and click "Run doxygen" from the "Run" tab.
Get Doxygen from http://www.doxygen.org & Graphviz from http://www.graphviz.org.
See section "Documentation build" below on how to automate documentation
generation as part of the build process.

The documentation is also available online at
http://urho3d.github.io/documentation/HEAD/index.html


Building prerequisites
----------------------

Although all required third-party libraries are included as source code, there
are system-level dependencies that must be satisfied before Urho3D can be built
successfully:

- For Windows, the June 2010 DirectX SDK needs to be installed.

- For Linux, the following development packages need to be installed:
  libx11-dev, libxrandr-dev, libasound2-dev on Debian-based distros;
  libX11-devel, libXrandr-devel, alsa-lib-devel on RedHat-based distros.
  Also install the package libgl1-mesa-dev (Debian) or mesa-libGL-devel (RH)
  if your GPU driver does not include OpenGL headers & libs. Building as 32-bit 
  on a 64-bit system requires installing also the 32-bit versions of the 
  development libraries.

- For Raspberry Pi, the following development packages need to be installed:
  libraspberrypi0, libraspberrypi-dev, libasound2-dev, libudev-dev on Raspbian;
  raspberrypi-vc-libs, raspberrypi-vc-libs-devel, alsa-lib-devel, systemd-devel
  on Pidora. The first two packages which contain the Broadcom VideoCore IV
  libraries and development headers should normally come preinstalled.

- For Mac OS X, the Xcode developer tools package should include everything
  necessary.

- For Android, the Android SDK and Android NDK (minimum API level 12) need to be
  installed. Optionally, also install Eclipse ADT plugin for building and 
  deployment via Eclipse.

To run Urho3D, the minimum system requirements are:

- Windows: CPU with SSE instructions support, Windows XP or newer, DirectX 9.0c,
  GPU with Shader Model 2 support (Shader Model 3 recommended.)

- Linux & Mac OS X: CPU with SSE instructions support, GPU with OpenGL 2.0
  support, EXT_framebuffer_object and EXT_packed_depth_stencil extensions.

- Raspberry Pi: Model B revision 2.0 with at least 128 MB of 512 MB SDRAM
  allocated for GPU. OpenGL ES 2.0 capable GPU.

- Android: OS version 2.3 or newer, OpenGL ES 2.0 capable GPU.

- iOS: OpenGL ES 2.0 capable GPU.

SSE requirement can be eliminated by disabling the use of SSE instruction set,
see "Build options" below.


Desktop build process
---------------------

On desktop systems Urho3D uses CMake (http://www.cmake.org) to build. The
process has two steps:

1) Run CMake in the root directory with your preferred toolchain specified to
   generate the build files. You can use the provided batch files or shell
   scripts on the respective platform.
   
    Windows: cmake_vs2008.bat, cmake_vs2010.bat, cmake_vs2012.bat, or
             cmake_mingw.bat,
    Linux: cmake_gcc.sh, cmake_eclipse.sh, or cmake_codeblocks.sh,
    Mac OS X: cmake_gcc.sh or cmake_macosx.sh.

2) Open the CMake's generated project file in the IDE of your choice. Change the
   build configuration (Debug/Release) and then build all the targets.

   Visual Studio: open Urho3D.sln from the Build directory.
   Xcode: open Urho3D.xcodeproj.
   CodeBlocks: open Urho3D.cbp.
   Eclipse: import project using File|Import "Existing Projects into Workspace".
   GCC: execute make from the Build directory in a terminal/console.

Note that Eclipse requires CDT plugin to build C/C++ project.

If using MinGW to compile, DirectX headers may need to be acquired separately.
They can be copied to the MinGW installation eg. from the following package:
http://www.libsdl.org/extras/win32/common/directx-devel.tar.gz. These will
be missing some of the headers related to shader compilation, so a MinGW build 
will use OpenGL by default. To build in Direct3D9 mode, the MinGW-w64 port is 
necessary: http://mingw-w64.sourceforge.net/. Using it, Direct3D9 can be 
enabled with the CMake option -DURHO3D_OPENGL=0.

After the build is complete, the programs can be run from the Bin directory.
These include the Urho3D player application, which can run application scripts,
the tools, and C++ sample applications if they have been enabled.

To run the Urho3D player application from the Visual Studio debugger, set the 
Urho3DPlayer project as the startup project and enter its relative path and 
filename into Properties -> Debugging -> Command: ..\..\..\Bin\Urho3DPlayer.exe.
Additionally, entering -w into Debugging -> Command Arguments is highly
recommended. This enables startup in windowed mode: without it running into an
exception or breakpoint will be obnoxious as the mouse cursor will likely be
hidden. To actually make the Urho3DPlayer application do something useful, it
must be supplied with the name of the script file it should load and run. You
can try for example the following arguments: Scripts/NinjaSnowWar.as -w

To run from Eclipse on Linux, locate and select the Urho3DPlayer executable in
the Project Explorer. From the menu, choose "Run Configurations" to create a new
launch configuration for "C/C++ Application". Switch to "Arguments" tab, specify
the argument required by Urho3DPlayer executable.

To run from Xcode on Mac OS X, edit the Product Scheme to set "Run" setting
to execute "Urho3DPlayer" in the "Info" tab. In the "Arguments" tab, specify the
arguments required by Urho3DPlayer executable. Ensure the check boxes are ticked
on the argument entries that you want to be active.

CMake caches some internal variables to speed up the subsequent invocation of
the CMake build script. This is normally a good thing. However, there are cases
when this is not desirable, for instance when switching CMake generators or
after upgrading development software components. In such cases, it is recomended
to first clean the CMake cache by invoking cmake_clean.bat or cmake_clean.sh.


Android build process
---------------------

First, if you are building under Windows host without MKLINK support then copy
Bin/Data and Bin/CoreData directories to the Source/Android/assets directory
(you can use the provided batch file CopyData.bat). This step is not necessary
for Windows with MKLINK support and non-Windows host because the build script
uses symbolic links for host development environments that support it.

Set the ANDROID_NDK environment variable to point to your Android NDK. On
Windows, ensure that make.exe from the Android NDK is included in the path and
is executable from the command line.

On Windows, execute cmake_android.bat. If your user account has privilege to use
MKLINK then you have the option to provide the build option "-DURHO3D_MKLINK=1"
when invoking the cmake_android.bat to generate out-of-source build tree. Then
go to the build tree directory which could be either 'Source/Android' or
'android-Build' (when using MKLINK) and execute the below commands to start the
build.

- android update project -p . -t 1 (replace id to match the desired target API)
- make -j8 (replace the number to match your host's number of logical CPU cores)
- ant debug

On OS X or Linux, execute cmake_gcc.sh (the presence of ANDROID_NDK environment
variable instructs the shell script to also generate project file for Android
build besides normal desktop build). Then go to the build tree (which is
currently defaulted to 'android-Build' directory) and execute the same commands
as above.

After the commands finish successfully, the APK should have been generated in
the build tree's "bin" subdirectory, from where it can be installed on a device
or an emulator. The command "ant installd" can be used for this. After the debug
APK has been installed, you can use "rake android" command to automate the test
running of the APK on the attached Android device.

For a release build, use the "ant release" command instead of "ant debug" and
follow the Android SDK instructions on how to sign your APK properly.

By default the Android package for Urho3D is com.googlecode.urho3d. For a real
application you must replace this with your own package name. The Urho3D
activity subclasses the SDLActivity from org.libsdl.app package, whose name (or
the JNI code from SDL library) does not have to be changed.

Note that the native code is built by default for armeabi-v7a ABI. To make your
program compatible also with old Android devices, build also an armeabi version
by executing the CMake batch file or shell script again with the build option
"-DANDROID_ABI=armeabi" added, then execute make again in the build directory.
Similarly, the native code can be built using 64-bit ABI by changing the value
of this build option. See \ref Build_Options "Build options" for all the
possible values.

You can also build and deploy using Eclipse IDE with ADT plugin. To do that,
after setting the ANDROID_NDK environment variable then run cmake_eclipse.sh
instead of cmake_gcc.sh. Import "Existing Android Code into Workspace" from the
CMake generated Eclipse project file in the build tree. Switch Eclipse IDE to
use Java Perspective. Update project properties to choose the desired Android
API target. Choose "Run" to let ADT automatically build and deploy the
application to Android (virtual) device.


iOS build process
-----------------

Run cmake_ios.sh. This generates an Xcode project named Urho3D.xcodeproj.

Open the Xcode project and check the properties for the Urho3D project (topmost
in the Project Navigator.) In Architectures -> Base SDK, choose your iOS SDK
(CMake would automatically select latest iOS when generating the Xcode project).
In Code Signing, enter your developer identity as necessary.

The Urho3DPlayer target will actually build the application bundle and copy
resources from Bin/Data and Bin/CoreData directories. Edit its build scheme to
choose debug or release mode.

To run from Xcode on iPhone/iPad Simulator, edit the Product Scheme to set "Run"
destination setting to "iPhone Simulator" or "iPad Simulator", and executable
to "Urho3DPlayer.app".


Raspberry Pi build process
--------------------------

For native build on Raspberry Pi itself, use the similar process for Linux
Desktop build described above.

For cross-compiling build on another build/host machine, firstly set the
RASPI_TOOL environment variable to point to your Raspberry Pi Cross-Compiling
tool where all the arm-linux-gnueabihf-* executables are located. You can setup
the tool using crosstool-NG (http://crosstool-ng.org/) or just download one
from https://github.com/raspberrypi/tools. Secondly, set the RASPI_ROOT
environment variable to point to your Raspbian or Pidora system root. You must
install the Urho3D prerequisites software development packages for Raspberry Pi
(see "Building_Prerequisites") in the system root before attempting to do the
Urho3D cross-compiling build. You can download a Raspbian system root from
https://github.com/urho3d/rpi-sysroot.

When running cmake_gcc.sh with RASPI_TOOL environment variable set, it tells
build script to generate additional raspi-Build directory for cross-compiling.
Go to this raspi-Build directory and proceed to execute make. After the build
is complete, the ARM executables can be found in Bin-CC output directory.

You can also build, deploy, run/debug (as C/C++ Remote Application) using
Eclipse IDE, if you run cmake_eclipse.sh to generate the project file. Import
the CMake generated Eclipse project in the raspi-Build directory into Eclipse's
workspace. Build the project as usual. Use the URHO3D_SCP_TO_TARGET build option
to automatically deploy the ARM executables to target Raspberry Pi as part of
every project build or configure Eclipse to perform a "download to target path"
in the Run/Debug configuration for C/C++ Remote Application. Either way, you
have to configure the Run/Debug configuration how to reach your target Raspberry
Pi.


MinGW cross-compile build process
---------------------------------

It is possible to cross-compile Urho3D for Windows using a Linux system. The
process is largely the same as for the Linux Desktop build process described
above.

To cross-compile, the MinGW tool-chain (compiler, linker and w32api) needs to be
installed on the system. You will also need the DirectX header files, those can
be downloaded and installed from the following packet:
http://www.libsdl.org/extras/win32/common/directx-devel.tar.gz.

For activating the MinGW tool-chain, and to allow it to find the correct
compiler, the MINGW_PREFIX environment variable needs to be set when running
cmake_gcc.sh. This variable should be set to the prefix of the compiler name.
So, if for example your MinGW compiler is named i686-pc-mingw32-gcc, the
MINGW_PREFIX should read i686-pc. Most likely you also need to set MINGW_ROOT
environment variable to point to your mingw32 system root.

Running cmake_gcc.sh with the MINGW_PREFIX environment variable set, produces
an additional mingw-Build directory. Go to this directory and execute make to
start the build process. When the build is complete, the Windows executables can
be found in the mingw-Bin output directory.


Desktop 64bit build
-------------------

When using MSVC compiler, the Urho3D CMake build script will configure the
Urho3D project to be built in 32-bit by default. When using other non-MSVC
compilers (like GCC or clang), the CMake build script will set the default to
32-bit or 64-bit based on the installed toolchain in the host system. You can
use the build option "URHO3D_64BIT" to override the default, by setting the
option to '0' (for 32-bit) and '1' (for 64-bit) explicitly. For MSVC on Windows
platform, setting the option to '1' also instructs the build script to use a
64-bit solution generator.


Library build
-------------

As of v1.31, the build process first builds the Urho3D library target (either
static or shared). The library is then linked against by other targets like
tools and samples that reference Urho3D as one of the external libraries. The
Urho3D library type is defaulted to static, so the build process would generate
standalone executables as previous releases. The Urho3D library type can be
changed using "URHO3D_LIB_TYPE" build option.

To install the Urho3D library (or should we call it SDK), use the usual
'make install' command when using Makefile. There is an equivalent command in
Visual Studio and Xcode IDE to build 'install' target instead of the default
'all' target. This could be useful when you want your application to always link
against a 'stable' installed version of the Urho3D library, while keeping your
Urho3D project root tree in sync with origin/master. That is, install the newly
built library after you have tested the changes do not break your application
during development.

Refer to "Using Urho3D as external library" on how to setup your own project to
use Urho3D as external library.


Documentation build
-------------------

If URHO3D_DOCS build option is set then a normal (ALL) build would not only
build Urho3D software but also Urho3D documentation automatically. If it is not
then the documentation can be generated by manually invoking 'make doc' command
or its equivalent command in IDE.

The prerequisites are Doxygen and Graphviz. Tools to dump the AngelScript API
for the default scripting subsystem and the LuaScript API (when the LuaScript
subsystem is also enabled) will be built internally when all the tools are being
built.


Compiling Direct3D shaders
--------------------------

When building with the Windows 8 SDK, copy d3dcompiler_46.dll from
C:/Program Files (x86)/Windows Kits/8.0/bin/x86 to Urho3D Bin directory so that
Urho3D executables will run correctly.

Note that you can also force an OpenGL mode build on Windows by using the CMake
option in the table below; OpenGL does not depend on a separate shader compiler
DLL.


Build options
-------------

A number of build options can be defined explicitly when invoking the above
cmake_xxxx batch files or shell scripts.

|---------------------|-|------------------------------------------------------|
|Build Option         |V|Description                                           |
|---------------------|-|------------------------------------------------------|
|URHO3D_64BIT         |-|Enable 64bit build, on MSVC default to 0, on other    |
|                     | | compilers the default is set based on the installed  |
|                     | | toolchain on host system                             |
|URHO3D_ANGELSCRIPT   |1|Enable AngelScript scripting support                  |
|URHO3D_LUA           |0|Enable Lua scripting support                          |
|URHO3D_LUAJIT        |0|Enable Lua scripting support using LuaJIT (check      |
|                     | | LuaJIT's CMakeLists.txt for more options)            |
|URHO3D_LUAJIT_AMALG  |0|Enable LuaJIT amalgamated build (LuaJIT only)         |
|URHO3D_SAFE_LUA      |0|Enable Lua C++ wrapper safety checks (when Lua        |
|                     | | scripting support is enabled only)                   |
|URHO3D_NETWORK       |1|Enable Networking support                             |
|URHO3D_PHYSICS       |1|Enable Physics support                                |
|URHO3D_NAVIGATION    |1|Enable Navigation support                             |
|URHO3D_URHO2D        |1|Enable 2D rendering & physics support                 |
|URHO3D_SAMPLES       |0|Build sample applications                             |
|URHO3D_TOOLS         |1|Build standalone tools (Desktop and RPI only;         |
|                     | | on Android only build Lua standalone tools)          |
|URHO3D_EXTRAS        |0|Build extras (Desktop and RPI only)                   |
|URHO3D_DOCS          |0|Generate documentation as part of normal build (the   |
|                     | | 'doc' builtin target can be used to generate         |
|                     | | documentation regardless of this option's value)     |
|URHO3D_DOCS_QUIET    |0|Generate documentation as part of normal build,       |
|                     | | suppress generation process from sending anything to |
|                     | | stdout                                               |
|URHO3D_SSE           |1|Enable SSE instruction set                            |
|URHO3D_MINIDUMPS     |1|Enable minidumps on crash (VS only)                   |
|URHO3D_FILEWATCHER   |1|Enable filewatcher support                            |
|URHO3D_PROFILING     |1|Enable profiling support                              |
|URHO3D_LOGGING       |1|Enable logging support                                |
|URHO3D_TESTING       |0|Enable testing support                                |
|URHO3D_TEST_TIME_OUT |5|Number of seconds to test run the executables (when   |
|                     | | testing support is enabled only)                     |
|URHO3D_OPENGL        |0|Use OpenGL instead of Direct3D (Windows platform only)|
|URHO3D_MKLINK        |0|Use mklink command to create symbolic links (Windows  |
|                     | | Vista and above only)                                |
|URHO3D_STATIC_RUNTIME|0|Use static C/C++ runtime libraries and eliminate the  |
|                     | | need for runtime DLLs installation (VS only)         |
|URHO3D_LIB_TYPE      |*|Specify Urho3D library type, possible values are      |
|                     | | STATIC (*default) and SHARED                         |
|URHO3D_SCP_TO_TARGET |-|Use scp to transfer executables to target system (non-|
|                     | | Android cross-compiling build only), SSH digital key |
|                     | | must be setup first for this to work, typical value  |
|                     | | has a pattern of usr@tgt:remote-loc                  |
|CMAKE_BUILD_TYPE     |*|Specify CMake build configuration (single-            |
|                     | | configuration generator only), possible values are   |
|                     | | Release (*default), RelWithDebInfo, and Debug        |
|CMAKE_OSX_           |-|Specify Mac OS X deployment target (OSX build only);  |
| DEPLOYMENT_TARGET   | | default to current running OS X if not specified     |
|IPHONEOS_            |-|Specify iPhone OS deployment target (iOS build only); |
| DEPLOYMENT_TARGET   | | default to latest installed iOS SDK if not specified |
|ANDROID_ABI          |*|Specify target ABI (Android build only), possible     |
|                     | | values are arm64-v8a, armeabi, armeabi-v6 with VFP,  |
|                     | | armeabi-v7a (*default), armeabi-v7a with NEON,       |
|                     | | armeabi-v7a with VFPV3, mips, mips64, x86, and x86_64|
|ANDROID_NATIVE_API   |*|Specify target API level (Android build only),        |
| _LEVEL              | | possible values depends on installed NDK version,    |
|                     | | default to API level 12 on 32-bit ABIs,              |
|                     | | default to API level 21 on 64-bit ABIs               |
|ANDROID_NDK_GDB      |0|Enable ndk-gdb support (Android Debug build only)     |
|---------------------|-|------------------------------------------------------|

Note that build option values specified via command line are cached by CMake.
The cached values will be used by CMake in the subsequent invocation. That is,
the same build options are not required to be specified again and again. Once a
non-default build option value is being cached, it can only be reverted back to
its default value by explicitly setting it via command line. That is, simply by
NOT passing the corresponding build option would not work. One way to revert all
the build options to their default values is by clearing the CMake cache by
calling cmake_clean.bat or cmake_clean.sh.


Using cmake-gui to configure and generate Urho3D project file
-------------------------------------------------------------

Instead of using one of the provided batch files or shell scripts, you can use
cmake-gui to configure and generate Urho3D project file. However, you have to
adhere to the current limitations:

- The build directory must be located inside the Urho3D project root directory
  and sibling of the "Source" directory.
- The build directory must be named accordingly based on the target platform.
    - On desktop/native platform: Build
    - On Windows platform using MinGW: mingw-Build
    - On iOS platform: ios-Build
    - On Android platform: android-Build
    - On Raspberry Pi platform: raspi-Build
- The runtime and archive output directories are automatically set based on the
  target platform regardless of the build directory name (should you choose not
  to adhere with the above). For example on Android platform, they will be
  android-Bin and android-Lib, respectively.
  If you choose not to adhere with this build directory naming convention then
  you will have to install the Urho3D library into your local filesystem in
  order to use the Urho3D library in your external project.
  See http://urho3d.github.io/documentation/HEAD/_building.html#Building_Library
  and http://urho3d.github.io/documentation/HEAD/_using_library.html#FromSDK.
- All the post-CMake workaround and/or bug fixes that are scripted in the batch
  files or shell scripts are not applied to the generated project file.

Steps to configure:

1. In the Urho3D project root directory, invoke "cmake-gui Source".
2. Set the build directory name.
3. Configure and update the build options as many times as necessary until there
   are no more new options in red. For the first configuration, choose the
   generator you like to use. Click the Group check box to group the build
   options.
    - In the Ungrouped Entries: check IOS option when targeting it on Xcode.
    - In the URHO3D group: check any of the options you desire. Some of the
      options when checked may cause new options to be made available in the
      subsequent configuration loop.
4. Generate when all the configurations are done.


History
-------

V1.32   - Finalized Urho2D functionality, including 2D physics using Box2D,
          sprite animation and tile maps
        - Threaded background resource loading. Must be manually triggered via
          ResourceCache or by loading a scene asynchronously
        - Attribute and material shader parameter animation system
        - Customizable onscreen joystick for mobile platforms. Used in examples
        - Touch camera control in examples on mobile platforms
        - Touch emulation by mouse
        - Multi-touch UI drag support
        - Consistent touch ID's across platforms
        - Absolute, relative and wrap modes for the operating system mouse cursor
        - Support for connecting & removing joysticks during runtime
        - Negative light & light brightness multiplier support
        - Transform spaces for Node's translate, rotate & lookat functions
        - Scrollable console
        - Selectable console command interpreter (AngelScript, Lua, FileSystem)
        - Touch scroll in ScrollView & ListView
        - UI layout flex scale mode
        - Custom sound streams from C++
        - LogicComponent C++ base class with virtual update functions similar to
          ScriptObject
        - Signed distance field font support
        - JSON data support
        - Matrix types in Variant & XML data
        - Intermediate rendertarget refactoring: use viewport size to allow
          consistent UV addressing
        - ParticleEmitter refactoring: use ParticleEffect resource for
          consistency with ParticleEmitter2D and more optimal net replication
        - Expose LZ4 compression functions
        - Support various cube map layouts contained in a single image file
        - Configurable Bullet physics stepping behavior. Can use elapsed time
          limiting, or a variable timestep to use less CPU
        - Default construct math objects to zero / identity
        - Mandatory registration for remote events. Check allowed event only
          when receiving
        - Teapot & torus builtin objects
        - FXAA 3.11 shader
        - Triangle rendering in DebugRenderer (more efficient than 3 lines)
        - Material/texture quality and anisotropy as command line options and
          engine startup parameters
        - Spline math class, which the SplinePath component uses
        - Console auto-show on error
        - DrawableProxy2D system for optimizing 2D sprite drawing
        - Possibility to decouple BorderImage border UV's from element size
        - Editor & NinjaSnowWar resources split into subdirectories
        - UI hover start & end events
        - UI drag cancel by pressing ESC
        - Allowed screen orientations can be controlled. Effective only on iOS
        - Rendering sceneless renderpaths
        - Define individual material passes as SM3-only
        - Support for copying ListView text to system clipboard
        - Async system command execution
        - Generic attribute access for Lua script objects
        - Use Lua functions directly as event subscribers
        - Touch gesture recording and load/save
        - AssetImporter option to allow multiple import of identical meshes
        - Automatically create a physics world component to scene when necessary
        - GetSubimage function in the Image class
        - Possibility to clone existing components from another scene node
        - Improve terrain rendering on mobile devices
        - Refactoring of camera facing modes in BillboardSet & Text3D
        - Additive alpha techniques for particle rendering
        - Possibility to use CustomGeometry component for physics triangle mesh 
          collision
        - Access to 2D node coordinates for convenience when using 2D graphics
          features
        - Save embedded textures in AssetImporter
        - Use best matching fullscreen resolution if no exact match
        - Use SDL_iPhoneSetAnimationCallback instead of blocking main loop
        - Allow fast partial terrain updates by modifying the heightmap image
        - API for setting image pixels by integer colors
        - Refactor to remove the separate ShortStringHash class
        - Deep clone functionality in Model resource
        - Zone can define a texture which is available to shaders. Not used by
          default
        - Allow logging from outside the main thread
        - Log warnings for improper attempts to use events from outside main 
          thread
        - Improved CustomGeometry dynamic updates
        - ConvexCast function in PhysicsWorld
        - Screen to world space conversion functions in Viewport class
        - Allow sending client rotation to server in addition to position
        - Allow accessing and modifying the engine's next timestep
        - DeepEnabled mechanism for disabling node or UI element hierarchies and
          then restoring their own enabled state
        - Allow to prevent closing a modal window with ESC
        - Per-viewport control of whether debug geometry should render
        - Optional interception of resource requests
        - Readded optional slow & robust mode to AreaAllocator
        - Optionally disable RigidBody mass update to allow fast adding of
        - several CollisionShape components to the same node
        - Runtime synchronization of resource packages from server to client
        - Disable multisample antialiasing momentarily during rendering. Used by
          default for UI & quad rendering
        - Glyph offset support in Font class
        - Font class internal refactoring
        - Allow to create AngelScript script objects by specifying the interface
          it implements
        - Window position startup parameters
        - Functions to get time since epoch & modify file's last modified time
        - Optionally auto-disable child elements of a scroll view when touch 
          scrolling
        - Allocate views permanently per viewport to allow querying for
          drawables, lights etc. reliably
        - Allow to specify material techniques/passes that should not be used
          on mobile devices
        - Reduced default shadow mapping issues on mobile devices
        - Minor rendering optimizations
        - Build system: possibility to build Urho3D without networking or 2D
          graphics functionality
        - Build system: improved generated scripting documentation
        - Build system: improved support for IDE's in CMake scripts
        - Build system: support up to Android NDK r10c and 64-bit ABIs
        - Build system: numerous other improvements
        - Editor: resource browser
        - Editor: spawn window for random-generating objects
        - Editor: allow either zoom or move from mouse wheel
        - Editor: locate object by doubleclicking node in hierarchy
        - Editor: take screenshots with F11, camera panning
        - Editor: button in value edit fields that allows editing by mouse drag
        - Updated SDL to 2.0.3.
        - Updated AngelScript to 2.29.1
        - Updated assimp
        - Updated Recast/Detour
        - Fix MinGW build issues
        - Fix techniques referring to wrong shaders
        - Fix Node::LookAt() misbehaving in certain situations
        - Fix resize event not reporting correct window size if window is
          maximized at start
        - Fix PhysicsWorld::GetRigidBodies() not using collision mask
        - Fix zone misassignment issues
        - Fix Lua not returning correctly typed object for UIElement::GetChild()
          & UIElement::GetParent()
        - Fix uninitialized variables in 2D physics components
        - Fix quad rendering not updating elapsed time uniform
        - Fix forward rendering normal mapping issues by switching calculations
          back to world space
        - Fix wrong logging level on Android
        - Fix multiple subscribes to same event on Lua
        - Fix missing Octree update in headless mode
        - Fix crash when using FreeType to access font kerning tables
        - Fix ReadString() endless loop if the string does not end
        - Fix shadow mapping on OS X systems with Intel GPU
        - Fix manually positioned bones being serialized properly
        - Fix file checksum calculation on Android
        - Fix accelerometer input on Android when device is flipped 180 degrees
        - Fix missing or misbehaving Lua bindings
        - Fix crashes in physics collision handling when objects are removed
          during it
        - Fix shader live reload if previous compile resulted in error
        - Fix named manual textures not recreating their GPU resource after
          device loss
        - Fix skeleton-only model not importing in AssetImporter
        - Fix terrain raycast returning incorrect position/normal
        - Fix animation keyframe timing in AssetImporter if start time is not 0
        - Fix storing Image resources to memory unnecessarily during cube/3D 
          texture loading
        - Fix to node transform dirtying mechanism and the TransformChanged() 
          script function
        - Fix returned documents directory not being writable on iOS
        - Fix click to emptiness not closing a menu
        - Fix FileWatcher notifying when file was still being saved. By default
          delay notification 1 second
        - Fix .txml import in the editor
        - Fix erroneous raycast to triangles behind the ray
        - Fix crash when multiple AnimatedModels exist in a node and the master 
          model is destroyed
        - Fix missing Matrix4 * Matrix3x4 operator in script
        - Fix various compile warnings that leak to applications using Urho3D
        - Fix DebugHud update possibly being late one frame
        - Fix various macros not being usable outside Urho3D namespace
        - Fix erroneous layout with wordwrap text elements
        - Fix debug geometry rendering on flipped OpenGL viewports
        - Fix kNet debug mode assert with zero sized messages
        - Fix not being able to stop and restart kNet server
        - Fix AreaAllocator operation
        - Fix possible crash with parented rigidbodies
        - Fix missing network delta update if only user variables in a Node have
          been modified
        - Fix to only search for June 2010 DirectX SDK, as earlier SDK's will
          fail
        - Fix wrong search order of added resource paths
        - Fix global anisotropic filtering on OpenGL
        - Fix animation triggers not working if trigger is at animation end
        - Fix CopyFramebuffer shader name not being used correctly on case-
          sensitive systems
        - Fix UI elements not receiving input when the window containing them is
          partially outside the screen to the left
        - Fix occlusion rendering not working with counterclockwise triangles
        - Fix material shader parameter animations going out of sync with other
          animations when the object using the material is not in view
        - Fix CPU count functions on Android          

V1.31   - Extensive build system improvements, especially for using Urho3D as
          a library in an external project.
        - LuaJIT support.
        - Improved Lua bindings, Lua coroutine support, automatic loading of
          compiled Lua scripts (.luc) if they exist.
        - HDR rendering, 3D textures, height fog and several new post process
          shaders.
        - Shader refactoring. Need for XML shader descriptions & ShaderCompiler
          tool removed.
        - Reflection / refraction rendering support.
        - 2D drawable components: StaticSprite2D, AnimatedSprite2D,
          ParticleEmitter2D.
        - ToolTip & MessageBox UI elements. UI logic improvements.
        - Optimized text rendering + dynamic population of font textures for
          improved batching.
        - AngelScript DelayedExecute for free functions, and event handling for
          any script object, not just ScriptInstances
        - Editor: added grid, toolbar, camera view presets, camera orbit,
          mouse wheel zoom, multiple viewports and orthographic camera.
        - Borderless window mode, possibility to change application icon.
        - SDL GameController support, raw key codes support.
        - Optimized shadow rendering on mobile devices. Low quality mode
          avoids dependent texture reads.
        - HttpRequest class runs in a background thread to avoid blocking.
        - Compressed package file support using the LZ4 library.
        - Cone parameters in SoundSource3D for directional attenuation.
        - Variant GetPtr() safety refactoring. Uses WeakPtr to store RefCounted
          subclasses. Use GetVoidPtr() to store unsafe arbitrary pointers.
        - Improved work queue completion events. Work items are now RefCounted
          to allow persisting them as necessary.
        - Allow to disable automatic execution of AngelScript & Lua from the 
          engine console.
        - Added shader variations, for example ambient occlusion texture and
          better emissive color support.
        - Added examples.
        - Update SDL to 2.0.1
        - Update AngelScript to 2.28.1.
        - Update FreeType to 2.5.0.
        - Fix partial texture updates, both Direct3D9 & OpenGL.
        - Fix long-standing audio click bug.
        - Fix kinematic rigidbodies to apply impulses correctly to dynamic
          bodies.
        - Plus many more improvements and bugfixes.

V1.3    - Lua scripting support.
        - Optional build as a single external library, static or dynamic.
        - Raspberry Pi support.
        - 64-bit build support.
        - HTTP client using the Civetweb library.
        - Enhanced CMake build scripts. Android build also uses CMake.
          Use out-of-source build on platform that supports it.
        - Rendering performance optimizations, optional StaticModelGroup
          component for culling and lighting several objects as one unit.
        - A set of sample applications implemented in C++, AngelScript and Lua.
        - Automatic Node/component handle member variable serialization for
          AngelScript script objects.
        - New UI theme.
        - Shadow & stroke effects in Text & Text3D.
        - Boolean shader uniforms.
        - Quick menu in the editor.
        - Material editor and preview in the editor.
        - Editable attributes for particle emitters.
        - Components are grouped into categories in the editor.
        - Update SDL to stable 2.0.0 release.
        - Several other improvements and bugfixes.

V1.23   - UI editing support in the editor.
        - Undo/redo in the editor.
        - Recast/Detour library integration for navigation mesh generation and
          pathfinding.
        - Open Asset Import Library update, enables FBX file support.
        - "Is Enabled" attribute in scene nodes and components for an uniform
          mechanism to temporarily disable unneeded audiovisual, physics or
          logic objects.
        - Script object public variables editing and serialization.
        - New components: Text3D and Sprite.
        - UI library functionality improvements.
        - sRGB texture and framebuffer support.
        - Switched to GLEW library for OpenGL extension handling.
        - Vegetation and lightmapping example shaders.
        - Engine configuration through a parameter map.
        - Lots of refactoring, code cleanup and bugfixes.

V1.22   - Configurable render path replaces hardcoded forward/prepass/deferred
          modes. Render path system also used for postprocessing now.
        - Threaded task priorities; long-running tasks (more than one frame) can
          coexist with the time-critical tasks.
        - Possibility to use also RGB normal maps.
        - CustomGeometry component, which allows geometry to be defined
          similarly to OpenGL immediate mode.
        - Elapsed time shader parameter for material animation.
        - Cubic environment mapping example shaders.
        - Separate physics collision start & end events.
        - Visual Studio 2012, Eclipse & Xcode build support.
        - Many bugfixes, including iOS 6 orientation & shadow mapping bugfixes,
          skinning on some Android devices.

V1.21   - Bugfixes and code cleanup.
        - External window support (experimental.)
        - UI elements refactored to use attributes for serialization.
        - Animation state editing and animation trigger events.
        - Scene update time scale can be modified.
        - Improved the delayed method call system.

V1.2    - Android and iOS support.
        - Decal rendering.
        - Terrain rendering.
        - Joystick input support.
        - Use SDL library for windowing and input on all platforms.
        - KTX and PVR image loading (for ETC1 & PVRTC compressed textures.)
        - Removed need for shader preprocessing; reorganized shaders to be more
          friendly to base custom shaders on.
        - Inbuilt geometry shapes in the editor.

V1.16   - Switched to Bullet physics library.
        - More physics constraint types.
        - Rendering and networking performance optimizations.
        - Use Squish library to implement software DXT decompression when not
          supported in hardware.

V1.15   - New deferred rendering pipeline.
        - Unicode support.
        - Live resource reloading in the editor (Windows only so far.)
        - More accurate frame timing.
        - Bugfixes to physics jittering and FBO performance issue on Linux.

V1.14   - Object (partial scene) load/save.
        - Post-processing.
        - Switched to pugixml library, scene load/save optimizations.
        - Bugfixes to rendertexture views and component attributes.

V1.13   - Task-based multithreading.
        - Vertex lighting option.
        - Forward and light pre-pass rendering pipelines.

V1.12   - Manipulator gizmo and multi-editing in the editor.
        - Switched to forward rendering exclusively, which is optimized to do
          more work in the vertex shader.
        - Zone system refactoring. Objects check the zone they belong to for
          per-zone light masking, ambient light and fog settings.
        - Scripting API fixes and improvements.

V1.11   - Bugfixes and performance optimizations.
        - Added GraphicsTest example from V1.0 (now called TestSceneOld.)
        - Added fallback mode, which is used if multiple render targets or
          hardware shadow maps are not available.

V1.1    - Object and scene model refactoring.
        - Automatic serialization of scene objects via attributes.
        - Added OpenGL and cross-platform support.
        - Switched to kNet library for networking.

V1.0    - Original release.
