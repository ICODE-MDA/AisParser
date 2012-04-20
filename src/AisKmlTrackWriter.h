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
//#include <AisTrack.h>
//#include <AisCoordinate.h>

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


//class MessageCompare
//{
//	bool operator() (const AisMessage& lhs, const AisMessage& rhs) const
//	{
//		/*if( lhs.getMMSI() < rhs.getMMSI() )
//		{
//			return true;
//		}
//		else if(lhs.getMMSI() > rhs.getMMSI())
//		{
//			return false;
//		}
//		else*/ //MMSIs are equal
//		//{
//			return lhs.getDATETIME() < rhs.getDATETIME();
//		//}
//	}
//};

//class TrackCompare
//{
//	bool operator() (const AisTrack& lhs, const AisTrack& rhs) const
//	{
//		if( lhs.getMMSI() < rhs.getMMSI() )
//		{
//			return true;
//		}
//		else if(lhs.getMMSI() > rhs.getMMSI())
//		{
//			return false;
//		}
//		else //MMSIs are equal
//		{
//			return lhs.getDATETIME() < rhs.getDATETIME();
//		}
//	}
//};


class AisCoordinate
{
public:
	AisCoordinate()
	{
		m_lat = 0;
		m_lon = 0;
		m_timestamp = 0;
	}
	
	AisCoordinate(double lat, double lon, int timestamp ){
		m_lat = lat;
		m_lon = lon;
		m_timestamp = timestamp;
	}

	double m_lat;
	double m_lon;
	int m_timestamp;
};


class AisTrack{
public:
	AisTrack(const AisMessage &m)
	{
		//hasStaticInformation = false;
		m_mmsi = m.getMMSI();
		addData(m);
	}
	
	void addStaticMessageInfo(const AisMessage &m)
	{
		for(int i=0; i<m_messages.size(); i++)
		{
			if( m == m_messages[i]){
				return;
			}
		}

		//if it gets here it was not found, so add it
		m_messages.push_back(m);
	}

	void addData(const AisMessage &message) 
	{
		if(message.getMESSAGETYPE() == 5)
		{
			addStaticMessageInfo(message);
		}
		else
		{
			m_aisCoords.push_back(AisCoordinate(message.getLAT(), message.getLON(), message.getDATETIME()));
		}
		
		//if(message.getMESSAGETYPE() == 5)
		//{
		//	if(!hasStaticInformation)
		//	{
		//		m_message = message;
		//		hasStaticInformation = true;
		//	}
		//}else{
		//	m_aisCoords.push_back(AisCoordinate(message.getLAT(), message.getLON(), message.getDATETIME()));
		//}
	}
	
	static bool dateCompare(const AisCoordinate& lhs, const AisCoordinate& rhs)
	{
		return lhs.m_timestamp < rhs.m_timestamp;
	}

	void sortByDateTime()
	{
		std::sort(m_aisCoords.begin(), m_aisCoords.end(), dateCompare);
	}

	double getMMSI() const{
		return m_mmsi;
	}

	//double getMMSI() const{
	//	return m_message.getMMSI();
	//}

	//double getIMO() const{
	//	return m_message.getIMO();
	//}

	//double getDATETIME() const{
	//	return m_message.getDATETIME();
	//}

	//std::string getCALLSIGN() const{
	//	return m_message.getCALLSIGN();
	//}

	//std::string getDESTINATION() const{
	//	return m_message.getDESTINATION();
	//}

	//std::string getVESSELNAME() const{
	//	return m_message.getVESSELNAME();
	//}

	//int getVESSELTYPEINT() const{
	//	return m_message.getVESSELTYPEINT();
	//}

	//int getSHIPLENGTH() const{
	//	return m_message.getSHIPLENGTH();
	//}

	//int getSHIPWIDTH() const{
	//	return m_message.getSHIPWIDTH();
	//}

	//int getBOW() const{
	//	return m_message.getBOW();
	//}

	//int getSTERN() const{
	//	return m_message.getSTERN();
	//}

	//int getPORT() const{
	//	return m_message.getPORT();
	//}

	//double getSTARBOARD() const{
	//	return m_message.getSTARBOARD();
	//}

	//double getDRAUGHT() const{
	//	return m_message.getDRAUGHT();
	//}

	//double getETA() const{
	//	return m_message.getETA();
	//}	
	//
	//double getPOSFIXTYPE() const{
	//	return m_message.getPOSFIXTYPE();
	//}

	//std::string getSTREAMID() const{
	//	return m_message.getSTREAMID();
	//}

	//int getMESSAGETYPE() const{
	//	return m_message.getMESSAGETYPE();
	//}

