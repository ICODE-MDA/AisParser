#ifndef AisParserTemplates_h
#define AisParserTemplates_h
#include <iostream>
#include <stdexcept>
#include <string>
#include <set>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/timer/timer.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

//Input sources
#include<AisInputSource.h>

//Parsers
#include <AisMessageParser.h>
#include <RadarMessageParser.h>

#include <AisMessage.h>
#include <AisDebug.h>

using namespace std;

void tcpToFlatfileParserUsage()
{
	cerr << "AisParserApp.exe <hostname> <port> <number-of-entries-per-tsv>" << endl;
	cerr << "Will create a file named <input-filename>.p<partition-number>.tsv with the parsed AIS.\nThis file will be suitable for uploading using SQL Loader." << endl;
	cerr << "<number-of-entries-per-tsv> is the number of entries per file.\nIt will create mutliple files named <input-filename><file-number>.tsv.\nIf set to 0, it will push all to a single file." << endl;
}


void flatfileParserUsage()
{
	cerr << "AisParserApp.exe <input-filename> <number-of-entries-per-tsv>" << endl;
	cerr << "Will create a file named <input-filename>.p<partition-number>.tsv with the parsed AIS.\nThis file will be suitable for uploading using SQL Loader." << endl;
	cerr << "<number-of-entries-per-tsv> is the number of entries per file.\nIt will create mutliple files named <input-filename><file-number>.tsv.\nIf set to 0, it will push all to a single file." << endl;
}

template<class OutputType, class AisSentenceParserType>
int flatfileParser(AisInputSource& aisInputSource, string filename, unsigned int messagesPerFile)
{
	boost::timer::auto_cpu_timer timer;
	
	unsigned int partition = 0;

	while(aisInputSource.isReady())
	{
		std::shared_ptr<OutputType> aisWriter;
		
		boost::gregorian::date d(boost::gregorian::day_clock::universal_day());
		//Define output class (an AisWriter)
		//STEPX: choose the correct type of output source
		if(filename == "")
		{
			aisWriter = std::shared_ptr<OutputType>(new OutputType(d.year(), d.month().as_number(), d.day().as_number(), partition++));
		}
		else
		{
			aisWriter = std::shared_ptr<OutputType>(new OutputType(filename + ".p" + boost::lexical_cast<string>(partition++)));
		}

		if(!aisWriter->isReady())
		{
			aisDebug("AisWriter is not ready");
			return -1;
		}

		for(unsigned int messageCount = 0; ((messagesPerFile == 0) || (messageCount < messagesPerFile)) && (aisInputSource.isReady()); messageCount++)
		{

			//load the next sentence from the AIS input to the parser
			//STEPX: choose the correct type of sentence parser
			AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
			AisMessageParser aisMessageParser;

			if(aisSentenceParser.isMessageValid())
			{
				//This check is to make sure that if the first sentence of the message
				//was bad we won't read the second sentence and parse it as a new message
				if(aisSentenceParser.getSentenceNumber()==1)
				{
					aisMessageParser.addData(aisSentenceParser.getData());	
					//if the current sentence is part of a multipart message
					//grab the next message until you have them all, or message is invalid
					try
					{
						while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
						{
							aisSentenceParser.setSentence(aisInputSource.getNextSentence());
							if(aisSentenceParser.isMessageValid()){
								aisMessageParser.addData(aisSentenceParser.getData());	
							}
							else
							{
								//aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
								throw std::runtime_error("Invalid multipart message");
							}
						}

						AisMessage aisMessage = aisMessageParser.parseMessage();
						//add time from ais sentence to the ais message
						aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
						//add streamid from ais sentence to the ais message
						aisMessage.setSTREAMID(aisSentenceParser.getStreamId());

						aisWriter->writeEntry(aisMessage);
					}
					catch(exception &e)
					{
						cerr << e.what() << endl;
					}
				}
				else
				{
					aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
					continue;
				}
			}
			else
			{
				//aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
			}
		}
	}
	return 0;
}

