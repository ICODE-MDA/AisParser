#ifndef AisKmlPlacemarkWriter_h
#define AisKmlPlacemarkWriter_h

#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <AisWriter.h>
#include <AisMessage.h>


//<?xml version="1.0" encoding="UTF-8"?>
//<kml xmlns="http://www.opengis.net/kml/2.2">
//  <Document>
//    <Placemark>
//      <name>CDATA example</name>
//      <description>
//        <![CDATA[
//          <h1>CDATA Tags are useful!</h1>
//          <p><font color="red">Text is <i>more readable</i> and 
//          <b>easier to write</b> when you can avoid using entity 
//          references.</font></p>
//        ]]>
//      </description>
//      <Point>
//        <coordinates>102.595626,14.996729</coordinates>
//      </Point>
//    </Placemark>
//  </Document>
//</kml>

class AisKmlPlacemarkWriter : public AisWriter{
public:
	AisKmlPlacemarkWriter(std::string filename){
		of.open(filename, std::ios::out);
		writeHeader();
	}

	AisKmlPlacemarkWriter(int year, int month, int day, int partition){
		stringstream filename;
		filename << setfill('0');
		filename << year;
		filename << setw(2) << month;
		filename << setw(2) << day;
		filename << ".p" << partition << ".kml";
		of.open(filename.str(), std::ios::out);
		writeHeader();
	}

	~AisKmlPlacemarkWriter(){
		if(of.is_open()){
			of << "</Document>" << endl;
			of << "</kml>" << endl;
			of.close();
		}
	}

	void writeHeader()
	{
		if(of.is_open())
		{
			of << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
			of << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">" << endl;
			of << "<Document>" << endl;
		}
	}

	bool writeEntry(const AisMessage& message){

		of.precision(10);
		of << "	<Placemark>" << endl;
		of << "		<name>" << message.getVESSELNAME() << " " << message.getMMSI() << "</name>" << endl;
		of << "		<description>" << endl;
        of << "		<![CDATA[" << endl;
        of << "			Message Type:" << message.getMESSAGETYPE() << "<br>" << endl;
		of << "			MMSI:" << message.getMMSI() << "<br>" << endl;
		of << "			Navigation Status:" << message.getNAVSTATUS() << "<br>" << endl;
		of << "			ROT:" << message.getROT() << "<br>" << endl;
		of << "			SOG:" << message.getSOG() << "<br>" << endl;
		of << "			LON:" << message.getLON() << "<br>" << endl;
		of << "			LAT:" << message.getLAT() << "<br>" << endl;
		of << "			COG:" << message.getCOG() << "<br>" << endl;
		of << "			True Heading:" << message.getTRUE_HEADING() << "<br>" << endl;
		of << "			Date Time:" << message.getDATETIME() << "<br>" << endl;
		of << "			IMO:" << message.getIMO() << "<br>" << endl;
		of << "			Vessel Name:" << message.getVESSELNAME() << "<br>" << endl;
		of << "			Vessel Type Int:" << message.getVESSELTYPEINT() << "<br>" << endl;
		of << "			Ship Length:" << message.getSHIPLENGTH() << "<br>" << endl;
		of << "			Ship Width:" << message.getSHIPWIDTH() << "<br>" << endl;
		of << "			BOW:" << message.getBOW() << "<br>" << endl;
		of << "			STERN:" << message.getSTERN() << "<br>" << endl;
		of << "			PORT:" << message.getPORT() << "<br>" << endl;
		of << "			STARBOARD:" << message.getSTARBOARD() << "<br>" << endl;
		of << "			DRAUGHT:" << message.getDRAUGHT() << "<br>" << endl;
		of << "			Destination:" << message.getDESTINATION() << "<br>" << endl;
		of << "			Callsign:" << message.getCALLSIGN() << "<br>" << endl;
		of << "			Position Accuracy:" << message.getPOSACCURACY() << "<br>" << endl;
		of << "			ETA:" << message.getETA() << "<br>" << endl;
		of << "			Position Fix Type:" << message.getPOSFIXTYPE() << "<br>" << endl;
		of << "			StreamID:" << message.getSTREAMID() << "<br>" << endl;
		of << "		]]>" << endl;
		of << "		</description>" << endl;
		of << "		<Point>" << endl;
		of << "		<coordinates>" << message.getLON() << "," << message.getLAT() << "</coordinates>" << endl;
		of << "		</Point>" << endl;
		of << "	</Placemark>" << endl;
		return true;
	}

	bool isReady(){
		return of.is_open();
	}

private:
	std::ofstream of;
};
#endif
