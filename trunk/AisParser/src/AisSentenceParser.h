#ifndef AisSentenceParser_h
#define AisSentenceParser_h

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <AisDebug.h>

/**
This class parses AIS sentences into a vector
of strings that can be used in other classes.
The class has the ability to validate the
checksum of the AIS message
*/
class AisSentenceParser{
public:
	/**
	@param sentence is the sentence to parse
	@param numFields is the number of fields in the sentence (e.g. sentence == a,b,c,d,e would mean that numFields == 5)
	*/
	AisSentenceParser(std::string sentence){
		m_fullSentence = sentence;
		boost::split(m_parsedSentence, sentence, boost::is_any_of(","));
		m_numberOfSentences = 0;
		m_currentSentenceNumber = 0;
	}

	void setSentence(std::string sentence){
		m_fullSentence = sentence;
		boost::split(m_parsedSentence, sentence, boost::is_any_of(","));
	}

	std::vector<std::string> getParsedSentence(){
		return m_parsedSentence;
	}

	/**
	Returns the current sentence
	*/
	std::string getCurrentSentence()
	{
		return m_fullSentence;
	}


	virtual bool isMessageValid() = 0;


	virtual bool isChecksumValid() = 0;

	/**	
	Call isMessageValid() before using this function
	*/
	virtual int getTimestamp() = 0;

	/**	
	Call isMessageValid() before using this function
	*/
	virtual std::string getStreamId() = 0;

	/**
	Call isMessageValid() before using this function
	*/
	virtual int getNumberOfSentences() = 0;

	/**
	Call isMessageValid() before using this function
	*/
	virtual int getSentenceNumber() = 0;

	/**
	Call isChecksumValid() before using this function
	*/
	virtual std::string getData() = 0;

protected:
	std::string m_fullSentence;
	std::vector<std::string> m_parsedSentence;
	int m_numberOfSentences;
	int m_currentSentenceNumber;
};

#endif