void trackParserUsage()
{
	cerr << "LogToKmlTrack.exe <tracks-per-kml-file> <input-AIS-log-filename> w optional AOI & vessel type <llLat> <urLat> <llLon> <urLon> <vesselTypeMin> <vesselTypeMax>" << endl;
	cerr << "Will create a kml track file input-AIS-log-filename.kml for AIS log file <input-AIS-log-filename>.\nThis file will be suitable for uploading Google Earth." << endl;
}
template<class TrackWriterType, class AisSentenceParserType>
int trackParser(AisInputSource& aisInputSource, string filename, unsigned int tracksPerFile)
{	
	boost::timer::auto_cpu_timer timer;

	unsigned int partition = 0;
	TrackWriterType aisWriter(filename, tracksPerFile);

	while(aisInputSource.isReady())
	{
		while(aisInputSource.isReady())
		{

			//load the next sentence from the AIS input to the parser
			//STEPX: choose the correct type of sentence parser
			AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
			AisMessageParser aisMessageParser;

			if(aisSentenceParser.isMessageValid())
			{
				//This check is to make sure that if the first sentence of the message
				//was bad we won't read the second sentence and parse it as a new message
				if(aisSentenceParser.getSentenceNumber()==1)
				{
					aisMessageParser.addData(aisSentenceParser.getData());	
					//if the current sentence is part of a multipart message
					//grab the next message until you have them all, or message is invalid
					try
					{
						while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
						{
							aisSentenceParser.setSentence(aisInputSource.getNextSentence());
							if(aisSentenceParser.isMessageValid()){
								aisMessageParser.addData(aisSentenceParser.getData());	
							}
							else
							{
								//aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
								throw std::runtime_error("Invalid multipart message");
							}
						}

						AisMessage aisMessage = aisMessageParser.parseMessage();
						//add time from ais sentence to the ais message
						aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
						//add streamid from ais sentence to the ais message
						aisMessage.setSTREAMID(aisSentenceParser.getStreamId());
						//aisWriter.writeEntry(aisMessage);
						aisWriter.writeEntry(aisMessage);
					}
					catch(exception &e)
					{
						cerr << e.what() << endl;
					}
				}
				else
				{
					aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
					continue;
				}
			}
			else
			{
				//aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
			}
		}
	}

	aisWriter.writeToFile();

	return 0;

}

template<class TrackWriterType, class AisSentenceParserType>
int trackParserLimited(AisInputSource& aisInputSource, string filename, unsigned int tracksPerFile, double urLat, double llLat, double urLon, double llLon,
	int vesselTypeMin, int vesselTypeMax)
{	
	boost::timer::auto_cpu_timer timer;

	unsigned int partition = 0;
	TrackWriterType aisWriter(filename, tracksPerFile);

	while(aisInputSource.isReady())
	{
		while(aisInputSource.isReady())
		{

			//load the next sentence from the AIS input to the parser
			//STEPX: choose the correct type of sentence parser
			AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
			AisMessageParser aisMessageParser;

			if(aisSentenceParser.isMessageValid())
			{
				//This check is to make sure that if the first sentence of the message
				//was bad we won't read the second sentence and parse it as a new message
				if(aisSentenceParser.getSentenceNumber()==1)
				{
					aisMessageParser.addData(aisSentenceParser.getData());	
					//if the current sentence is part of a multipart message
					//grab the next message until you have them all, or message is invalid
					try
					{
						while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
						{
							aisSentenceParser.setSentence(aisInputSource.getNextSentence());
							if(aisSentenceParser.isMessageValid()){
								aisMessageParser.addData(aisSentenceParser.getData());	
							}
							else
							{
								//aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
								throw std::runtime_error("Invalid multipart message");
							}
						}

						AisMessage aisMessage = aisMessageParser.parseMessage();
						//add time from ais sentence to the ais message
						aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
						//add streamid from ais sentence to the ais message
						aisMessage.setSTREAMID(aisSentenceParser.getStreamId());
						// Limit tracks by AOI of vesseltype
						double AIS_LAT = aisMessage.getLAT();
						double AIS_LON = aisMessage.getLON();
						int AIS_VESSELTYPE = aisMessage.getVESSELTYPEINT();
						string AIS_VESSELNAME = aisMessage.getVESSELNAME();
						int AIS_MESSAGETYPE = aisMessage.getMESSAGETYPE();
						//Test Dynamic messages for AOI - Static Data message have -999 as default for LAT and LON
						if (AIS_LAT != -999 && AIS_LON != -999)
						{
							
							if (AIS_LAT <= urLat && AIS_LAT >= llLat && AIS_LON <= urLon && AIS_LON >= llLon)
							{
								if (AIS_MESSAGETYPE == 19) //Need to add both static and dynamic data
								{
									aisWriter.writeEntry(aisMessage); //write static data 
								}
								aisWriter.writeEntry(aisMessage);
							}
						}
						else
						{
							aisWriter.writeEntry(aisMessage);
						}

					}
					catch(exception &e)
					{
						cerr << e.what() << endl;
					}
				}
				else
				{
					aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
					continue;
				}
			}
			else
			{
				//aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
			}
		}
	}
	cout << "before writeToFile " << endl;
	aisWriter.writeToFileVesslTypeLimt(vesselTypeMin, vesselTypeMax);

	return 0;

}


