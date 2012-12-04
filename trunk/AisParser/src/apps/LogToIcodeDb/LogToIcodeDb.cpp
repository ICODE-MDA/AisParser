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
	if (argc>10 || argc<6)
	{
		flatfileToDatabaseSchemaUsage();
		return -1;
	}
	string filename = argv[1];
	string db_user = argv[2];
	string db_pass = argv[3];
	string db_host = argv[4];
	string db_name = argv[5];
	string db_numIterations = argv[6];
	string db_dynamic_table = "ais_dynamic";
	string db_target_table = "target_location";
	
	string db_static_table = "ais_static";
	//cout << "Call: " << argv[0] << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << endl;
	//cout << "Number of arguments: " << argc << endl;;
	if(argc==10)
	{
		db_dynamic_table = argv[7];
		db_target_table = argv[8];
		db_static_table= argv[9];
		cout << "Manually setting table names.\n";
	}
	cout << "Dynamic table: " << db_dynamic_table << "\nTarget table: " << db_target_table << "\nStatic table: " << db_static_table << endl;
	cout << "filename: " << filename << " db_host: " <<  db_host << " num_inter: " << db_numIterations << endl;
	AisFlatFileInputSource aisInputSource(filename);

	databaseParserIcodeDb<AisPostgreSqlDatabaseWriter, AisSatSentenceParser>(aisInputSource, db_user, db_pass, db_host, db_name, 
																			 db_dynamic_table, db_numIterations, db_static_table, db_target_table);
	return 0;
}

