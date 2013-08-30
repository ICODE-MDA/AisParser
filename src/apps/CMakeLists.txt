cmake_minimum_required(VERSION 2.8)
project( AisParser )
add_subdirectory( CannedAisServer )
add_subdirectory( TcpToTsv )
add_subdirectory( TcpToTcp )
add_subdirectory( LogToTsv )
add_subdirectory( TcpToCsv )
add_subdirectory( LogToCsv )
add_subdirectory( LogToKmlPlacemarks )
add_subdirectory( LogToKmlTracks )

if(BUILD_QT_APPS OR BUILD_ALL_APPS)
add_subdirectory( AisParserApp )
endif()

if(BUILD_ORACLE_APPS OR BUILD_ALL_APPS)
add_subdirectory( TcpToOracleDb )
add_subdirectory( LogToOracleDb )
endif()

if(BUILD_MYSQL_APPS OR BUILD_ALL_APPS)
add_subdirectory( LogToMySqlDb )
add_subdirectory( TcpToMySqlDb )
endif()

if(BUILD_POSTGRESQL_APPS OR BUILD_ALL_APPS)
add_subdirectory( LogToPostgreSqlDb )
add_subdirectory( TcpToPostgreSqlDb )
add_subdirectory( LogToIcodeDb)
add_subdirectory(TcpToPostgreSqlDbWtable)
endif()

if(BUILD_SHAPEFILE_APPS OR BUILD_ALL_APPS)
add_subdirectory( LogToShapefilePoints )
add_subdirectory( LogToShapefileTracks )
endif()



