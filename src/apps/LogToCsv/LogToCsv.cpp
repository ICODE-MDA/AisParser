#include <AisParserTemplates.h>

//Input sources
#include <AisFlatFileInputSource.h>

//Output sources
#include <AisCsvWriter.h>

//Parsers
#include <AisSatSentenceParser.h>


int main(int argc, char** argv)
{

	//parse args
	if(argc!=3)
	{
		flatfileParserUsage();
		return -1;
	}

	string filename = argv[1];
	unsigned int messagesPerFile = 0;
	try
	{
		messagesPerFile = boost::lexical_cast<unsigned int>(argv[2]);
	}
	catch(exception& e)
	{
		cerr << e.what() << endl;
		cerr << "Setting messages per file to 0" << endl;
		messagesPerFile = 0;
	}

		
	//Define input class (an AisInputSource)
	//STEPX: choose the correct type of input source
	AisFlatFileInputSource aisInputSource(filename);

	flatfileParser<AisCsvWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);

	return 0;
}

