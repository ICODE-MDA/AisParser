#ifndef AisCsvWriter_h
#define AisCsvWriter_h

#include <string>
#include <fstream>
#include <iomanip>

#include <AisWriter.h>
#include <AisMessage.h>

class AisCsvWriter : public AisWriter{
public:
	AisCsvWriter(std::string filename){
		of.open(filename + ".csv", std::ios::out);
	}
	AisCsvWriter(int year, int month, int day, int partition){
		stringstream filename;
		filename << setfill('0');
		filename << year;
		filename << setw(2) << month;
		filename << setw(2) << day;
		filename << ".p" << partition << ".csv";
		of.open(filename.str(), std::ios::out);
	}
	~AisCsvWriter(){
		if(of.is_open()){
			of.close();
		}
	}

	bool writeEntry(const AisMessage& message){
		of << setprecision(10) << 
			message.getMESSAGETYPE() << "," <<
			message.getMMSI() << "," <<
			message.getNAVSTATUS() << "," <<
			message.getROT() << "," <<
			message.getSOG() << "," <<
			message.getLON() << "," <<
			message.getLAT() << "," <<
			message.getCOG() << "," <<
			message.getTRUE_HEADING() << "," <<
			message.getDATETIME() << "," <<
			message.getIMO() << "," <<
			message.getVESSELNAME() << "," <<
			message.getVESSELTYPEINT() << "," <<
			message.getSHIPLENGTH() << "," <<
			message.getSHIPWIDTH() << "," <<
			message.getBOW() << "," <<
			message.getSTERN() << "," <<
			message.getPORT() << "," <<
			message.getSTARBOARD() << "," <<
			message.getDRAUGHT() << "," <<
			message.getDESTINATION() << "," <<
			message.getCALLSIGN() << "," <<
			message.getPOSACCURACY() << "," <<
			message.getETA() << "," <<
			message.getPOSFIXTYPE() << "," <<
			message.getSTREAMID() << endl;
		return true;
	}

	bool isReady(){
		return of.is_open();
	}

private:
	std::ofstream of;
};
#endif
