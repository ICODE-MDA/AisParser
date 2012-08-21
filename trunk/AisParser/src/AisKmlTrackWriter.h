#ifndef AisKmlTrackWriter_h
#define AisKmlTrackWriter_h

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

//<?xml version="1.0" encoding="UTF-8"?>
//<kml xmlns="http://www.opengis.net/kml/2.2">
//  <Document>
//    <name>Paths</name>
//    <description>Examples of paths. Note that the tessellate tag is by default
//      set to 0. If you want to create tessellated lines, they must be authored
//      (or edited) directly in KML.
//        <![CDATA[
//          <h1>CDATA Tags are useful!</h1>
//          <p><font color="red">Text is <i>more readable</i> and 
//          <b>easier to write</b> when you can avoid using entity 
//          references.</font></p>
//        ]]></description>
//    <Style id="yellowLineGreenPoly">
//      <LineStyle>
//        <color>7f00ffff</color>
//        <width>4</width>
//      </LineStyle>
//      <PolyStyle>
//        <color>7f00ff00</color>
//      </PolyStyle>
//    </Style>
//    <Placemark>
//      <name>Absolute Extruded</name>
//      <description>Transparent green wall with yellow outlines</description>
//      <styleUrl>#yellowLineGreenPoly</styleUrl>
//      <LineString>
//        <extrude>1</extrude>
//        <tessellate>1</tessellate>
//        <altitudeMode>absolute</altitudeMode>
//        <coordinates> -112.2550785337791,36.07954952145647,2357
//          -112.2549277039738,36.08117083492122,2357
//          -112.2552505069063,36.08260761307279,2357
//          -112.2564540158376,36.08395660588506,2357
//          -112.2580238976449,36.08511401044813,2357
//          -112.2595218489022,36.08584355239394,2357
//          -112.2608216347552,36.08612634548589,2357
//          -112.262073428656,36.08626019085147,2357
//          -112.2633204928495,36.08621519860091,2357
//          -112.2644963846444,36.08627897945274,2357
//          -112.2656969554589,36.08649599090644,2357 
//        </coordinates>
//      </LineString>
//    </Placemark>
//  </Document>
//</kml>


class AisKmlTrackWriter : public AisTrackWriter{
public:
	AisKmlTrackWriter(std::string filename):
		AisTrackWriter(filename + ".kml"),
		of()
	{
		
	}

	AisKmlTrackWriter(std::string filename, unsigned int tracksPerFile):
		AisTrackWriter(filename, tracksPerFile),
		of()
	{	
	}

