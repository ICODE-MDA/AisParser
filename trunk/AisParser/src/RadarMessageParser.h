#ifndef RadarMessageParser_h
#define RadarMessageParser_h

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <RadarMessage.h>
#include <AisDebug.h>

using namespace std;



/**
Class that parses PVOL Radar
sentences into an radarMessage object
*/
class RadarMessageParser{
public:
	/*RadarMessageParser(){
		m_data.clear();
	}*/

	/*void addData(std::vector<std::string> data){
		m_data.append(data);
	}*/

	RadarMessage parseRadar(std::vector<std::string> radarVector){

		// Convert AIS Message from 6-bit Hex to Binary
		//vector<bool> aisBin;
		RadarMessage radarMessage;

		//AISMessageToBin(m_data, aisBin);
		DecodeRadarMessage(radarVector, radarMessage);

		return radarMessage;
	}



private:


/**
 * Radar tracks and Satellite Imagery ship positions are pushed to MSSIS as PVOL entries
 * Togo Format:  $PVOL,PDM,s,x.x,x.x,x.x,x.x,x.x,s[,s,s,s]*CS,x<CR><LF>
 *                                                       | Unix time
 *                     |  |   |   |   |   |  |  | | | | Check Sum
 *                     |  |   |   |   |   |  |  | | | Optional Field 3
 *                     |  |   |   |   |   |  |  | | Optional Field 2
 *                     |  |   |   |   |   |  |  | Optional Field 1
 *                     |  |   |   |   |   |  |  Source [ex: radar]
 *                     |  |   |   |   |   |  SOG (knots)
 *                     |  |   |   |   |   COG (True) 
 *                     |  |   |   |   Longitude (decimal degrees) 
 *                     |  |   |   Latitude (decimal degrees)  
 *                     |  |   TOD (seconds)  
 *                     |  Track Identifier (szComID) [ex: MMSI-TID] 
 * Track Identifier:  for the ARPA contact derived from a ship board radar.
 * TOD:  UTC time since midnight (in seconds)
 * optional field 1:  Track Status
 * optional field 2:  
 * optional field 3:  Message Type (ex. TTM)
 * Check Sum
 * Unix Time
 * Example: $PVOL,PDM,TGO-01,86391,6.055817,1.295286,85.9,0.1,shore-radar,T,M,,TTM*59,r166710001,1373414395,1373414403
 * Nigeria Format:  $PVOL,PDM,s,x.x,x.x,x.x,x.x,x.x,s[,s,s,s,s,]*CS,s,x.x,x.x<CR><LF>
 *                            |  |   |   |   |   |  |  | | | |   |  |  | Unix Time of Track
 *                            |  |   |   |   |   |  |  | | | |   |  | Comment
 *                            |  |   |   |   |   |  |  | | | |   |  Check Sum                                                          | Check Sum
 *                            |  |   |   |   |   |  |  | | | | Optional Field 4
 *                            |  |   |   |   |   |  |  | | | Optional Field 3
 *                            |  |   |   |   |   |  |  | | Optional Field 2
 *                            |  |   |   |   |   |  |  | Optional Field 1
 *                            |  |   |   |   |   |  |  Source [ex: radar]
 *                            |  |   |   |   |   |  SOG (knots)
 *                            |  |   |   |   |   COG (True) 
 *                            |  |   |   |   Longitude (decimal degrees) 
 *                            |  |   |   Latitude (decimal degrees)  
 *                            |  |   TOD (seconds)  
 *                            |  Track Identifier (szComID) [ex: MMSI-TID] 
 * optional Field 1: Unix Time of Track
 * optional Field 2: ID of Source
 * optional Field 3: Ship Name
 * optional Filed 4: MMSI if known
 * Exampe: $PVOL,PDM,ST-4095,16861,4.68269157409668,7.15723180770874,360.0,0.0,ST-RADAR,1362026461,RMAC,TEERA BHUM,1193046*7A,rST-RADAR-TEST,1362026461,1362026527
 * Togo and Nigeria has the simliar formats for the first 9 objects.
 */
	
	int DecodeRadarMessage(std::vector<std::string> m_data, RadarMessage &RADARPosit)
	{
		int MessageType = 2;
		if (boost::find_first(m_data[10],"RMAC")){
			MessageType = 1;
		} else
		{
			MessageType = 2;
		}
		
		bool debug = false;
		RADARPosit.setMESSAGETYPE(MessageType);
		RADARPosit.setTRACKNUM(boost::lexical_cast<int>(m_data[2].substr(4)));
		RADARPosit.setLAT(boost::lexical_cast<double>(m_data[4]));
		RADARPosit.setLON(boost::lexical_cast<double>(m_data[5]));
		
		RADARPosit.setCOG(boost::lexical_cast<double>(m_data[6]));
		RADARPosit.setSOG(boost::lexical_cast<double>(m_data[7]));
		RADARPosit.setSOURCE(m_data[8]);
		switch (MessageType)
		{
		case 1:		// // Radar Report with RMAC format
			{
			RADARPosit.setDATETIME(boost::lexical_cast<int>(m_data[9]));
			RADARPosit.setSTREAMID(m_data[13]);
			RADARPosit.setVESSELNAME(m_data[11]);
			//boost::find_first indEnd = boost::find_first(m_fullSentence,"*");
			//RADARPosit.setMMSI(m_data[12]);
			}
		case 2:		// Radar Report with Togo format
			{
			RADARPosit.setTARGETSTATUS(m_data[9]);
			RADARPosit.setTARGETACQ(m_data[10]);
			RADARPosit.setSTREAMID(m_data[13]);
			RADARPosit.setDATETIME(boost::lexical_cast<int>(m_data[14].substr(0,10)));
			
			break;
			}
		default:
			{
			aisDebug( "The message type was not recognized and therefore could not be decoded. MessageType=" << MessageType );
			}
		}


		if (RADARPosit.getLON()!=-999)
		{
			wrapLonTo180(RADARPosit);
		}

		return 0;
	}

	void wrapLonTo180(RadarMessage& m)
	{
		if ( m.getLON() > 180.0 )
		{
			do
			{
				m.setLON(m.getLON() - 360.0);
			} while ( m.getLON() > 180.0 );
		}
		else if ( m.getLON() < -180.0  )
		{
			do
			{
				m.setLON(m.getLON() + 360.0);
			} while ( m.getLON() < -180.0 );
		}
		return;
	}
	

	//std::vector<std::string> m_data;
};

#endif