void tcpToDatabaseParserUsage()
{
	cerr << "TcpToPostgreSqlDb.exe <TCIP_hostname> <TCIP_port> <db-username> <db-password> <db-hostname> <db-name> <db-table> <db-numIterations>" << endl;
	cerr << "For example:\n AisParserApp.exe localhost 2410 username password databaseserver.example.com exampleDB AISTable 100000" << endl;
}
void tcpToDatabaseParserWtableUsage()
{
	cerr << "TcpToPostgreSqlDbWtable.exe <TCIP_hostname> <TCIP_port> <db-username> <db-password> <db-hostname> <db-name> <db-numIterations> [<aisdb-table> <radardb-table>] " << endl;
	cerr << "For example:\n AisParserApp.exe localhost 2410 username password databaseserver.example.com exampleDB 100000" << endl;
}
void flatfileToDatabaseParserUsage()
{
	cerr << "This application will parse and push AIS messages to a database specified on the command line." << endl;
	cerr << "If you specify a static table, it will push static messages to the static table, and dynamic messages to the dynamic table" << endl;
	cerr << "AisParserApp.exe <input-filename> <db-username> <db-password> <db-hostname> <db-name> <db-table> <db-numIterations> [<db-static-table>]" << endl;
	cerr << "For example:\n AisParserApp.exe 20111010.log username password databaseserver.example.com exampleDB AISTable 100000 staticAISTable" << endl;
	cerr << "...OR..." << endl;
	cerr << "For example:\n AisParserApp.exe 20111010.log username password databaseserver.example.com exampleDB AISTable 100000" << endl;
}
void LogToPostgreSqlDb()
{
	cerr << "This application will parse and push AIS messages to a database specified on the command line." << endl;
	cerr << "If you specify a radar table, it will push radar messages to the radar table;otherwise, only AIS messages are pushed to AIStable specified" << endl;
	cerr << "Optional Usage: the user may specific a start and stop unix time to limit the data pasred into the database tables" << endl;
	cerr << "LogToPostgreSqlDb.exe <input-filename> <db-username> <db-password> <db-hostname> <db-name> <db-table> <db-numIterations> [<db-radar-table> <unix start time> <unix end time>]" << endl;
	cerr << "For example:\n AisParserApp.exe 20111010.log username password databaseserver.example.com exampleDB AISTable 100000 radarTable" << endl;
	cerr << "...OR..." << endl;
	cerr << "For example:\n AisParserApp.exe 20111010.log username password databaseserver.example.com exampleDB AISTable 100000" << endl;
	cerr << "...OR..." << endl;
	cerr << "For example:\n AisParserApp.exe 20111010.log username password databaseserver.example.com exampleDB AISTable 100000 radarTable 1375806152 1375806158" << endl;
}