	double getLAT() const
	{
		if(m_aisCoords.size() > 0)
		{
			return m_aisCoords[m_aisCoords.size()-1].m_lat;
		}
		else
		{
			return -999;
		}
	}

	double getLON() const
	{
		if(m_aisCoords.size() > 0)
		{
			return m_aisCoords[m_aisCoords.size()-1].m_lon;
		}
		else
		{
			return -999;
		}
	}

	AisCoordinate& operator[] (unsigned int idx)
	{
		return m_aisCoords[idx];
	}

	unsigned int size()
	{
		return m_aisCoords.size();
	}

	
	//void writeToFile(std::ofstream &of){
	//	of << "<Placemark>" << endl;
	//	of << "	<name>Absolute Extruded</name>" << endl;
	//	of << "	<description>" << m_message.getCALLSIGN() << m_message.getDESTINATION() << m_message.getVESSELNAME() << "</description>" << endl;
	//	of << "	<styleUrl>#yellowLineGreenPoly</styleUrl>" << endl;
	//	of << "	<LineString>" << endl;
	//	of << "		<extrude>0</extrude>" << endl;
	//	of << "		<tessellate>1</tessellate>" << endl;
	//	of << "		<altitudeMode>clampToGround</altitudeMode>" << endl;
	//	of << "		<coordinates>" << endl;
	//	for(int i = 0; i < m_aisCoords.size(); i++)
	//	{
	//		of << setprecision(10) << "			" << m_aisCoords[i].m_lon << "," << m_aisCoords[i].m_lat << ",0" << endl;
	//	}
	//	of << "		</coordinates>" << endl;
	//	of << "	</LineString>" << endl;
	//	of << "</Placemark>" << endl;
	//}

	std::vector<AisMessage> m_messages;
private:
	double m_mmsi;
	//bool hasStaticInformation;
	//AisMessage m_message;
	//std::vector<AisMessage> m_messages;
	std::vector<AisCoordinate> m_aisCoords;
};

class AisTrackSet{
public:
	AisTrackSet()
	{}

	~AisTrackSet(){}

	void add(const AisMessage& m)
	{
		//check if any tracks mmmsi == m.getMMSI();
		if(!alreadyHaveShip(m))
		{
			aisTracks.push_back(AisTrack(m));
		}
	}


	bool messageBelongsToCurrentTrack(const AisMessage& m, std::vector<AisTrack>::iterator t)
	{
		if(m.getMMSI() == t->getMMSI())
		{
			//check if the points have a lat/lon and if they're within 1 (magic number) manhattan degree of eachother
			if(t->getLAT() != -999 && m.getLAT() != -999 )//&& t->getLON() != -999 && m.getLON() != -999
			{
				if( ((t->getLAT() - m.getLAT())*(t->getLAT() - m.getLAT()) + (t->getLON() - m.getLON())* (t->getLON() - m.getLON())) > 1)
				{
					return false;
				}
			}

			return true;
		}
		
		return false;
	}

	bool alreadyHaveShip(const AisMessage& message)
	{

		std::vector<AisTrack>::iterator itr = aisTracks.begin();

		while(itr != aisTracks.end()){
			if(messageBelongsToCurrentTrack(message, itr))
			{
				itr->addData(message);
				return true;
			}
			itr++;
		}
		return false;
	}

	void removeBrackets(std::string &input)
	{
		boost::algorithm::erase_all(input, "<");
		boost::algorithm::erase_all(input, ">");
	}

