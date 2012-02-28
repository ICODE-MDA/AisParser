@echo off

set OLDPATH=%PATH%

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

set PATH=C:\Program Files (x86)\CMake 2.8\bin;%PATH%

set boost_dir="D:\libraries\boost\v1_48_0"
set ais_parser_src_dir="D:\libraries\icode\oddworld-icode\AisParser\src"
set oracle_includes="D:\libraries\oracle\instantclient-sdk-nt-11.2.0.2.0\include"
set oracle_library_dir="D:\libraries\oracle\instantclient-sdk-nt-11.2.0.2.0\lib\msvc"
set oracle_bin_dir=D:\libraries\oracle\instantclient-sdk-nt-11.2.0.2.0\bin

cmake -G "Visual Studio 10"^
 -DAIS_PARSER_SRC_DIR=%ais_parser_src_dir%^
 -DORACLE_INCLUDE_DIR=%oracle_includes%^
 -DORACLE_LIBRARY_PATH=%oracle_library_dir%^
 -DBOOST_INCLUDE_DIR=%boost_dir%^
 -DBOOST_LIBRARY_PATH=%boost_dir%/stage/lib^
 ../src/


rem Create scripts to put the Oracle binaries in the path for running 

rem LogToDb Release
mkdir .\apps\LogToDb\Release
echo set OLDPATH=%%PATH%% >> .\apps\LogToDb\Release\LogToDb.bat
echo set PATH=%oracle_bin_dir%;%%PATH%% >> .\apps\LogToDb\Release\LogToDb.bat
echo LogToDb.exe %%1 %%2 %%3 %%4 %%5 %%6 %%7 >> .\apps\LogToDb\Release\LogToDb.bat
echo set PATH=%%OLDPATH%% >> .\apps\LogToDb\Release\LogToDb.bat
echo set OLDPATH= >> .\apps\LogToDb\Release\LogToDb.bat


rem TcpToDb Release
mkdir .\apps\TcpToDb\Release
echo set OLDPATH=%%PATH%% >> .\apps\TcpToDb\Release\TcpToDb.bat
echo set PATH=%oracle_bin_dir%;%%PATH%% >> .\apps\TcpToDb\Release\TcpToDb.bat
echo TcpToDb.exe %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 >> .\apps\TcpToDb\Release\TcpToDb.bat
echo set PATH=%%OLDPATH%% >> .\apps\TcpToDb\Release\TcpToDb.bat
echo set OLDPATH= >> .\apps\TcpToDb\Release\TcpToDb.bat

set PATH=%OLDPATH%
set OLDPATH=
