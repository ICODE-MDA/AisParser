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
	if(argc != 8 )
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
	string db_table;
	//while (1) {
		const boost::posix_time::ptime utcNow = boost::posix_time::second_clock::universal_time();
	
		cout << "utc date " << utcNow.date() << " hour " << utcNow.time_of_day().hours() <<  " min " << utcNow.time_of_day().minutes();
		cout << " sec " << utcNow.time_of_day().seconds() << endl;

		boost::gregorian::date today = utcNow.date();
		boost::gregorian::date tomorrow = today + boost::gregorian::days(1);
		boost::posix_time::ptime tomorrow_start(tomorrow); //midnight tomorrow
		boost::posix_time::time_duration endTime = tomorrow_start - utcNow;

		//Define input class (an AisInputSource)
		//STEPX: choose the correct type of input source
	
		AisTcpStreamInputSource aisInputSource(host, port, endTime);
		db_table = "ter_" + boost::gregorian::to_iso_string(today);
		
		cout << "Writting TV-32 data to  " << db_table << " for " << endTime.total_seconds() << " seconds" << endl;
		databaseParser<AisPostgreSqlDatabaseWriterSingleAISTable, AisSatSentenceParser>(aisInputSource,db_user, db_pass, 
			db_host, db_name, db_table, db_numIterations, db_table);
	//}
	return 0;
}