	void writeData(std::ofstream &of)
	{
		for(int trackIdx = 0; trackIdx<aisTracks.size(); trackIdx++)
		{
			string vesselNames;
			for(int staticMessageIdx = 0; staticMessageIdx < aisTracks[trackIdx].m_messages.size(); staticMessageIdx++)
			{
				vesselNames += aisTracks[trackIdx].m_messages[staticMessageIdx].getVESSELNAME() + " - ";
			}
			//string vesselName = aisTracks[trackIdx].getVESSELNAME();
			removeBrackets(vesselNames);
			of << "<Placemark>" << endl;
			of << "	<name>" << vesselNames << "</name>" << endl;
			//of << "	<description>" << aisTracks[trackIdx].getCALLSIGN() << aisTracks[trackIdx].getDESTINATION() << aisTracks[trackIdx].getVESSELNAME() << "</description>" << endl;
			of << "		<description>" << endl;
			of << "		<![CDATA[" << endl;
			of << "			MMSI:" << aisTracks[trackIdx].getMMSI() << "<br>" << endl;
			for(int staticMessageIdx = 0; staticMessageIdx < aisTracks[trackIdx].m_messages.size(); staticMessageIdx++)
			{
				//of << "			Message Type:" << aisTracks[trackIdx].getMESSAGETYPE() << "<br>" << endl;
				//of << "			Navigation Status:" << aisTracks[trackIdx].getNAVSTATUS() << "<br>" << endl;
				//of << "			ROT:" << aisTracks[trackIdx].getROT() << "<br>" << endl;
				//of << "			SOG:" << aisTracks[trackIdx].getSOG() << "<br>" << endl;
				//of << "			LON:" << aisTracks[trackIdx].getLON() << "<br>" << endl;
				//of << "			LAT:" << aisTracks[trackIdx].getLAT() << "<br>" << endl;
				//of << "			COG:" << aisTracks[trackIdx].getCOG() << "<br>" << endl;
				//of << "			True Heading:" << message.getTRUE_HEADING() << "<br>" << endl;
				//of << "			Date Time:" << message.getDATETIME() << "<br>" << endl;
				of << "			IMO:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getIMO() << "<br>" << endl;
				of << "			Vessel Name:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getVESSELNAME() << "<br>" << endl;
				of << "			Vessel Type Int:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getVESSELTYPEINT() << "<br>" << endl;
				of << "			Ship Length:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getSHIPLENGTH() << "<br>" << endl;
				of << "			Ship Width:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getSHIPWIDTH() << "<br>" << endl;
				of << "			BOW:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getBOW() << "<br>" << endl;
				of << "			STERN:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getSTERN() << "<br>" << endl;
				of << "			PORT:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getPORT() << "<br>" << endl;
				of << "			STARBOARD:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getSTARBOARD() << "<br>" << endl;
				of << "			DRAUGHT:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getDRAUGHT() << "<br>" << endl;
				of << "			Destination:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getDESTINATION() << "<br>" << endl;
				of << "			Callsign:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getCALLSIGN() << "<br>" << endl;
				//of << "			Position Accuracy:" << message.getPOSACCURACY() << "<br>" << endl;
				of << "			ETA:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getETA() << "<br>" << endl;
				of << "			Position Fix Type:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getPOSFIXTYPE() << "<br>" << endl;
				of << "			StreamID:" <<  aisTracks[trackIdx].m_messages[staticMessageIdx].getSTREAMID() << "<br>" << endl;
				of << "			------" << endl;
			}
			of << "		]]>" << endl;
			of << "		</description>" << endl;
			of << "	<styleUrl>#yellowLineGreenPoly</styleUrl>" << endl;
			of << "	<LineString>" << endl;
			of << "		<extrude>0</extrude>" << endl;
			of << "		<tessellate>1</tessellate>" << endl;
			of << "		<altitudeMode>clampToGround</altitudeMode>" << endl;
			of << "		<coordinates>" << endl;
			aisTracks[trackIdx].sortByDateTime();
			for(int coordIdx = 0; coordIdx < aisTracks[trackIdx].size(); coordIdx++)
			{
				of << setprecision(10) << "			" << aisTracks[trackIdx][coordIdx].m_lon << "," << aisTracks[trackIdx][coordIdx].m_lat << ",0" << endl;
			}
			of << "		</coordinates>" << endl;
			of << "	</LineString>" << endl;
			of << "</Placemark>" << endl;
			for(int coordIdx = 0; coordIdx < aisTracks[trackIdx].size(); coordIdx++)
			{
				of << "<Placemark>" << endl;
				of << "	<description>" << endl;
				of << setprecision(10) << "Timestamp:" << aisTracks[trackIdx][coordIdx].m_timestamp << endl;
				of << "	</description>" << endl;
				of << " <Point>" << endl;
				of << "		<coordinates>" << endl;
				of << setprecision(10) << "			" << aisTracks[trackIdx][coordIdx].m_lon << "," << aisTracks[trackIdx][coordIdx].m_lat << ",0" << endl;
				of << "		</coordinates>" << endl;
				of << "	</Point>" << endl;
				of << "</Placemark>" << endl;
			}
		}
	}


private:
	std::vector<AisTrack> aisTracks;
};

class AisKmlTrackWriter : public AisWriter{
public:
	AisKmlTrackWriter(std::string filename){
		of.open(filename, std::ios::out);
	}

	~AisKmlTrackWriter(){
		if(of.is_open()){
			of << "</Document>" << endl;
			of << "</kml>" << endl;
			of.close();
		}
	}
	
	bool isReady(){
		return of.is_open();
	}

	bool writeEntry(const AisMessage& message)
	{
		m_trackSet.add(message);
		return true;
	}

	void writeToFile(){
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
		m_trackSet.writeData(of);
	}

private:
	std::ofstream of;
	AisTrackSet m_trackSet;
};

#endif
