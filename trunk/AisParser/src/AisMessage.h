#ifndef AISMESSAGE_H
#define AISMESSAGE_H
#include <string>
#include <sstream>
#include <iomanip>

#include <boost/algorithm/string.hpp>

using namespace std;

class AisMessage {

public:

	AisMessage()
	{
		/*
		//type 1,2,3
		type
		repeat
		mmsi
		status
		turn
		speed
		accuracy
		lon
		lat
		course
		heading
		second
		maneuver
		raim
		radio

		//type 4 additional
		year
		month
		day
		hour
		minute
		second
		epfd

		//type 5 additional
		ais_version
		imo
		callsign
		shipname
		shiptype
		to_bow
		to_stern
		to_port
		to_starboard
		draught
		destination
		dte

		//type 6 additional
		seqno
		dest_mmsi
		retransmit
		dac
		fid
		data

		//type 6 dangerous cargo additional
		lastport
		lmonth
		lday
		lhour
		lminute
		nextport
		nmonth
		nday
		nhour
		nminute
		dangerous
		imdcat
		unid
		amount
		unit

		//type 6 tidal window additional
		tidals
		from_hour
		from_min
		to_hour
		to_min
		cdir
		cspeed

		//type 6 number of persons onboard additional
		persons



		*/

		MESSAGETYPE = -1;
		MMSI = -1;
		NAVSTATUS = -1;
		ROT = -1 ;
		SOG = -1;
		LAT = -999;
		LON = -999;
		COG = -1;
		TRUE_HEADING = -999;
		DATETIME = -1;
		IMO = -1;
		VESSELNAME = "";
		VESSELTYPEINT = -1;
		SHIPLENGTH = -1;
		SHIPWIDTH = -1;
		BOW = -1;
		STERN = -1;
		PORT = -1;
		STARBOARD = -1;
		DRAUGHT = -1;
		DESTINATION = "";
		CALLSIGN = "";
		POSACCURACY = -1;
		ETA = -1;
		PARTNUMBER = -1;
		POSFIXTYPE = -1;
		STREAMID = "";






		//Additional things from original AISDot
		time=-1;
		Year=-1;
		Month=-1;
		Day=-1;
		Hour=-1;
		Minute=-1;
		Second=-1;
		UTCTime=-1;
		VesselType="";
		Status="";
		AISChannel="";
		ImageIndex=-1;
		AssociatedImageTime="";
		AssociatedImageSensor="";
		AssociatedImageResolution=-1;
	}
	
	bool operator != (const AisMessage& lhs) const{
		return !(operator==(lhs));
	}

