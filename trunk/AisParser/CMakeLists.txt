cmake_minimum_required(VERSION 2.8)


project( AisParser )

######### Check to make sure variable are set correctly #########
if(NOT DEFINED BUILD_ALL_APPS)
	set(BUILD_ALL_APPS NO CACHE BOOL "Flag for building all applications")
endif()

if(BUILD_ALL_APPS OR BUILD_QT_APPS)
	set(BUILD_ORACLE_APPS YES CACHE BOOL "Flag for building Oracle applications" FORCE)
	set(BUILD_MYSQL_APPS YES CACHE BOOL "Flag for building MySQL applications" FORCE)
	set(BUILD_POSTGRESQL_APPS YES CACHE BOOL "Flag for building PostgreSQL applications" FORCE)
	set(BUILD_SHAPEFILE_APPS YES CACHE BOOL "Flag for building shapefile applications" FORCE)
	set(BUILD_QT_APPS YES CACHE BOOL "Flag for building QT applications" FORCE)
else()
	if(NOT DEFINED BUILD_ORACLE_APPS)
		set(BUILD_ORACLE_APPS NO CACHE BOOL "Flag for building Oracle applications" FORCE)
	endif()

	if(NOT DEFINED BUILD_MYSQL_APPS)
		set(BUILD_MYSQL_APPS NO CACHE BOOL "Flag for building MySQL applications" FORCE)
	endif()

	if(NOT DEFINED BUILD_POSTGRESQL_APPS)
		set(BUILD_POSTGRESQL_APPS NO CACHE BOOL "Flag for building PostgreSQL applications" FORCE)
	endif()

	if(NOT DEFINED BUILD_SHAPEFILE_APPS)
		set(BUILD_SHAPEFILE_APPS NO CACHE BOOL "Flag for building shapefile applications" FORCE)
	endif()

	if(NOT DEFINED BUILD_QT_APPS)
		set(BUILD_QT_APPS NO CACHE BOOL "Flag for building QT applications" FORCE)
	endif()

	message(WARNING "If you don't specify BUILD_ALL_APPS=YES, you must speficfy which apps you would like to build by specifying the apps that you would like to build like one of the following BUILD_ORACLE_APPS=YES, BUILD_MYSQL_APPS=YES, BUILD_POSTGRESQL_APPS=YES, BUILD_SHAPEFILE_APPS=YES ")
endif()

######
###
if(BUILD_MYSQL_APPS)
	if(NOT DEFINED MYSQL_INCLUDE_DIR)
		set(MYSQL_INCLUDE_DIR "" CACHE PATH "Mysql Include Directory")
	endif()

	if(IS_DIRECTORY ${MYSQL_INCLUDE_DIR})
		message(STATUS "Mysql Include Directory: ${MYSQL_INCLUDE_DIR}")
	else()	
		message(SEND_ERROR "MYSQL_INCLUDE_DIR must be set to a directory!")
	endif()

	if(NOT DEFINED MYSQL_LIBRARY_PATH)
		set(MYSQL_LIBRARY_PATH "" CACHE PATH "Mysql library Directory")
	endif()

	if(IS_DIRECTORY ${MYSQL_LIBRARY_PATH})
		message(STATUS "Mysql library Directory: ${MYSQL_LIBRARY_PATH}")
	else()	
		message(SEND_ERROR "MYSQL_LIBRARY_PATH must be set to a directory!")
	endif()
endif()
###

