#include <AisParserTemplates.h>

//Input sources
#include <AisFlatFileInputSource.h>

//Output sources
#include <AisKmlTrackWriter.h>

//Parser Type
#include <AisSatSentenceParser.h>


int main(int argc, char** argv)
{
	//parse args
	if(argc!=3)
	{
		trackParserUsage();
		return -1;
	}

	string filename = argv[1];
	unsigned int tracksPerFile = 0;
	try
	{
		tracksPerFile = boost::lexical_cast<unsigned int>(argv[2]);
	}
	catch(exception& e)
	{
		cerr << e.what() << endl;
		cerr << "Setting messages per file to 0" << endl;
		tracksPerFile = 0;
	}
	
	//Define input class (an AisInputSource)
	//STEPX: choose the correct type of input source
	AisFlatFileInputSource aisInputSource(filename);

	trackParser<AisKmlTrackWriter, AisSatSentenceParser>(aisInputSource, filename, tracksPerFile);
	return 0;
}
