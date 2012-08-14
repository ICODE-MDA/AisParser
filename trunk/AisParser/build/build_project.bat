@echo off

set OLDPATH=%PATH%

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

set PATH=C:\Program Files (x86)\CMake 2.8\bin;%PATH%
set qt_dir=D:\libraries\qt\qt-everywhere-opensource-src-4.8.2_x86

set boost_dir="D:\libraries\boost\v1_48_0"
set ais_parser_src_dir="D:\libraries\icode\google-icode-mda\trunk\AisParser\src"

set oracle_includes="D:\libraries\oracle\instantclient-sdk-nt-11.2.0.2.0\include"
set oracle_library_dir="D:\libraries\oracle\instantclient-sdk-nt-11.2.0.2.0\lib\msvc"

set mysql_includes="D:\libraries\mysql\v110\include"
set mysql_library_dir="D:\libraries\mysql\v110\lib"

set postgresql_includes="D:\libraries\libpqxx\v4_0\include"
set postgresql_library_dir="D:\libraries\libpqxx\v4_0\lib\x86"

set icode_third_party_dir="D:\libraries\icode\3rd-party"
set oracle_bin_dir=D:\libraries\oracle\instantclient-sdk-nt-11.2.0.2.0\bin
set mysql_bin_dir=D:\libraries\mysql\v110\lib

cmake -G "Visual Studio 10"^
 -DBUILD_ALL_APPS=ON^
 -DQT_DIR=%qt_dir%^
 -DAIS_PARSER_SRC_DIR=%ais_parser_src_dir%^
 -DICODE_THIRD_PARTY_DIR=%icode_third_party_dir%^
 -DORACLE_INCLUDE_DIR=%oracle_includes%^
 -DORACLE_LIBRARY_PATH=%oracle_library_dir%^
 -DMYSQL_INCLUDE_DIR=%mysql_includes%^
 -DMYSQL_LIBRARY_PATH=%mysql_library_dir%^
 -DPOSTGRESQL_INCLUDE_DIR=%postgresql_includes%^
 -DPOSTGRESQL_LIBRARY_PATH=%postgresql_library_dir%^
 -DBOOST_INCLUDE_DIR=%boost_dir%^
 -DBOOST_LIBRARY_PATH=%boost_dir%/stage/lib/x86^
 ../

rem Create scripts to put the Oracle binaries in the path for running 

rem LogToOracleDb Release
mkdir .\apps\LogToOracleDb\Release
echo set OLDPATH=%%PATH%% >> .\apps\LogToOracleDb\Release\LogToOracleDb.bat
echo set PATH=%oracle_bin_dir%;%%PATH%% >> .\apps\LogToOracleDb\Release\LogToOracleDb.bat
echo LogToOracleDb.exe %%1 %%2 %%3 %%4 %%5 %%6 %%7 >> .\apps\LogToOracleDb\Release\LogToOracleDb.bat
echo set PATH=%%OLDPATH%%  >> .\apps\LogToOracleDb\Release\LogToOracleDb.bat
echo set OLDPATH=  >> .\apps\LogToOracleDb\Release\LogToOracleDb.bat


rem TcpToDb Release
mkdir .\apps\TcpToOracleDb\Release
echo set OLDPATH=%%PATH%% >> .\apps\TcpToOracleDb\Release\TcpToOracleDb.bat
echo set PATH=%oracle_bin_dir%;%%PATH%% >> .\apps\TcpToOracleDb\Release\TcpToOracleDb.bat
echo TcpToOracleDb.exe %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 >> .\apps\TcpToOracleDb\Release\TcpToOracleDb.bat
echo set PATH=%%OLDPATH%% >> .\apps\TcpToOracleDb\Release\TcpToOracleDb.bat
echo set OLDPATH= >> .\apps\TcpToOracleDb\Release\TcpToOracleDb.bat


rem Create scripts to put the Mysql binaries in the path for running 

rem LogToMySqlDb Release
mkdir .\apps\LogToMySqlDb\Release
echo set OLDPATH=%%PATH%% >> .\apps\LogToMySqlDb\Release\LogToMySqlDb.bat
echo set PATH=%mysql_bin_dir%;%%PATH%% >> .\apps\LogToMySqlDb\Release\LogToMySqlDb.bat
echo LogToMySqlDb.exe %%1 %%2 %%3 %%4 %%5 %%6 %%7 >> .\apps\LogToMySqlDb\Release\LogToMySqlDb.bat
echo set PATH=%%OLDPATH%% >> .\apps\LogToMySqlDb\Release\LogToMySqlDb.bat
echo set OLDPATH= >> .\apps\LogToMySqlDb\Release\LogToMySqlDb.bat


rem TcpToMySqlDb Release
mkdir .\apps\TcpToMySqlDb\Release
echo set OLDPATH=%%PATH%% >> .\apps\TcpToMySqlDb\Release\TcpToMySqlDb.bat
echo set PATH=%mysql_bin_dir%;%%PATH%% >> .\apps\TcpToMySqlDb\Release\TcpToMySqlDb.bat
echo TcpToMySqlDb.exe %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 >> .\apps\TcpToMySqlDb\Release\TcpToMySqlDb.bat
echo set PATH=%%OLDPATH%% >> .\apps\TcpToMySqlDb\Release\TcpToMySqlDb.bat
echo set OLDPATH= >> .\apps\TcpToMySqlDb\Release\TcpToMySqlDb.bat

set PATH=%OLDPATH%
set OLDPATH=
