cmake_minimum_required(VERSION 2.8)
project( TcpToCsv )
add_executable( TcpToCsv TcpToCsv.cpp )
FILE(GLOB BOOST_LIBRARIES ${BOOST_LIBRARY_PATH}/*.lib)
set(LIBRARIES ${BOOST_LIBRARIES})
set(INCLUDES ${BOOST_INCLUDE_DIR} ${AIS_PARSER_SRC_DIR})
include_directories( TcpToCsv ${INCLUDES})
target_link_libraries( TcpToCsv ${LIBRARIES} )
