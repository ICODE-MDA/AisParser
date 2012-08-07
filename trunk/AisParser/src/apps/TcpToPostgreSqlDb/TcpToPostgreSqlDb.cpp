#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/timer/timer.hpp>

//Input sources
#include <AisTcpStreamInputSource.h>

//Output sources
#include <AisPostgreSqlDatabaseWriter.h>

//Parsers
#include <AisMsisSentenceParser.h>
#include <AisSatSentenceParser.h>
#include <AisMessageParser.h>

#include <AisMessage.h>
#include <AisDebug.h>

using namespace std;


void usage()
{
	cerr << "AisParserApp.exe <hostname> <port> <db-username> <db-password> <db-hostname> <db-name> <db-table> <db-numIterations>" << endl;
	cerr << "For example:\n AisParserApp.exe localhost 2410 username password databaseserver.example.com exampleDB AISTable 100000" << endl;
}

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================

int main(int argc, char** argv)
{

	//parse args
	if(argc!=9)
	{
		usage();
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

	//Define input class (an AisInputSource)
	//STEPX: choose the correct type of input source
	AisTcpStreamInputSource aisInputSource(host, port);
	
	//Define output class (an AisWriter)
	//STEPX: choose the correct type of output source
	AisMySqlDatabaseWriter aisWriter(db_user, db_pass, db_host, db_name, db_table, boost::lexical_cast<int>(db_numIterations));

	if(!aisWriter.isReady())
	{
		aisDebug("AisWriter is not ready");
		return -1;
	}

	while(aisInputSource.isReady())
	{
		//load the next sentence from the AIS input to the parser
		//STEPX: choose the correct type of sentence parser
		AisMsisSentenceParser aisSentenceParser(aisInputSource.getNextSentence());
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
						//	aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
							throw std::runtime_error("Invalid multipart message");
						}
					}

					AisMessage aisMessage = aisMessageParser.parseMessage();
					//add time from ais sentence to the ais message
					aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
					//add streamid from ais sentence to the ais message
					aisMessage.setSTREAMID(aisSentenceParser.getStreamId());

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
	return 0;
}

