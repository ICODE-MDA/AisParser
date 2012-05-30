#ifndef AisShapefilePointWriter_h
#define AisShapefilePointWriter_h

#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <shapelib/shapefil.h>

#include <AisWriter.h>
#include <AisMessage.h>

class AisShapefilePointWriter : public AisWriter{
public:
	AisShapefilePointWriter(std::string filename){
		initializeShapefiles(filename + ".shp");
	}

	AisShapefilePointWriter(int year, int month, int day, int partition){
		stringstream filename;
		filename << setfill('0');
		filename << year;
		filename << setw(2) << month;
		filename << setw(2) << day;
		filename << ".p" << partition << ".shp";
		initializeShapefiles(filename.str());
	}

	~AisShapefilePointWriter(){
		if(shpHandle)
		{
			SHPClose(shpHandle);
		}

		if(dbfHandle)
		{
			DBFClose( dbfHandle);
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
		shpHandle = SHPCreate(filename.c_str(), SHPT_POINT);
		dbfHandle = DBFCreate(filename.c_str());
	}

	bool writeEntry(const AisMessage& message){

		double longitude = message.getLON();
		double latitude = message.getLAT();
		int panParts = 0;

		SHPObject* shpObject = SHPCreateSimpleObject( SHPT_POINT, 1, &longitude, &latitude, NULL);
		SHPWriteObject(shpHandle, -1 , shpObject);
		SHPDestroyObject(shpObject);

		DBFWriteIntegerAttribute(dbfHandle, currentShape, 0, message.getMESSAGETYPE());
		DBFWriteIntegerAttribute(dbfHandle, currentShape, 1, message.getMMSI());
		DBFWriteIntegerAttribute(dbfHandle, currentShape, 2, message.getNAVSTATUS());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 3, message.getROT());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 4, message.getSOG());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 5, message.getLON());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 6, message.getLAT());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 7, message.getCOG());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 8, message.getTRUE_HEADING());
		DBFWriteIntegerAttribute(dbfHandle, currentShape, 9, message.getDATETIME());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 10, message.getIMO());
		DBFWriteStringAttribute(dbfHandle, currentShape, 11, message.getVESSELNAME().c_str());
		DBFWriteIntegerAttribute(dbfHandle, currentShape, 12, message.getVESSELTYPEINT());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 13, message.getSHIPLENGTH());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 14, message.getSHIPWIDTH());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 15, message.getBOW());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 16, message.getSTERN());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 17, message.getPORT());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 18, message.getSTARBOARD());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 19, message.getDRAUGHT());
		DBFWriteStringAttribute(dbfHandle, currentShape, 20, message.getDESTINATION().c_str());
		DBFWriteStringAttribute(dbfHandle, currentShape, 21, message.getCALLSIGN().c_str());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 22, message.getPOSACCURACY());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 23, message.getETA());
		DBFWriteDoubleAttribute(dbfHandle, currentShape, 24, message.getPOSFIXTYPE());
		DBFWriteStringAttribute(dbfHandle, currentShape++, 25, message.getSTREAMID().c_str());

		return true;
	}

	bool isReady(){
		return (shpHandle!=NULL) && (dbfHandle!=NULL);
	}

private:
	unsigned int currentShape;
	SHPHandle shpHandle;
	DBFHandle dbfHandle;
};
#endif
