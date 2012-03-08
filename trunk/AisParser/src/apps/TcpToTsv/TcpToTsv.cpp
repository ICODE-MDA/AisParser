#include <iostream>
#include <string>

#include <boost/timer/timer.hpp>

//Input sources
//#include <AisFlatFileInputSource.h>
#include <AisTcpStreamInputSource.h>

//Output sources
//#include <AisDatabaseWriter.h>
//#include <AisCsvWriter.h>
#include <AisTsvWriter.h>

//Parsers
#include <AisMsisSentenceParser.h>
#include <AisSatSentenceParser.h>
#include <AisMessageParser.h>

#include <AisMessage.h>
#include <AisDebug.h>

using namespace std;

void usage()
{
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

	boost::timer::auto_cpu_timer timer;

	std::string host = argv[1];
	std::string port = argv[2];

	//Define input class (an AisInputSource)
	//STEPX: choose the correct type of input source
	AisTcpStreamInputSource aisInputSource(host, port);
	
	//Define output class (an AisWriter)
	//STEPX: choose the correct type of output source
	AisTsvWriter aisWriter(host+"port"+port+".tsv");

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
							//aisDebug("Invalid multipart message:\n" + aisSentenceParser.getCurrentSentence());
							throw exception("Invalid multipart message");
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