	void closeFile()
	{
		if(of.is_open()){
			of << "</Document>" << endl;
			of << "</kml>" << endl;
			of.close();
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
		if(!(m_trackSet[trackIdx].m_messages.size() > 1))
		{
			return;
		}

		if(trackIdx >= m_trackSet.size()){
			aisDebug("AisTrackSet::writeTrack the trackIdx exceeds the maximum index of aisTrackSet");
			return;
		}

		string vesselNames;
		for(int staticMessageIdx = 0; staticMessageIdx < m_trackSet[trackIdx].m_messages.size(); staticMessageIdx++)
		{
			vesselNames += m_trackSet[trackIdx].m_messages[staticMessageIdx].getVESSELNAME() + " - ";
		}

		replaceBracketsAndAmpersands(vesselNames);
		of << "<Placemark>" << endl;
		of << "	<name>" << vesselNames << "</name>" << endl;
		of << "		<description>" << endl;
		of << "		<![CDATA[" << endl;
		of << setprecision(10) << "			MMSI:" << m_trackSet[trackIdx].getMMSI() << "<br>" << endl;
		for(int staticMessageIdx = 0; staticMessageIdx < m_trackSet[trackIdx].m_messages.size(); staticMessageIdx++)
		{
			of << "			Message Type:" << m_trackSet[trackIdx].m_messages[staticMessageIdx].getMESSAGETYPE() << "<br>" << endl;
			of << "			Navigation Status:" << m_trackSet[trackIdx].m_messages[staticMessageIdx].getNAVSTATUS() << "<br>" << endl;
			//of << "			ROT:" << m_trackSet[trackIdx].m_messages[staticMessageIdx].getROT() << "<br>" << endl;
			//of << "			SOG:" << m_trackSet[trackIdx].m_messages[staticMessageIdx].getSOG() << "<br>" << endl;
			//of << "			LON:" << m_trackSet[trackIdx].m_messages[staticMessageIdx].getLON() << "<br>" << endl;
			//of << "			LAT:" << m_trackSet[trackIdx].m_messages[staticMessageIdx].getLAT() << "<br>" << endl;
			//of << "			COG:" << m_trackSet[trackIdx].m_messages[staticMessageIdx].getCOG() << "<br>" << endl;
			//of << "			True Heading:" << m_trackSet[trackIdx].m_messages[staticMessageIdx].getTRUE_HEADING() << "<br>" << endl;
			of << "			Date Time:" << m_trackSet[trackIdx].m_messages[staticMessageIdx].getDATETIME() << "<br>" << endl;
			of << "			IMO:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getIMO() << "<br>" << endl;
			of << "			Vessel Name:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getVESSELNAME() << "<br>" << endl;
			of << "			Vessel Type Int:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getVESSELTYPEINT() << "<br>" << endl;
			of << "			Ship Length:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getSHIPLENGTH() << "<br>" << endl;
			of << "			Ship Width:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getSHIPWIDTH() << "<br>" << endl;
			of << "			BOW:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getBOW() << "<br>" << endl;
			of << "			STERN:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getSTERN() << "<br>" << endl;
			of << "			PORT:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getPORT() << "<br>" << endl;
			of << "			STARBOARD:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getSTARBOARD() << "<br>" << endl;
			of << "			DRAUGHT:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getDRAUGHT() << "<br>" << endl;
			of << "			Destination:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getDESTINATION() << "<br>" << endl;
			of << "			Callsign:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getCALLSIGN() << "<br>" << endl;
			of << "			Position Accuracy:" << m_trackSet[trackIdx].m_messages[staticMessageIdx].getPOSACCURACY() << "<br>" << endl;
			of << "			ETA:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getETA() << "<br>" << endl;
			of << "			Position Fix Type:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getPOSFIXTYPE() << "<br>" << endl;
			of << "			StreamID:" <<  m_trackSet[trackIdx].m_messages[staticMessageIdx].getSTREAMID() << "<br>" << endl;
			of << "			-------------------<br>" << endl;
		}
		of << "		]]>" << endl;
		of << "		</description>" << endl;
		of << "	<styleUrl>#yellowLineGreenPoly</styleUrl>" << endl;
		of << "	<LineString>" << endl;
		of << "		<extrude>0</extrude>" << endl;
		of << "		<tessellate>1</tessellate>" << endl;
		of << "		<altitudeMode>clampToGround</altitudeMode>" << endl;
		of << "		<coordinates>" << endl;
		m_trackSet[trackIdx].sortByDateTime();
		for(int coordIdx = 0; coordIdx < m_trackSet[trackIdx].size(); coordIdx++)
		{
			of << setprecision(10) << "			" << m_trackSet[trackIdx][coordIdx].m_lon << "," << m_trackSet[trackIdx][coordIdx].m_lat << ",0" << endl;
		}
		of << "		</coordinates>" << endl;
		of << "	</LineString>" << endl;
		of << "</Placemark>" << endl;

		//write first with a pin
		if(m_trackSet[trackIdx].size() > 0)
		{
			of << "<Placemark>" << endl;
			of << "	<description>" << endl;
			of << "		MMSI:" << m_trackSet[trackIdx].getMMSI() << endl;
			of << setprecision(10) << "		Timestamp:" << m_trackSet[trackIdx][0].m_timestamp << endl;
			of << "	</description>" << endl;
			of << " <Point>" << endl;
			of << "		<coordinates>" << endl;
			of << setprecision(10) << "			" << m_trackSet[trackIdx][0].m_lon << "," << m_trackSet[trackIdx][0].m_lat << ",0" << endl;
			of << "		</coordinates>" << endl;
			of << "	</Point>" << endl;
			of << "</Placemark>" << endl;
		}

		//write last with a pin
		if(m_trackSet[trackIdx].size() > 1)
		{
			unsigned int lastIdx = m_trackSet[trackIdx].size()-1;
			of << "<Placemark>" << endl;
			of << "	<description>" << endl;
			of << "		MMSI:" << m_trackSet[trackIdx].getMMSI() << endl;
			of << setprecision(10) << "		Timestamp:" << m_trackSet[trackIdx][lastIdx].m_timestamp << endl;
			of << "	</description>" << endl;
			of << " <Point>" << endl;
			of << "		<coordinates>" << endl;
			of << setprecision(10) << "			" << m_trackSet[trackIdx][lastIdx ].m_lon << "," << m_trackSet[trackIdx][lastIdx ].m_lat << ",0" << endl;
			of << "		</coordinates>" << endl;
			of << "	</Point>" << endl;
			of << "</Placemark>" << endl;
		}

		//for(int coordIdx = 0; coordIdx < m_trackSet[trackIdx].size(); coordIdx++)
		//{
		//	of << "<Placemark>" << endl;
		//	of << "	<description>" << endl;
		//	of << "		MMSI:" << m_trackSet[trackIdx].getMMSI() << endl;
		//	of << setprecision(10) << "		Timestamp:" << m_trackSet[trackIdx][coordIdx].m_timestamp << endl;
		//	of << "	</description>" << endl;
		//	of << " <Point>" << endl;
		//	of << "		<coordinates>" << endl;
		//	of << setprecision(10) << "			" << m_trackSet[trackIdx][coordIdx].m_lon << "," << m_trackSet[trackIdx][coordIdx].m_lat << ",0" << endl;
		//	of << "		</coordinates>" << endl;
		//	of << "	</Point>" << endl;
		//	of << "</Placemark>" << endl;
		//}
	}

	void writeToFile(){
		unsigned int currentTrack = 0;
		while(currentTrack < m_trackSet.size())
		{
			std::string currentFilename = m_filename + ".p" + boost::lexical_cast<string>(m_currentPartition++) + ".kml";
			aisDebug("Writing to file: " << currentFilename );
			of.open(currentFilename, std::ios::out);
		
			if(of.is_open())
			{
				of << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
				of << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">" << endl;
				of << "<Document>" << endl;
				of << "<Style id=\"yellowLineGreenPoly\">" << endl;
				of << "	<LineStyle>" << endl;
				of << "		<color>7f00ffff</color>" << endl;
				of << "		<width>4</width>" << endl;
				of << "	</LineStyle>" << endl;
				of << "	<PolyStyle>" << endl;
				of << "		<color>7f00ff00</color>" << endl;
				of << "	</PolyStyle>" << endl;
				of << "</Style>" << endl;
			}
			else
			{
				aisDebug("kml file writer not ready");
			}

			for(unsigned int trackCount = 0; ((m_tracksPerFile == 0) || (trackCount < m_tracksPerFile)) && (currentTrack < m_trackSet.size()); trackCount++){
				writeTrack(currentTrack++);
			}
			closeFile();
		}
	}

private:
	std::ofstream of;
};

#endif