template<class DatabaseWriterType, class AisSentenceParserType>
int databaseParser(AisInputSource& aisInputSource,string db_user, string db_pass, string db_host, string db_name, string db_table, string db_numIterations, string db_static_table )
{
	boost::timer::auto_cpu_timer timer;

	bool splitStaticAndDynamic = false;
	if(db_static_table!="")
	{
		splitStaticAndDynamic = true;
	}
	
	
	//Define output class (an AisWriter)
	//STEPX: choose the correct type of output source
	DatabaseWriterType aisWriterD(db_user, db_pass, db_host, db_name, db_table, boost::lexical_cast<int>(db_numIterations));
	std::shared_ptr<DatabaseWriterType> aisWriterS;
	if(splitStaticAndDynamic){
		aisWriterS = std::shared_ptr<DatabaseWriterType>(new DatabaseWriterType(db_user, db_pass, db_host, db_name, db_static_table, boost::lexical_cast<int>(db_numIterations)));
		if(!aisWriterS->isReady())
		{
			aisDebug("AisWriter is not ready");
			return -1;
		}
	}

	if(!aisWriterD.isReady())
	{
		aisDebug("AisWriter is not ready");
		return -1;
	}
	while(aisInputSource.isReady())
	{
		//load the next sentence from the AIS input to the parser
		//STEPX: choose the correct type of sentence parser
		AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
		AisMessageParser aisMessageParser;

		if(aisSentenceParser.isMessageValid())
		{
			//This check is to make sure that if the first sentence of the message
			//was bad we won't read the second sentence and parse it as a new message
			if(aisSentenceParser.getSentenceNumber()==1)
			{
				aisMessageParser.addData(aisSentenceParser.getData());	
				//if the current sentence is part of a multipart message
				//grab the next message until you have them all, or message is invalid
				try
				{
				while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
				{
					aisSentenceParser.setSentence(aisInputSource.getNextSentence());
					if(aisSentenceParser.isMessageValid()){
						aisMessageParser.addData(aisSentenceParser.getData());	
					}
					else
					{
						//aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
						throw std::runtime_error("Invalid multipart message");
					}
				}

				AisMessage aisMessage = aisMessageParser.parseMessage();
				//add time from ais sentence to the ais message
				aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
				//add streamid from ais sentence to the ais message
				aisMessage.setSTREAMID(aisSentenceParser.getStreamId());

				int message_type = aisMessage.getMESSAGETYPE();
				//check if static AIS message type
				if ((message_type == 5 || message_type == 24) && splitStaticAndDynamic)
				{
					aisWriterS->writeEntry(aisMessage);
				}
				else
				{
					aisWriterD.writeEntry(aisMessage);
				}
						
				}
				catch(exception &e)
				{
					cerr << e.what() << endl;
				}
			}
			else
			{
				aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
				continue;
			}
		}
		else
		{
			//aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
		}
	}
	return 0;
}
template<class DatabaseWriterType, class AisSentenceParserType>
int databaseParserLimitTime(AisInputSource& aisInputSource,string db_user, string db_pass, string db_host, string db_name, string db_table, 
	string db_numIterations, string db_static_table, int unix_start, int unix_end)
{
	boost::timer::auto_cpu_timer timer;

	bool splitStaticAndDynamic = false;
	if(db_static_table!="")
	{
		splitStaticAndDynamic = true;
	}
	
	
	//Define output class (an AisWriter)
	//STEPX: choose the correct type of output source
	
	DatabaseWriterType aisWriterD(db_user, db_pass, db_host, db_name, db_table, boost::lexical_cast<int>(db_numIterations));
	std::shared_ptr<DatabaseWriterType> aisWriterS;
	if(splitStaticAndDynamic){
		aisWriterS = std::shared_ptr<DatabaseWriterType>(new DatabaseWriterType(db_user, db_pass, db_host, db_name, db_static_table, 
			boost::lexical_cast<int>(db_numIterations)));
		if(!aisWriterS->isReady())
		{
			aisDebug("AisWriter is not ready");
			return -1;
		}
	}

	if(!aisWriterD.isReady())
	{
		aisDebug("AisWriter is not ready");
		return -1;
	}
	while(aisInputSource.isReady())
	{
		//load the next sentence from the AIS input to the parser
		//STEPX: choose the correct type of sentence parser
		AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
		AisMessageParser aisMessageParser;

		if(aisSentenceParser.isMessageValid())
		{
			//This check is to make sure that if the first sentence of the message
			//was bad we won't read the second sentence and parse it as a new message
			if(aisSentenceParser.getSentenceNumber()==1)
			{
				aisMessageParser.addData(aisSentenceParser.getData());	
				//if the current sentence is part of a multipart message
				//grab the next message until you have them all, or message is invalid
				try
				{
				while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
				{
					aisSentenceParser.setSentence(aisInputSource.getNextSentence());
					if(aisSentenceParser.isMessageValid()){
						aisMessageParser.addData(aisSentenceParser.getData());	
					}
					else
					{
						//aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
						throw std::runtime_error("Invalid multipart message");
					}
				}

				AisMessage aisMessage = aisMessageParser.parseMessage();
				//add time from ais sentence to the ais message
				aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
				//add streamid from ais sentence to the ais message
				aisMessage.setSTREAMID(aisSentenceParser.getStreamId());

				int message_type = aisMessage.getMESSAGETYPE();
				//check if static AIS message type and within time period
				
				if ((unix_start <= aisMessage.getDATETIME()) && (aisMessage.getDATETIME() <= unix_end))
				{
					if ((message_type == 5 || message_type == 24) && splitStaticAndDynamic)
					{
						aisWriterS->writeEntry(aisMessage);
					}
					else
					{
						aisWriterD.writeEntry(aisMessage);
					}	
				}
				}
				catch(exception &e)
				{
					cerr << e.what() << endl;
				}
			}
			else if (aisSentenceParser.isRadarData())
			{
				RadarMessage radarMessage = radarMessageParser.parseRadar(aisSentenceParser.getRadarData());
			}
			else
			{
				aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
				continue;
			}
		}
		else
		{
			//aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
		}
	}
	return 0;
}

