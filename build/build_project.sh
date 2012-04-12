boost_dir="/home/bbagnall/code/boost/boost_1_48_0"
ais_parser_src_dir="/home/bbagnall/code/icode/trunk/AisParser/src"
oracle_includes="/home/bbagnall/code/oracle/instantclient-sdk-nt-11.2.0.2.0/include"
oracle_library_dir="/home/bbagnall/code/oracle/instantclient-sdk-nt-11.2.0.2.0/lib"
mysql_includes="/home/bbagnall/code/mysql/mysql-connector/v110/include"
mysql_library_dir="/home/bbagnall/code/mysql/mysql-connector/v110/lib"
cmake -G "Unix Makefiles" -DAIS_PARSER_SRC_DIR=$ais_parser_src_dir -DORACLE_INCLUDE_DIR=$oracle_includes -DORACLE_LIBRARY_PATH=$oracle_library_dir -DBOOST_INCLUDE_DIR=$boost_dir -DBOOST_LIBRARY_PATH=$boost_dir/stage/lib -DMYSQL_INCLUDE_DIR=$mysql_includes -DMYSQL_LIBRARY_PATH=$mysql_library_dir ../src