###
if(BUILD_POSTGRESQL_APPS)
	if(NOT DEFINED POSTGRESQL_INCLUDE_DIR)
		set(POSTGRESQL_INCLUDE_DIR "" CACHE PATH "PostgreSQL Include Directory")
	endif()

	if(IS_DIRECTORY ${POSTGRESQL_INCLUDE_DIR})
		message(STATUS "PostgreSQL Include Directory: ${POSTGRESQL_INCLUDE_DIR}")
	else()	
		message(SEND_ERROR "POSTGRESQL_INCLUDE_DIR must be set to a directory!")
	endif()

	if(NOT DEFINED POSTGRESQL_LIBRARY_PATH)
		set(POSTGRESQL_LIBRARY_PATH "" CACHE PATH "PostgreSQL library Directory")
	endif()

	if(IS_DIRECTORY ${POSTGRESQL_LIBRARY_PATH})
		message(STATUS "PostgreSQL library Directory: ${POSTGRESQL_LIBRARY_PATH}")
	else()	
		message(SEND_ERROR "POSTGRESQL_LIBRARY_PATH must be set to a directory!")
	endif()	
endif()
###

###
if(BUILD_ORACLE_APPS)
	if(NOT DEFINED ORACLE_INCLUDE_DIR)
		set(ORACLE_INCLUDE_DIR "" CACHE PATH "Oracle Include Directory")
	endif()

	if(IS_DIRECTORY ${ORACLE_INCLUDE_DIR})
		message(STATUS "Oracle Include Directory: ${ORACLE_INCLUDE_DIR}")
	else()	
		message(SEND_ERROR "ORACLE_INCLUDE_DIR must be set to a directory!")
	endif()

	if(NOT DEFINED ORACLE_LIBRARY_PATH)
		set(ORACLE_LIBRARY_PATH "" CACHE PATH "Oracle library Directory")
	endif()

	if(IS_DIRECTORY ${ORACLE_LIBRARY_PATH})
		message(STATUS "Oracle library Directory: ${ORACLE_LIBRARY_PATH}")
	else()	
		message(SEND_ERROR "ORACLE_LIBRARY_PATH must be set to a directory!")
	endif()
endif()
###

###
if(BUILD_SHAPEFILE_APPS)
	if(NOT DEFINED ICODE_THIRD_PARTY_DIR)
		set(ICODE_THIRD_PARTY_DIR "" CACHE PATH "ICODE 3rd Party Include Directory")
	endif()

	if(IS_DIRECTORY ${ICODE_THIRD_PARTY_DIR})
		message(STATUS "ICODE 3rd Party Include Directory: ${ICODE_THIRD_PARTY_DIR}")
	else()	
		message(SEND_ERROR "ICODE_THIRD_PARTY_DIR must be set to a directory!")
	endif()
endif()
###

if(BUILD_QT_APPS)
	if(NOT DEFINED QT_DIR)
		set(QT_DIR "" CACHE PATH "QT Directory")
	endif()

	if(IS_DIRECTORY ${QT_DIR})
		message(STATUS "QT Directory: ${QT_DIR}")
	else()	
		message(SEND_ERROR "QT_DIR must be set to a directory!")
	endif()
endif()
######

if(NOT DEFINED BOOST_INCLUDE_DIR)
	set(BOOST_INCLUDE_DIR "" CACHE PATH "Boost Include Directory")
endif()

if(IS_DIRECTORY ${BOOST_INCLUDE_DIR})
	message(STATUS "Boost Include Directory: ${BOOST_INCLUDE_DIR}")
else()	
	message(SEND_ERROR "BOOST_INCLUDE_DIR must be set to a directory!")
endif()
###
if(NOT DEFINED BOOST_LIBRARY_PATH)
	set(BOOST_LIBRARY_PATH "" CACHE PATH "Boost library Directory")
endif()

if(IS_DIRECTORY ${BOOST_LIBRARY_PATH})
	message(STATUS "Boost library Directory: ${BOOST_LIBRARY_PATH}")
else()	
	message(FATAL_ERROR "BOOST_LIBRARY_PATH must be set to a directory!")
endif()
###

#####################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")


# Save libs and executables in separate directories on Unix builds and the same directory on windows
if (UNIX)
  set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Output directory for libraries" )
  set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for applications" )
elseif(WIN32)
  set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}" CACHE PATH "Output directory for libraries" )
  set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}" CACHE PATH "Output directory for applications" )
endif ()

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")


IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)



add_subdirectory( src )
