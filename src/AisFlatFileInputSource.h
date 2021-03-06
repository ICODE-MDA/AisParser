#ifndef AisFlatFileInputSource_h
#define AisFlatFileInputSource_h

#include <fstream>
#include <string>

#include <AisInputSource.h>

using namespace std;

/**
This class handles grabbing external AIS data
The class can grab data from a flatfile containing 
messages such as log or gnm
*/
class AisFlatFileInputSource: public AisInputSource
{
public:
	AisFlatFileInputSource(std::string filename)
	{
		m_inputFile.open(filename, std::ios::in);
	}
	
	~AisFlatFileInputSource()
	{
		if(m_inputFile.is_open())
		{
			m_inputFile.close();
		}
	}

	/**
	Returns true if the input source is ready
	to start providing data, false otherwhise
	*/
	bool isReady()
	{
		return m_inputFile.good();
	}
	
	/**
	Returns the next sentence from the AIS
	input source as a string.
	*/
	string getNextSentence()
	{
		getline(m_inputFile, m_sentence); 
		return m_sentence;
	}


private:
	string m_sentence;
	ifstream m_inputFile;
};
#endif
