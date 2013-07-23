#include <AisParserTemplates.h>

//Output Type
#include <AisPostgreSqlDatabaseWriterSingleAISTable.h>

//Input Type
#include <AisTcpStreamInputSource.h>

//Parser Type
#include <AisSatSentenceParser.h>

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================


int main(int argc, char** argv)
{
	//parse args
	if(argc>9 || argc<8)
	{
		tcpToDatabaseParserUsage();
		return -1;
	}
	

	boost::timer::auto_cpu_timer timer;

	std::string host = argv[1];
	std::string port = argv[2];
	string db_user = argv[3];
	string db_pass = argv[4];
	string db_host = argv[5];
	string db_name = argv[6];
	string db_table = argv[7];
	string db_numIterations = argv[8];
	//string db_static_table = string("");
	/*if(argc==9)
	{
		db_static_table= argv[8];
	}*/
	
	//Define input class (an AisInputSource)
	//STEPX: choose the correct type of input source
	AisTcpStreamInputSource aisInputSource(host, port);

	databaseParser<AisPostgreSqlDatabaseWriterSingleAISTable, AisSatSentenceParser>(aisInputSource,db_user, db_pass, db_host, db_name, db_table, db_numIterations, db_table);
	return 0;
}

