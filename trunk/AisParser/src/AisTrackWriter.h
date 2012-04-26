#ifndef AisTrackWriter_h
#define AisTrackWriter_h

#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>

#include <AisWriter.h>
#include <AisMessage.h>
#include <AisTrack.h>
#include <AisCoordinate.h>
#include <AisTrackSet.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>


class AisTrackWriter : public AisWriter{
public:
	AisTrackWriter(std::string filename){
		m_tracksPerFile = 0;
		m_currentPartition = 0;
		m_filename = filename;
	}

	AisTrackWriter(std::string filename, unsigned int tracksPerFile){
		m_filename = filename;
		m_tracksPerFile = tracksPerFile;
		m_currentPartition = 0;
	}

	~AisTrackWriter(){
		closeFile();
	}
	
	virtual void closeFile(){}// = 0;

	bool isReady(){
		return (m_filename != "") && (m_tracksPerFile >= 0);
	}

	bool writeEntry(const AisMessage& message)
	{
		m_trackSet.add(message);
		return true;
	}

	//requires: trackIdx<aisTracks.size()
	virtual void writeTrack(unsigned int trackIdx) = 0;
	
	virtual void writeToFile() = 0;

protected:
	std::string m_filename;
	unsigned int m_tracksPerFile;
	unsigned int m_currentPartition;
	AisTrackSet m_trackSet;
};

//void AisTrackWriter::closeFile(){}

#endif