	bool operator == (const AisMessage& lhs) const{

		if(
			equalWithExceptionOfTime(lhs) &&
			this->DATETIME == lhs.getDATETIME() 
			)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool equalWithExceptionOfTime (const AisMessage& lhs) const{

		if(
			this->BOW == lhs.getBOW() &&
			this->CALLSIGN == lhs.getCALLSIGN() &&
			this->COG == lhs.getCOG() &&
			this->DESTINATION == lhs.getDESTINATION() &&
			this->DRAUGHT == lhs.getDRAUGHT() &&
			this->ETA == lhs.getETA() &&
			this->PARTNUMBER == lhs.getPARTNUMBER() &&
			this->IMO == lhs.getIMO() &&
			this->LAT == lhs.getLAT() &&
			this->LON == lhs.getLON() &&
			this->MESSAGETYPE == lhs.getMESSAGETYPE() &&
			this->MMSI == lhs.getMMSI() &&
			this->NAVSTATUS == lhs.getNAVSTATUS() &&
			this->PORT == lhs.getPORT() &&
			this->POSACCURACY == lhs.getPOSACCURACY() &&
			this->POSFIXTYPE == lhs.getPOSFIXTYPE() &&
			this->ROT == lhs.getROT() &&
			this->SHIPLENGTH == lhs.getSHIPLENGTH() &&
			this->SHIPWIDTH == lhs.getSHIPWIDTH() &&
			this->SOG == lhs.getSOG() &&
			this->STARBOARD == lhs.getSTARBOARD() &&
			this->STERN == lhs.getSTERN() &&
			this->STREAMID == lhs.getSTREAMID() &&
			this->TRUE_HEADING == lhs.getTRUE_HEADING() &&
			this->VESSELNAME == lhs.getVESSELNAME() &&
			this->VESSELTYPEINT == lhs.getVESSELTYPEINT()
			)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	double getMMSI() const
	{
		return MMSI;
	}
	void setMMSI(double mmsi)
	{
		this->MMSI = mmsi;
	}
	int getNAVSTATUS() const
	{
		return NAVSTATUS;
	}
	void setNAVSTATUS(int navstatus)
	{
		this->NAVSTATUS = navstatus;
	}
	double getROT() const
	{
		return ROT;
	}
	void setROT(double rot)
	{
		this->ROT = rot;
	}
	double getSOG() const
	{
		return SOG;
	}
	void setSOG(double sog)
	{
		this->SOG = sog;
	}
	double getLAT() const
	{
		return LAT;
	}
	void setLAT(double lat)
	{
		this->LAT = lat;
	}
	double getLON() const
	{
		return LON;
	}
	void setLON(double lon)
	{
		this->LON = lon;
	}
	double getCOG() const
	{
		return COG;
	}
	void setCOG(double cog)
	{
		this->COG = cog;
	}
	double getTRUE_HEADING() const
	{
		return TRUE_HEADING;
	}
	void setTRUE_HEADING(double trueheading)
	{
		this->TRUE_HEADING = trueheading;
	}
	int getDATETIME() const
	{
		return DATETIME;
	}
	void setDATETIME(int datetime)
	{
		this->DATETIME = datetime;
	}

	int getMESSAGETYPE() const
	{
		return MESSAGETYPE;
	}
	void setMESSAGETYPE(int messagetype)
	{
		this->MESSAGETYPE = messagetype;
	}

	double getIMO() const
	{
		return IMO;
	}
	void setIMO(double imo)
	{
		this->IMO = imo;
	}
	string getVESSELNAME() const
	{
		return VESSELNAME;
	}
	void setVESSELNAME(string vesselname)
	{
		this->VESSELNAME = vesselname;
	}
	int getVESSELTYPEINT() const
	{
		return VESSELTYPEINT;
	}
	void setVESSELTYPEINT(int vesseltypeint)
	{
		this->VESSELTYPEINT = vesseltypeint;
	}

	void setSHIPLENGTH(double shiplength)
	{
		this->SHIPLENGTH = shiplength;
	}
	double getSHIPWIDTH() const
	{
		return SHIPWIDTH;
	}
	void setSHIPWIDTH(double shipwidth)
	{
		this->SHIPWIDTH = shipwidth;
	}
	double getBOW() const
	{
		return BOW;
	}

	/**
	* @return the SHIPLENGTH
	*/
	double getSHIPLENGTH()  const{
		return SHIPLENGTH;
	}

	/**
	* @param BOW the BOW to set
	*/
	void setBOW(double BOW) {
		this->BOW = BOW;
	}

	/**
	* @return the STERN
	*/
	double getSTERN() const{
		return STERN;
	}

	/**
	* @param STERN the STERN to set
	*/
	void setSTERN(double STERN) {
		this->STERN = STERN;
	}

	/**
	* @return the PORT
	*/
	double getPORT() const{
		return PORT;
	}

	/**
	* @param PORT the PORT to set
	*/
	void setPORT(double PORT) {
		this->PORT = PORT;
	}

	/**
	* @return the STARBOARD
	*/
	double getSTARBOARD() const{
		return STARBOARD;
	}

	/**
	* @param STARBOARD the STARBOARD to set
	*/
	void setSTARBOARD(double STARBOARD) {
		this->STARBOARD = STARBOARD;
	}

	/**
	* @return the DRAUGHT
	*/
	double getDRAUGHT() const{
		return DRAUGHT;
	}

	/**
	* @param DRAUGHT the DRAUGHT to set
	*/
	void setDRAUGHT(double DRAUGHT) {
		this->DRAUGHT = DRAUGHT;
	}

	/**
	* @return the DESTINATION
	*/
	string getDESTINATION() const{
		return DESTINATION;
	}

	/**
	* @param DESTINATION the DESTINATION to set
	*/
	void setDESTINATION(string DESTINATION) {
		this->DESTINATION = DESTINATION;
	}

	/**
	* @return the CALLSIGN
	*/
	string getCALLSIGN() const{
		return CALLSIGN;
	}

	/**
	* @param CALLSIGN the CALLSIGN to set
	*/
	void setCALLSIGN(string CALLSIGN) {
		this->CALLSIGN = CALLSIGN;
	}

	/**
	* @return the STREAMID
	*/
	string getSTREAMID() const{
		return STREAMID;
	}

	/**
	* @param STREAMID the STREAMID to set
	*/
	void setSTREAMID(string STREAMID) {
		this->STREAMID = STREAMID;
	}

	/**
	* @return the POSACCURACY
	*/
	double getPOSACCURACY() const{
		return POSACCURACY;
	}

	/**
	* @param POSACCURACY the POSACCURACY to set
	*/
	void setPOSACCURACY(double POSACCURACY) {
		this->POSACCURACY = POSACCURACY;
	}

	/**
	* @return the ETA
	*/
	double getETA() const{
		return ETA;
	}

	/**
	* @param ETA the ETA to set
	*/
	void setETA(double ETA) {
		this->ETA = ETA;
	}

	/**
	* @return the PARTNUMBER 
	*/
	double getPARTNUMBER() const{
		return PARTNUMBER;
	}

	/**
	* @param PARTNUMBER the PARTNUMBER to set
	*/
	void setPARTNUMBER(double PARTNUMBER) {
		this->PARTNUMBER = PARTNUMBER;
	}

	/**
	* @return the POSFIXTYPE
	*/
	double getPOSFIXTYPE() const
	{
		return POSFIXTYPE;
	}

	/**
	* @param POSFIXTYPE the POSFIXTYPE to set
	*/
	void setPOSFIXTYPE(double POSFIXTYPE) {
		this->POSFIXTYPE = POSFIXTYPE;
	}

	string getJSON() const
	{
		stringstream ss;
		ss << setprecision(10) << 
			"{ \"messageType\":" << this->getMESSAGETYPE() << ", " <<
			"\"mmsi\":" << this->getMMSI() << ", " <<
			"\"navStatus\":" << this->getNAVSTATUS() << ", " <<
			"\"rot\":" << this->getROT() << ", " <<
			"\"sog\":" << this->getSOG() << ", " <<
			"\"lon\":" << this->getLON() << ", " <<
			"\"lat\":" << this->getLAT() << ", " <<
			"\"cog\":" << this->getCOG() << ", " <<
			"\"trueHeading\":" << this->getTRUE_HEADING() << ", " <<
			"\"dateTime\":" << this->getDATETIME() << ", " <<
			"\"imo\":" << this->getIMO() << ", " <<
			"\"vesselName\": \"" <<  cleanForJSON(this->getVESSELNAME()) << "\", " <<
			"\"vesselTypeInt\":" << this->getVESSELTYPEINT() << ", " <<
			"\"shipLength\":" << this->getSHIPLENGTH() << ", " <<
			"\"shipWidth\":" << this->getSHIPWIDTH() << ", " <<
			"\"bow\":" << this->getBOW() << ", " <<
			"\"stern\":" << this->getSTERN() << ", " <<
			"\"port\":" << this->getPORT() << ", " <<
			"\"starboard\":" << this->getSTARBOARD() << ", " <<
			"\"draught\":" << this->getDRAUGHT() << ", " <<
			"\"destination\": \"" <<  cleanForJSON(this->getDESTINATION()) << "\", " <<
			"\"callsign\": \"" <<  cleanForJSON(this->getCALLSIGN()) << "\", " <<
			"\"positionAccuracy\":" << this->getPOSACCURACY() << ", " <<
			"\"eta\":" << this->getETA() << ", " <<
			"\"positionFixType\":" << this->getPOSFIXTYPE() << ", " <<
			"\"streamId\": \"" <<  cleanForJSON(this->getSTREAMID()) << "\"}";
		return ss.str();
	}

	double time;
	int Year;
	int Month;
	int Day;
	int Hour;
	int Minute;
	int Second;
	double UTCTime;
	std::string VesselType;
	std::string Status;
	std::string AISChannel;
	int ImageIndex;
	std::string AssociatedImageTime;
	std::string AssociatedImageSensor;
	double AssociatedImageResolution;

private:

	std::string cleanForJSON(const std::string& input) const{
		return boost::replace_all_copy(boost::replace_all_copy(input, "\\", "\\\\"), "\"", "\\\"");
	}

	int MESSAGETYPE;
	double MMSI;
	int NAVSTATUS;
	double ROT;
	double SOG;
	double LAT;
	double LON;
	double COG;
	double TRUE_HEADING;
	int DATETIME;
	double IMO;
	string VESSELNAME;
	int VESSELTYPEINT;
	double SHIPLENGTH;
	double SHIPWIDTH;
	double BOW;
	double STERN;
	double PORT;
	double STARBOARD;
	double DRAUGHT;
	string DESTINATION;
	string CALLSIGN;
	double POSACCURACY;
	double ETA;
	double PARTNUMBER;
	double POSFIXTYPE;
	string STREAMID;

};

#endif
