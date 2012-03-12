#ifndef AisSatSentenceParser_h
#define AisSatSentenceParser_h

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <AisSentenceParser.h>

/**
This class parses AIS sentences into a vector
of strings that can be used in other classes.
The class has the ability to validate the
checksum of the AIS message
*/
class AisSatSentenceParser : public AisSentenceParser{
public:
	/**
	@param sentence is the sentence to parse
	@param numFields is the number of fields in the sentence (e.g. sentence == a,b,c,d,e would mean that numFields == 5)
	*/
	AisSatSentenceParser(std::string sentence): AisSentenceParser(sentence)
	{
	}


	bool isMessageValid(){
		if(m_parsedSentence.size() == 9 || m_parsedSentence.size() == 10 )
		{
			try
			{
				m_numberOfSentences = boost::lexical_cast<int>(m_parsedSentence[1]);
				m_currentSentenceNumber = boost::lexical_cast<int>(m_parsedSentence[2]);
			}
			catch(std::exception &e)
			{
				aisDebug(e.what() << "\nMessage:" << m_fullSentence);
				return false;
			}

			if(m_parsedSentence[0].size() == 6)
			{
				if(m_parsedSentence[0].substr(3,3).compare("VDM") == 0)
				{
					return isChecksumValid();
				}
				else
				{
					aisDebug("Message does not have !**VDM begining\n" + m_fullSentence);
					return false;
				}
			}
			else
			{
				aisDebug("Message does not have acceptable first entry\n" + m_fullSentence);
				return false;
			}
		}
		else
		{
			aisDebug("Message does not have 9||10 columns");
			return false;
		}
	}

	bool isChecksumValid(){
		// Verify that the checksum matches the message
		std::vector<std::string> checksumParsedSentence;
		boost::split(checksumParsedSentence, m_fullSentence, boost::is_any_of("*"));
		if(checksumParsedSentence.size()>1 && m_parsedSentence[6].size() == 4)
		{
			//take the message, excluding the initial '!', to the end but not including the '*'
			string message = checksumParsedSentence[0].substr(1);
			int CS = 0x00;
			for (unsigned int i=0; i < message.length(); i++)
			{
				CS = CS ^ message[i];
			}
			
			std::string strCS;
			std::stringstream stringStream;
			stringStream.width(2);
			stringStream.fill('0');
			stringStream << std::hex << CS;
			stringStream >> strCS;
			boost::to_upper(strCS);

			if(m_parsedSentence[6].substr(2) == strCS)
			{
				//aisDebug("Checksum is correct");
				return true;
			}
			else
			{
				aisDebug("Checksum does not match!\nChecksum" + m_parsedSentence[6].substr(2) + " != " + strCS);
				return false;
			}
		}
		else
		{
			aisDebug("Checksum * not found in message");
			return false;
		}
	}

	/**	
	Call isMessageValid() before using this function
	*/
	int getTimestamp(){
		return boost::lexical_cast<int>(m_parsedSentence[8]);
	}

	/**	
	Call isMessageValid() before using this function
	*/
	std::string getStreamId(){
		return m_parsedSentence[7];
	}

	/**
	Call isMessageValid() before using this function
	*/
	int getNumberOfSentences(){
		return m_numberOfSentences;
	}

	/**
	Call isMessageValid() before using this function
	*/
	int getSentenceNumber(){
		return m_currentSentenceNumber;
	}

	/**
	Call isChecksumValid() before using this function
	*/
	std::string getData(){
		return m_parsedSentence[5];
	}

private:
};

#endif