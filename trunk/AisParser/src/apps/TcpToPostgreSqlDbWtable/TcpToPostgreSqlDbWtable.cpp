#include <AisParserTemplates.h>

//Output Type
#include <AisPostgreSqlDatabaseWriterRadarAISTable.h>

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
	if(argc !=8 && argc != 10 )
	{
		tcpToDatabaseParserWtableUsage();
		return -1;
	}
	
	boost::timer::auto_cpu_timer timer;

	std::string host = argv[1];
	std::string port = argv[2];
	string db_user = argv[3];
	string db_pass = argv[4];
	string db_host = argv[5];
	string db_name = argv[6];
	string db_numIterations = argv[7];
	string aisDb_table;
	string radarDb_table;
	bool createDbTableName = true;
	bool create_RadarTable = true;
	bool create_AISTable = true;
	if (argc == 10)
	{
		aisDb_table = argv[8];
		radarDb_table = argv[9];
		createDbTableName = false;
	}
	while (1) {
		//Needed to declare AisTcpStreamInputSource for each case - create or not create a new table
		if (createDbTableName) 
		{
			const boost::posix_time::ptime utcNow = boost::posix_time::second_clock::universal_time();
	
			//cout << "utc date " << utcNow.date() << " hour " << utcNow.time_of_day().hours() <<  " min " << utcNow.time_of_day().minutes();
			//cout << " sec " << utcNow.time_of_day().seconds() << endl;

			boost::gregorian::date today = utcNow.date();
			boost::gregorian::date tomorrow = today + boost::gregorian::days(1);
			boost::posix_time::ptime tomorrow_start(tomorrow); //midnight tomorrow
			boost::posix_time::time_duration endTime = tomorrow_start - utcNow;

			//Define input class (an AisInputSource)
			//STEPX: choose the correct type of input source
			
			AisTcpStreamInputSource aisInputSource(host, port, endTime);
			aisDb_table = "ter_" + boost::gregorian::to_iso_string(today);
			radarDb_table = "radar_" + boost::gregorian::to_iso_string(today);
			cout << "aisDb_table " << aisDb_table << " radarDb_table " << radarDb_table << endl;
			databaseParserAIS_Radar<AisPostgreSqlDatabaseWriterRadarAISTable, AisSatSentenceParser>(aisInputSource,db_user, db_pass, db_host, db_name, aisDb_table, 
				db_numIterations, radarDb_table, create_RadarTable, create_AISTable);
		
			cout << "Writting TV-32 aid data to  " << aisDb_table << " and Radar to " << radarDb_table << " for " << endTime.total_seconds() << " seconds" << endl;
		} else
		{
			//Define input class (an AisInputSource)
			//STEPX: choose the correct type of input source
	
			AisTcpStreamInputSource aisInputSource(host, port);
			databaseParserAIS_Radar<AisPostgreSqlDatabaseWriterRadarAISTable, AisSatSentenceParser>(aisInputSource,db_user, db_pass, db_host, db_name, aisDb_table, 
				db_numIterations, radarDb_table, create_RadarTable, create_AISTable);
		}
	}
	return 0;
}

