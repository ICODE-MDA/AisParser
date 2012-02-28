#if 0
#include <iostream>
#include <string>

//Input sources
#include <AisFlatFileInputSource.h>
#include <AisTcpStreamInputSource.h>

//Output sources
#include <AisDatabaseWriter.h>
#include <AisCsvWriter.h>
#include <AisTsvWriter.h>

//Parsers
#include <AisMsisSentenceParser.h>
#include <AisSatSentenceParser.h>
#include <AisMessageParser.h>

#include <AisMessage.h>
#include <AisDebug.h>

using namespace std;

//aisFileToDatabaseApp.cpp
//aisStreamToCsvApp.cpp
//aisStreamToDbApp

void usage()
{
	//cerr << "AisParserApp.exe <input-filename>" << endl;
	cerr << "AisParserApp.exe <hostname> <port>" << endl;
}

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================

int main(int argc, char** argv)
{

	//parse args
	if(argc!=3)
	{
		usage();
		return -1;
	}

	//string filename = argv[1];
	std::string host = argv[1];
	std::string port = argv[2];

	//string filename = "20111206.log";
	//int port;	

	//Define input class (an AisInputSource)
	//STEPX: choose the correct type of input source
	//AisFlatFileInputSource aisInputSource(filename);
	//std::string host = "localhost";
	//std::string port = "10000";
	AisTcpStreamInputSource aisInputSource(host, port);
	
	//Define output class (an AisWriter)
	//STEPX: choose the correct type of output source
	//AisDatabaseWriter aisWriter("username", "password", "hostname","databaseName","tableName",100000);
	//AisCsvWriter aisWriter("20111206.log.csv");
	AisTsvWriter aisWriter("20111206.log.tsv");

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
				//grab the next message until you have them all
				while(aisSentenceParser.getSentenceNumber() < aisSentenceParser.getNumberOfSentences())
				{
					aisSentenceParser.setSentence(aisInputSource.getNextSentence());
					if(aisSentenceParser.isMessageValid()){
						aisMessageParser.addData(aisSentenceParser.getData());	
					}
					else
					{
						aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
					}
				}

				AisMessage aisMessage = aisMessageParser.parseMessage();
				//add time from ais sentence to the ais message
				aisMessage.setDATETIME(aisSentenceParser.getTimestamp());
				//add streamid from ais sentence to the ais message
				aisMessage.setSTREAMID(aisSentenceParser.getStreamId());

				aisWriter.writeEntry(aisMessage);		
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
