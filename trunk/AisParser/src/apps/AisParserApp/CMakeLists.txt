cmake_minimum_required(VERSION 2.8)
project( AisParserApp )

#QT_DIR must be set for the following
SET (QT_MIN_VERSION "4.2.2")
SET (QT_USE_QT3SUPPORT "ON")
FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS}
	-DQT_CORE_LIB
	-DQT_GUI_LIB
	-DQT_QT3SUPPORT_LIB
	-DQT_THREAD_SUPPORT
)

MACRO (OSSIM_QT4_WRAP_UI outfiles)
  QT4_EXTRACT_OPTIONS(ui_files ui_options ${ARGN})
  FOREACH (it ${ui_files})
    GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
    GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
    IF(UI_DESTINATION_DIR)
      SET(outfile ${UI_DESTINATION_DIR}/ui_${outfile}.h)
    ELSE(UI_DESTINATION_DIR)
      SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/ui_${outfile}.h)
    ENDIF(UI_DESTINATION_DIR)
    ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
      COMMAND ${QT_UIC_EXECUTABLE}
      ARGS ${ui_options} -o ${outfile} ${infile}
      MAIN_DEPENDENCY ${infile})
    SET(${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (it)

ENDMACRO (OSSIM_QT4_WRAP_UI)

INCLUDE_DIRECTORIES(${QT_INCLUDES})
SET(LIB_NAME ossimQt)
FILE(GLOB OSSIMQT_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/ossimQt/ossim*.h")
FILE(GLOB OSSIMQT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )
FILE(GLOB OSSIMQT_UIS  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.ui"


add_executable( LogToTsv LogToTsv.cpp )
FILE(GLOB BOOST_LIBRARIES ${BOOST_LIBRARY_PATH}/*.lib)
set(LIBRARIES ${BOOST_LIBRARIES})
set(INCLUDES ${BOOST_INCLUDE_DIR} ${AIS_PARSER_SRC_DIR})
include_directories( LogToTsv ${INCLUDES})
target_link_libraries( LogToTsv ${LIBRARIES} )
