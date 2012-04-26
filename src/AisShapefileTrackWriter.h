#ifndef AisShapefileTrackWriter_h
#define AisShapefileTrackWriter_h

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
#include <AisTrackWriter.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <shapelib/shapefil.h>

class AisShapefileTrackWriter : public AisTrackWriter{
public:
	AisShapefileTrackWriter(std::string filename):
		AisTrackWriter(filename)
	{
	}

	AisShapefileTrackWriter(std::string filename, unsigned int tracksPerFile):
		AisTrackWriter(filename, tracksPerFile)
	{	
	}

	void closeFile()
	{
		if(shpHandle)
		{
			SHPClose(shpHandle);
		}

		if(dbfHandle)
		{
			DBFClose( dbfHandle);
		}
	}

	void replaceBracketsAndAmpersands(std::string &input)
	{
		boost::algorithm::replace_all(input, "<", "&lt;");
		boost::algorithm::replace_all(input, ">", "&gt;");
		boost::algorithm::replace_all(input, "&", "&amp;");
	}

	//requires: trackIdx<aisTracks.size()
	void writeTrack(unsigned int trackIdx)
	{
		if(trackIdx >= m_trackSet.size()){
			aisDebug("AisTrackSet::writeTrack the trackIdx exceeds the maximum index of aisTrackSet");
			return;
		}

		AisMessage temp;
		if(m_trackSet[trackIdx].m_messages.size() == 0)
		{
			temp.setMMSI(m_trackSet[trackIdx].getMMSI());
		}
		else
		{
			//for ships with multiple static messages that are different from eachother,
			//we will add the string data of all of the different static messages.
			//TODO: in the future we should probably add IMO and other numeric info
			 temp = m_trackSet[trackIdx].m_messages[0];
			for(int staticMessageIdx = 1; staticMessageIdx < m_trackSet[trackIdx].m_messages.size(); staticMessageIdx++)
			{
				temp.setVESSELNAME( temp.getVESSELNAME() + " - " +  m_trackSet[trackIdx].m_messages[staticMessageIdx].getVESSELNAME());
				temp.setCALLSIGN( temp.getCALLSIGN() + " - " +  m_trackSet[trackIdx].m_messages[staticMessageIdx].getCALLSIGN());
				temp.setDESTINATION( temp.getDESTINATION() + " - " +  m_trackSet[trackIdx].m_messages[staticMessageIdx].getDESTINATION());
				temp.setSTREAMID( temp.getSTREAMID() + " - " +  m_trackSet[trackIdx].m_messages[staticMessageIdx].getSTREAMID());
			}
		}
		
		//write out static message info to dbf 
		DBFWriteIntegerAttribute(dbfHandle, currentShape, 0, temp.getMESSAGETYPE());
		DBFWriteIntegerAttribute(dbfHandle, currentShape, 1, temp.getMMSI());
		DBFWriteIntegerAttribute(dbfHandle, currentShape, 2, temp.getNAVSTATUS());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 3, temp.getROT());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 4, temp.getSOG());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 5, temp.getLON());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 6, temp.getLAT());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 7, temp.getCOG());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 8, temp.getTRUE_HEADING());
		DBFWriteIntegerAttribute(dbfHandle, currentShape, 9, temp.getDATETIME());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 10, temp.getIMO());
		DBFWriteStringAttribute(dbfHandle, currentShape, 11, temp.getVESSELNAME().c_str());
		DBFWriteIntegerAttribute(dbfHandle, currentShape, 12, temp.getVESSELTYPEINT());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 13, temp.getSHIPLENGTH());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 14, temp.getSHIPWIDTH());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 15, temp.getBOW());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 16, temp.getSTERN());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 17, temp.getPORT());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 18, temp.getSTARBOARD());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 19, temp.getDRAUGHT());
		DBFWriteStringAttribute(dbfHandle, currentShape, 20, temp.getDESTINATION().c_str());
		DBFWriteStringAttribute(dbfHandle, currentShape, 21, temp.getCALLSIGN().c_str());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 22, temp.getPOSACCURACY());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 23, temp.getETA());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 24, temp.getPOSFIXTYPE());
		DBFWriteStringAttribute(dbfHandle, currentShape++, 25, temp.getSTREAMID().c_str());


		m_trackSet[trackIdx].sortByDateTime();
		vector<double> longitude;
		vector<double> latitude;
		for(int coordIdx = 0; coordIdx < m_trackSet[trackIdx].size(); coordIdx++)
		{
			longitude.push_back(m_trackSet[trackIdx][coordIdx].m_lon);
			latitude.push_back(m_trackSet[trackIdx][coordIdx].m_lat);
		}
		
		SHPObject* shpObject = SHPCreateSimpleObject( SHPT_ARC, m_trackSet[trackIdx].size(), longitude.data(), latitude.data(), NULL);
		SHPWriteObject(shpHandle, -1 , shpObject);
		SHPDestroyObject(shpObject);
	}

	void writeToFile(){
		unsigned int currentTrack = 0;
		while(currentTrack < m_trackSet.size())
		{
			std::string currentFilename = m_filename + ".p" + boost::lexical_cast<string>(m_currentPartition++) + ".shp";
			aisDebug("Writing to file: " << currentFilename );
			initializeShapefiles(currentFilename);

			if(shpFileReady())
			{
			}
			else
			{
				aisDebug("shapefile file writer not ready");
				return;
			}

			for(unsigned int trackCount = 0; ((m_tracksPerFile == 0) || (trackCount < m_tracksPerFile)) && (currentTrack < m_trackSet.size()); trackCount++){
				writeTrack(currentTrack++);
			}
			closeFile();
		}
	}

	void initializeShapefiles(std::string filename)
	{
		currentShape = 0;
		initializeShapefileHandles(filename);
		if(dbfHandle == NULL)
		{
			aisDebug("Cannot open DBF file");
			return;
		}

		DBFAddField(dbfHandle, "MESSAGETYPE", FTInteger, 255, 0); 
		DBFAddField(dbfHandle, "MMSI", FTInteger, 255, 0);
		DBFAddField(dbfHandle, "NAVSTATUS", FTInteger, 255, 0);
		DBFAddField(dbfHandle, "ROT", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "SOG", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "LON", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "LAT", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "COG", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "TRUE_HEADING", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "DATETIME", FTInteger, 255, 0);
		DBFAddField(dbfHandle, "IMO", FTDouble, 15, 0);
		DBFAddField(dbfHandle, "VESSELNAME", FTString, 255, 0);
		DBFAddField(dbfHandle, "VESSELTYPEINT", FTInteger, 255, 0);
		DBFAddField(dbfHandle, "SHIPLENGTH", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "SHIPWIDTH", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "BOW", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "STERN", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "PORT", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "STARBOARD", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "DRAUGHT", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "DESTINATION", FTString, 255, 0);
		DBFAddField(dbfHandle, "CALLSIGN", FTString, 255, 0);
		DBFAddField(dbfHandle, "POSACCURACY", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "ETA", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "POSFIXTYPE", FTDouble, 15, 10);
		DBFAddField(dbfHandle, "STREAMID", FTString, 255, 0);
	}

	void initializeShapefileHandles(std::string filename)
	{
		shpHandle = SHPCreate(filename.c_str(), SHPT_ARC);
		dbfHandle = DBFCreate(filename.c_str());
	}

	bool shpFileReady(){
		return (shpHandle!=NULL) && (dbfHandle!=NULL);
	}

private:
	unsigned int currentShape;
	SHPHandle shpHandle;
	DBFHandle dbfHandle;
};

#endif
