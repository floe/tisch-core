TISCH Framework - Release 3.0 "Back From The Dead"
==================================================

"Tangible Interactive Surfaces for Collaboration between Humans"

The TISCH framework supports cross-platform development of novel UI
applications. For details on the underlying architecture and source 
code documentation, please visit the Sourceforge website:

http://tisch.sourceforge.net/


Highlights
----------

- support for multitouch, tangible interfaces, full-body interaction...
- ready-to-use user interface widgets based on OpenGL
- reconfigurable, hardware-independent gesture recognition engine
- support for widely used (move, scale, rotate..) as well as custom gestures

- hardware drivers for FTIR, DI, Wiimote, Kinect..
- native use of popular TUIO 2.0 transport format

- cross-platform: Linux, MacOS X, Windows (32 and 64 bit)
- cross-language: C++ with bindings for C#, Java, Python


Directory Layout
----------------

libtisch-3.0/
	calibd/      Calibration Layer
	drivers/     Hardware Abstraction Layer (HAL)
	gispl/       Gestural Interface Specification Layer (gesture recognizer)
	libs/        various helper libraries & wrappers for C#, Java, Python
	scripts/     build scripts
	widgets/     OpenGL-based Widget Layer


Quick Start
-----------

If you only have a mouse and want to give it a try:

	1. run "demo -m" (enables mouse emulation mode)

If you have a camera-based input device, e.g. an FTIR table or a Kinect:

	1. run "touchd -V -c example.xml" (this is the most basic mode, just tracks bright blobs)
	2. run "calibtool -f" and follow the instructions (this is necessary only once)
	3. run "calibd", if not already running
	4. run "demo"


Notes about demo programs
-------------------------

All of the demo programs (picview, slideshow, graph, demo) accept '-m' as first
parameter, switching them to mouse emulation mode. picview and slideshow take a
list of PNG images to display as additional parameters.

In mouse emulation mode, you can move elements with click-and-drag. Rotate them
by clicking any mouse button and scrolling the mouse wheel. Scale them by
additionally pressing CTRL while using the wheel.

NOTE: if you are running an X server with XInput2 support (commonly known as MPX),
you can actually use multiple mice to emulate multitouch fairly well.


Building & Requirements
-----------------------

Windows (tested with Visual Studio 2010 on Windows 7)

	Requirements:
	- Windows SDK 7.0A or greater (for DirectShow camera input)
	- freeglut 2.6.0   or greater (for OpenGL graphics)
	- freenect 0.1.2   or greater (OpenKinect library, for Kinect input)

	You need to have the Windows SDK & freeglut 2.6.0 or greater installed. By
	default, the include paths point to a "libs/freeglut/" directory and to the
	default Windows SDK install directory. Optionally, you can also include
	libfreenect in "libs/freenect/". Pre-built binaries for freeglut and
	freenect are provided in 2 ZIP archives in libs/.

	If you installed your libraries somewhere else, you may need to adjust these
	paths. To do so,
	- open libtisch.sln in Visual Studio. 
	- Go to "Property Explorer".
	- Expand the tree until	you see the "common" property page and open it.
	- Click on "User Macros" and enter the correct path for "freeglut".

	The same applies to "winsdk", which should point to the location where your
	Windows SDK (formerly Platform SDK) is installed.

	Then, just select the Debug or Release config and build the solution. All libs 
	and binaries should be created in the appropriate output directory. Note: for
	Debug builds, the packaged binary libs are unsuitable, as they were compiled
	as Release builds. 

Linux   (tested with gcc 4.5.2 on Ubuntu 11.04 and gcc 4.6.3 on Ubuntu 12.04)
MacOS X (tested with gcc 4.0 on MacOS X 10.5 and gcc 4.2 on MacOS X 10.6)

	Requirements:
	- freeglut 2.4.0 or greater (Linux only - use "apt-get install freeglut3-dev")
	- libdc1394 2.0.0 or greater (optional - for Firewire cameras)

	If you want to use a Firewire camera, install libdc1394 v2.0.0 or greater
	first. Take care that pkg-config knows about it. For Linux, you should also
	have installed a GLUT library (preferably freeglut 2.6.0 or greater) in your
	system include/library paths.

	Then, simply run "make all && make install" to build all binaries and put 
	them into build/{bin,lib}/. Don't forget to add the lib directory to the 
	library path (LD_LIBRARY_PATH on Linux, DYLD_LIBRARY_PATH on MacOS X). You 
	can also run "make" without arguments to review the other available parameters.

	If you wish to install the binaries to a system directory such as /usr/local,
	set the DESTDIR (installation directory) and PREFIX (runtime prefix) variables
	when running make, e.g. as "make DESTDIR=/tmp/ PREFIX=/usr/local/ install".

	The default method for camera access is through libdc1394 with Video4Linux as
	fallback. Note that camera access has been tested with several different types
	of Logitech QuickCams (both PWC- and UVC-based USB types) and with Pointgrey
	FireflyMV Firewire cameras.


Known Issues
------------

- Documentation is lacking - check the Sourceforge Wiki for more information.
- The example configuration files are rather convoluted.
- The graphical frontends also accept TUIO 1.0 data, but the object types may
  or may not match. Check the Wiki at www.tuio.org for details.

