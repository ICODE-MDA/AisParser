The code is currently split up into headers that contain all of the objects, and an apps directory with four different apps that can be used to read/write AIS. If you compare each apps cpp, you should notice that they are exactly the same with the exception of the declaration of the Writer class and Reader class.If you come up with a new AIS app, you can create a new folder for it in the apps directory. If you need a new functionality, you should first consider making a new object that will handle your functionality rather than changing the apps cpp.

There is a fifth app called "CannedAisServer" which will broadcast AIS on a TCP port specified on the command line. It can be used for creating an application that needs to have a TCP AIS input stream. It was made to simulate the AIS stream that TV32 broadcasts and/or the stream that SatAIS vendors broadcast. You specify a file with AIS data, the port to broadcast on, and the amount of time between each message broadcast (in milliseconds).

Prerequisites:
(1)Visual Studio 2010 Express
(2)Boost 1.48.0
(3)Oracle OCCI For Visual Studio 2010


========================================================================================================
========================================================================================================
====================================VISUAL STUDIO EXPRESS===============================================
========================================================================================================
========================================================================================================
========================================================================================================

http://www.microsoft.com/visualstudio/en-us/products/2010-editions/express


========================================================================================================
========================================================================================================
====================================BOOST===============================================================
========================================================================================================
========================================================================================================
========================================================================================================

For the project to work, you must download and compile the Boost libraries.

Download:
http://www.boost.org/users/history/version_1_48_0.html


Instructions for compile:
http://www.boost.org/doc/libs/1_48_0/more/getting_started/windows.html

Follow the build instructions pasted below:
5.2   Or, Simplified Build From Source

If you wish to build from source with Visual C++, you can use a simple build procedure described in this section. Open the command prompt and change your current directory to the Boost root directory. Then, type the following commands:

bootstrap
.\b2

The first command prepares the Boost.Build system for use. The second command invokes Boost.Build to build the separately-compiled Boost libraries. Please consult the Boost.Build documentation for a list of allowed options.



========================================================================================================
========================================================================================================
====================================ORACLE OCCI=========================================================
========================================================================================================
========================================================================================================
========================================================================================================

http://www.oracle.com/technetwork/topics/winsoft-085727.html

==Instant Client Package - Basic: All files required to run OCI, OCCI, and JDBC-OCI applications 
==Instant Client Package - SDK: Additional header files and an example makefile for developing Oracle applications with Instant Client