template<class DatabaseWriterType, class AisSentenceParserType>
int databaseParserAIS_Radar(AisInputSource& aisInputSource,string db_user, string db_pass, string db_host, string db_name, string aisDb_table, 
	string db_numIterations, string radarDb_table, bool create_RadarTable, bool create_AISTable)
{
	boost::timer::auto_cpu_timer timer;

	/*bool splitAISAndRadar = false;
	if(radarDb_table!="")
	{
		splitAISAndRadar = true;
	}*/
	
	
	//Define output class (an AisWriter)
	//STEPX: choose the correct type of output source
	DatabaseWriterType aisWriterAis(db_user, db_pass, db_host, db_name, aisDb_table, boost::lexical_cast<int>(db_numIterations),create_RadarTable, create_AISTable);
	std::shared_ptr<DatabaseWriterType> aisWriterRadar;
	
	aisWriterRadar = std::shared_ptr<DatabaseWriterType>(new DatabaseWriterType(db_user, db_pass, db_host, db_name, radarDb_table, 
			boost::lexical_cast<int>(db_numIterations),create_RadarTable, create_AISTable));
	if(!aisWriterRadar->isReady())
	{
		aisDebug("AisWriterRadar is not ready");
		return -1;
	}
	

	if(!aisWriterAis.isReady())
	{
		aisDebug("AisWriter is not ready");
		return -1;
	}
	while(aisInputSource.isReady())
	{
		//load the next sentence from the AIS input to the parser
		//STEPX: choose the correct type of sentence parser
		AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
		AisMessageParser aisMessageParser;
		RadarMessageParser radarMessageParser;

		if(aisSentenceParser.isMessageValid())
		{
			//This check is to make sure that if the first sentence of the message
			//was bad we won't read the second sentence and parse it as a new message
			if(aisSentenceParser.getSentenceNumber()==1)
			{
				aisMessageParser.addData(aisSentenceParser.getData());	
				//if the current sentence is part of a multipart message
				//grab the next message until you have them all, or message is invalid
				try
				{
					while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
					{
						aisSentenceParser.setSentence(aisInputSource.getNextSentence());
						if(aisSentenceParser.isMessageValid()){
							aisMessageParser.addData(aisSentenceParser.getData());	
						}
						else
						{
							//aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
							throw std::runtime_error("Invalid multipart message");
						}
					}

					AisMessage aisMessage = aisMessageParser.parseMessage();
					//add time from ais sentence to the ais message
					aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
					//add streamid from ais sentence to the ais message
					aisMessage.setSTREAMID(aisSentenceParser.getStreamId());
					aisWriterAis.writeEntry(aisMessage);
					
				}
				catch(exception &e)
				{
					cerr << e.what() << endl;
				}
			}
			else if (aisSentenceParser.isRadarData())
			{
				RadarMessage radarMessage = radarMessageParser.parseRadar(aisSentenceParser.getRadarData());
				aisWriterRadar->writeEntryRadar(radarMessage);
				
			}
			else
			{
				aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
				continue;
			}
		}
		else
		{
			//aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
		}
	}
	return 0;
}
template<class DatabaseWriterType, class AisSentenceParserType>
int databaseParserAIS_RadarLimitTime(AisInputSource& aisInputSource,string db_user, string db_pass, string db_host, string db_name, string aisDb_table, 
	string db_numIterations, string radarDb_table, int unix_start, int unix_end, bool create_RadarTable, bool create_AISTable)
{
	boost::timer::auto_cpu_timer timer;

	/*bool splitAISAndRadar = false;
	if(radarDb_table!="")
	{
		splitAISAndRadar = true;
	}*/
	
	
	//Define output class (an AisWriter)
	//STEPX: choose the correct type of output source
	DatabaseWriterType aisWriterAis(db_user, db_pass, db_host, db_name, aisDb_table, boost::lexical_cast<int>(db_numIterations),create_RadarTable, create_AISTable);
	std::shared_ptr<DatabaseWriterType> aisWriterRadar;
	aisWriterRadar = std::shared_ptr<DatabaseWriterType>(new DatabaseWriterType(db_user, db_pass, db_host, db_name, radarDb_table, 
	boost::lexical_cast<int>(db_numIterations),create_RadarTable, create_AISTable));
	if(!aisWriterRadar->isReady())
	{
		aisDebug("AisWriterRadra is not ready");
		return -1;
	}
	

	if(!aisWriterAis.isReady())
	{
		aisDebug("AisWriter is not ready");
		return -1;
	}
	while(aisInputSource.isReady())
	{
		//load the next sentence from the AIS input to the parser
		//STEPX: choose the correct type of sentence parser
		AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
		AisMessageParser aisMessageParser;
		RadarMessageParser radarMessageParser;

		if(aisSentenceParser.isMessageValid())
		{
			//This check is to make sure that if the first sentence of the message
			//was bad we won't read the second sentence and parse it as a new message
			if(aisSentenceParser.getSentenceNumber()==1)
			{
				aisMessageParser.addData(aisSentenceParser.getData());	
				//if the current sentence is part of a multipart message
				//grab the next message until you have them all, or message is invalid
				try
				{
					while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
					{
						aisSentenceParser.setSentence(aisInputSource.getNextSentence());
						if(aisSentenceParser.isMessageValid()){
							aisMessageParser.addData(aisSentenceParser.getData());	
						}
						else
						{
							//aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
							throw std::runtime_error("Invalid multipart message");
						}
					}

					AisMessage aisMessage = aisMessageParser.parseMessage();
					//add time from ais sentence to the ais message
					aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
					//add streamid from ais sentence to the ais message
					aisMessage.setSTREAMID(aisSentenceParser.getStreamId());
					if ((unix_start <= aisMessage.getDATETIME()) && (aisMessage.getDATETIME() <= unix_end))
					{
						aisWriterAis.writeEntry(aisMessage);
					}
				}
				catch(exception &e)
				{
					cerr << e.what() << endl;
				}
			}
			else if (aisSentenceParser.isRadarData())
			{
				RadarMessage radarMessage = radarMessageParser.parseRadar(aisSentenceParser.getRadarData());
				if ((unix_start <= radarMessage.getDATETIME()) && (radarMessage.getDATETIME() <= unix_end))
				{
						aisWriterRadar->writeEntryRadar(radarMessage);
				}
			}
			else
			{
				aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
				continue;
			}
		}
		else
		{
			//aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
		}
	}
	return 0;
}
void flatfileToDatabaseSchemaUsage()
{
	cerr << "This application will parse and push AIS messages to a database specified on the command line." << endl;
	cerr << "You may specify the following AIS tables: dynamic, static, and target location tables; otherwise";
	cerr <<	"the following default tables will be used: AIS_Dynamic, AIS_Static, and Target_Location." << endl;
	cerr << "LogToIcodeDb.exe <input-filename> <db-username> <db-password> <db-hostname> <db-name> <db-numIterations> ";
	cerr << "[<db-dynamic-table> <db-static-table> <db-target-table>]." << endl;
	cerr << "For example:\n LogToIcodeDb.exe.exe 20111010.log username password databaseserver.example.com exampleDB 100000" << endl;
	cerr << "...OR..." << endl;
	cerr << "For example:\n LogToIcodeDb.exe.exe 20111010.log username password databaseserver.example.com exampleDB 100000 ";
	cerr <<	"dynamicAISTable staticAISTable targetAISTable" << endl;
}

