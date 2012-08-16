#include <AisParserTemplates.h>

//Input sources
#include <AisFlatFileInputSource.h>

//Output sources
#include <AisKmlPlacemarkWriter.h>

//Parsers
#include <AisSatSentenceParser.h>


void usage()
{
	cerr << "AisParserApp.exe <input-filename> <number-of-entries-per-kml>" << endl;
	cerr << "Will create a file named <input-filename>.p<partition-number>.kml with the parsed AIS.\nThis file will be suitable for opening in Google Earth." << endl;
	cerr << "<number-of-entries-per-kml> is the number of entries per file.\nIt will create mutliple files named <input-filename><file-number>.kml.\nIf set to 0, it will push all to a single file." << endl;
}

int main(int argc, char** argv)
{

	//parse args
	if(argc!=3)
	{
		usage();
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

	flatfileParser<AisKmlPlacemarkWriter, AisSatSentenceParser>(aisInputSource, filename, messagesPerFile);

	return 0;
}


