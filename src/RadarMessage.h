#ifndef RADARMESSAGE_H
#define RADARMESSAGE_H
#include <string>

using namespace std;

class RadarMessage {

public:

	RadarMessage()
	{
		/*
		//type 1 (RMAC),2 (TTM)
		type
		track
		status
		speed (sog)
		lon
		lat
		course
		heading
		source
		datetime

		//type 1(RMAC) additional
		vessel name
		mmsi

		*/

		MESSAGETYPE = -1;
		TRACKNUM = -1;
		TARGETSTATUS = -1;
		TARGETACQ = -1;
		SOG = -1;
		LAT = -999;
		LON = -999;
		COG = -1;
		TRUE_HEADING = -999;
		DATETIME = -1;
		IMO = -1;
		VESSELNAME = "";
		STREAMID = "";
		SOURCE = "";






		//Additional things from original AISDot
		/*time=-1;
		Year=-1;
		Month=-1;
		Day=-1;
		Hour=-1;
		Minute=-1;
		Second=-1;*/
		UTCTime=-1;
		/*VesselType="";
		Status="";
		AISChannel="";
		ImageIndex=-1;
		AssociatedImageTime="";
		AssociatedImageSensor="";
		AssociatedImageResolution=-1;*/
	}
	
	bool operator != (const RadarMessage& lhs) const{
		return !(operator==(lhs));
	}

	bool operator == (const RadarMessage& lhs) const{

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

	bool equalWithExceptionOfTime (const RadarMessage& lhs) const{

		if(
			this->COG == lhs.getCOG() &&
			this->IMO == lhs.getIMO() &&
			this->LAT == lhs.getLAT() &&
			this->LON == lhs.getLON() &&
			this->MESSAGETYPE == lhs.getMESSAGETYPE() &&
			this->MMSI == lhs.getMMSI() &&
			this->TARGETSTATUS == lhs.getTARGETSTATUS() &&
			this->TARGETACQ == lhs.getTARGETACQ() &&
			this->SOG == lhs.getSOG() &&
			this->STREAMID == lhs.getSTREAMID() &&
			this->TRUE_HEADING == lhs.getTRUE_HEADING() &&
			this->VESSELNAME == lhs.getVESSELNAME()
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
	string getTARGETSTATUS() const
	{
		return TARGETSTATUS;
	}
	void setTARGETSTATUS(string targetstatus)
	{
		this->TARGETSTATUS = targetstatus;
	}
	string getTARGETACQ() const
	{
		return TARGETACQ;
	}
	void setTARGETACQ(string targetacq)
	{
		this->TARGETACQ = targetacq;
	}
	int getTRACKNUM() const
	{
		return TRACKNUM;
	}
	void setTRACKNUM(int tracknum)
	{
		this->TRACKNUM = tracknum;
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


	string getSOURCE() const
	{
		return SOURCE;
	}
	void setSOURCE(string source)
	{
		this->SOURCE = source;
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

	int MESSAGETYPE;
	double MMSI;
	string TARGETSTATUS;
	string TARGETACQ;
	double SOG;
	double LAT;
	double LON;
	double COG;
	double TRUE_HEADING;
	int DATETIME;
	double IMO;
	string VESSELNAME;
	string STREAMID;
	string SOURCE;
	int TRACKNUM;
};

#endif