template<class DatabaseWriterType, class AisSentenceParserType>
int databaseParserIcodeDb(AisInputSource& aisInputSource,string db_user, string db_pass, string db_host, string db_name, 
	string db_dynamic_table, string db_numIterations, string db_static_table, string db_target_table)
{
	boost::timer::auto_cpu_timer timer;

	std::shared_ptr<DatabaseWriterType> aisWriter;
	aisWriter = std::shared_ptr<DatabaseWriterType>(new DatabaseWriterType(db_user, db_pass, db_host, db_name, db_static_table, db_dynamic_table, boost::lexical_cast<int>(db_numIterations)));
	if(!aisWriter->isReady())
	{
		aisDebug("AisWriter is not ready");
		return -1;
	}
	
	
	
	
	while(aisInputSource.isReady())
	{
		//load the next sentence from the AIS input to the parser
		//STEPX: choose the correct type of sentence parser
		AisSentenceParserType aisSentenceParser(aisInputSource.getNextSentence());
		AisMessageParser aisMessageParser;

		if(aisSentenceParser.isMessageValid())
		{
			//This check is to make sure that if the first sentence of the message
			//was bad we won't read the second sentence and parse it as a new message
			if(aisSentenceParser.getSentenceNumber()==1)
			{
				aisMessageParser.addData(aisSentenceParser.getData());	
				//if the current sentence is part of a multipart message
				//grab the next message until you have them all, or message is invalid
				try
				{
				while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
				{
					aisSentenceParser.setSentence(aisInputSource.getNextSentence());
					if(aisSentenceParser.isMessageValid()){
						aisMessageParser.addData(aisSentenceParser.getData());	
					}
					else
					{
						//aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
						throw std::runtime_error("Invalid multipart message");
					}
				}

				AisMessage aisMessage = aisMessageParser.parseMessage();
				//add time from ais sentence to the ais message
				aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
				//add streamid from ais sentence to the ais message
				aisMessage.setSTREAMID(aisSentenceParser.getStreamId());

				//aisDebug("*** writing to table ***");
				aisWriter->writeEntry(aisMessage);
				}
				catch(exception &e)
				{
					cerr << e.what() << endl;
				}
			}
			else
			{
				aisDebug("First sentence of message was invalid/not receieved.\nSkipping the rest of the sentences of this message");
				continue;
			}
		}
		else
		{
			//aisDebug("Invalid message:\n" + aisSentenceParser.getCurrentSentence());
		}
	}
	return 0;
}
#endif