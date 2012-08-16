#include <AisParserTemplates.h>

//Output Type
#include <AisPostgreSqlDatabaseWriter.h>

//Input Type
#include <AisFlatFileInputSource.h>

//Parser Type
#include <AisSatSentenceParser.h>
#include <AisMsisSentenceParser.h>

int main(int argc, char** argv)
{
	//parse args
	if(argc>9 || argc<8)
	{
		flatfileToDatabaseParserUsage();
		return -1;
	}
	string filename = argv[1];
	string db_user = argv[2];
	string db_pass = argv[3];
	string db_host = argv[4];
	string db_name = argv[5];
	string db_table = argv[6];
	string db_numIterations = argv[7];
	string db_static_table = string("");
	if(argc==9)
	{
		db_static_table= argv[8];
	}

	AisFlatFileInputSource aisInputSource(filename);

	databaseParser<AisPostgreSqlDatabaseWriter, AisSatSentenceParser>(aisInputSource,db_user, db_pass, db_host, db_name, db_table, db_numIterations, db_static_table);
	//databaseParser<AisPostgreSqlDatabaseWriter, AisMsisSentenceParser>(aisInputSource,db_user, db_pass, db_host, db_name, db_table, db_numIterations, db_static_table);

	return 0;
}

