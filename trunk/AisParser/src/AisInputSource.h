#ifndef AisInputSource_h
#define AisInputSource_h

#include <string>
#include <AisDebug.h>

/**
This class handles grabbing external AIS data
*/
class AisInputSource{
public:

	/**
	Returns true if the input source is ready
	to start providing data, false otherwhise
	*/
	virtual bool isReady() = 0;

	/**
	Returns the next sentence from the AIS
	input source as a string.

	For sources that are not always necesarilly ready like
	a TCP connection, this should wait for a certain amount
	of time for the input to become ready
	*/
	virtual std::string getNextSentence()=0;

	
	//virtual bool endOfStream() = 0;
	
	
	/**
	Returns true if the input source can read
	another line of AIS
	*/
	//virtual bool canReadLine() = 0;

};

#endif
