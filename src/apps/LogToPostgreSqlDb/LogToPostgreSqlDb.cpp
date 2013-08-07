#include <AisParserTemplates.h>

//Output Type
#include <AisPostgreSqlDatabaseWriterSingleAISTable.h>
#include <AisPostgreSqlDatabaseWriterRadarAISTable.h>

//Input Type
#include <AisFlatFileInputSource.h>

//Parser Type
#include <AisSatSentenceParser.h>
#include <AisMsisSentenceParser.h>

int main(int argc, char** argv)
{
	//parse args
	cout << "argc " << argc << endl;
	if(argc!=8 && argc!=9 && argc!=11)
	{
		LogToPostgreSqlDb();
		return -1;
	}
	string filename = argv[1];
	string db_user = argv[2];
	string db_pass = argv[3];
	string db_host = argv[4];
	string db_name = argv[5];
	string db_table = argv[6];
	string db_numIterations = argv[7];
	bool parseRadarData = false;
	bool timeLimit = false;
	bool create_RadarTable = true;
	bool create_AISTable = true;
	string db_radar_table;
	if (argc == 9)
	{
		parseRadarData = true;
		db_radar_table = argv[8];
	}
	int unix_start = 0;
	int unix_end = 0;
	if(argc==11)
	{
		timeLimit = true;
		parseRadarData = true;
		db_radar_table = argv[8];
		unix_start = atoi(argv[9]); 
		unix_end = atoi(argv[10]);
	}
	
	if (unix_end < unix_start) 
	{
		cerr << "invalid start time entered: end time " << unix_end << " < " <<  "start_time " << unix_start << endl;
		return -1;
	}

	AisFlatFileInputSource aisInputSource(filename);
	string db_static_table = "";
	if (timeLimit) //create tables if they don't exist
	{
		databaseParserAIS_RadarLimitTime<AisPostgreSqlDatabaseWriterRadarAISTable, AisSatSentenceParser>(aisInputSource,db_user, db_pass, db_host, 
		db_name, db_table, db_numIterations, db_radar_table, unix_start, unix_end,create_RadarTable, create_AISTable);
	} else if (!timeLimit && parseRadarData) //create tables if they don't exist
	{
		databaseParserAIS_Radar<AisPostgreSqlDatabaseWriterRadarAISTable, AisSatSentenceParser>(aisInputSource,db_user, db_pass, db_host, db_name, db_table, 
				db_numIterations, db_radar_table, create_RadarTable, create_AISTable);
	} else // parse only AIS and skip radar data and does not create table
	{
		databaseParser<AisPostgreSqlDatabaseWriterSingleAISTable, AisSatSentenceParser>(aisInputSource,db_user, db_pass, db_host, 
		db_name, db_table, db_numIterations, db_static_table);
	
	}

	return 0;
}