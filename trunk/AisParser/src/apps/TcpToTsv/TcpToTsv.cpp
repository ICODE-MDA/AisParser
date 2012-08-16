#include <AisParserTemplates.h>

//Input sources
#include <AisTcpStreamInputSource.h>

//Output sources
#include <AisTsvWriter.h>

//Parsers
#include <AisSatSentenceParser.h>


int main(int argc, char** argv)
{
	//parse args
	if(argc!=4)
	{
		tcpToFlatfileParserUsage();
		return -1;
	}

	std::string host = argv[1];
	std::string port = argv[2];
	
	unsigned int messagesPerFile = 0;
	try
	{
		messagesPerFile = boost::lexical_cast<unsigned int>(argv[3]);
	}
	catch(exception& e)
	{
		cerr << e.what() << endl;
		cerr << "Setting messages per file to 0" << endl;
		messagesPerFile = 0;
	}

	//Define input class (an AisInputSource)
	//STEPX: choose the correct type of input source
	AisTcpStreamInputSource aisInputSource(host, port);

	flatfileParser<AisTsvWriter, AisSatSentenceParser>(aisInputSource, "", messagesPerFile);
	
	return 0;
}

