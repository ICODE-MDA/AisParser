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
	if(argc < 3 || argc > 9)
	{
		trackParserUsage();
		return -1;
	}
	
	string filename = argv[1];
	cout << "filename " << filename << endl;
	cout << "argv[2] " << argv[2] << endl;
	
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
	double urLat, llLat, llLon, urLon;
	int vesselTypeMin, vesselTypeMax;
	if (argc > 3)
	{
		
		llLat = atof(argv[3]);
		cout << "llLat " << llLat << endl;
		urLat = atof(argv[4]);
		cout << "urLat " << urLat << endl;
		llLon = atof(argv[5]);
		cout << "llLon " << llLon << endl;
		urLon = atof(argv[6]);
		cout << "urLon " << urLon << endl;
		
		vesselTypeMin = atoi(argv[7]);
		vesselTypeMax = atoi(argv[8]);

		cout << "vesselTypeMin " << vesselTypeMin << " vesselTypeMax" << vesselTypeMax << endl;
	}
	//Define input class (an AisInputSource)
	//STEPX: choose the correct type of input source
	AisFlatFileInputSource aisInputSource(filename);
	if (argc == 3) 
	{
		trackParser<AisKmlTrackWriter, AisSatSentenceParser>(aisInputSource, filename, tracksPerFile);
	} 
	else
	{
		// Limit track parser by geographic area and vessel type
		cout << "before track parser limited " << endl;
		trackParserLimited<AisKmlTrackWriter, AisSatSentenceParser>(aisInputSource, filename, tracksPerFile, urLat, llLat, urLon, llLon, vesselTypeMin, vesselTypeMax);
	}
	return 0;
}
