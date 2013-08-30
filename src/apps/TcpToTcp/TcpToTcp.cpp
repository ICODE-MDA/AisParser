#include <AisParserTemplates.h>

//Input sources
#include <AisTcpStreamInputSource.h>

//Output sources
#include <AisTcpWriter.h>

//Parsers
#include <AisSatSentenceParser.h>


//int main(int argc, char** argv)
//{
//	//parse args
//	if(argc!=4)
//	{
//		tcpToTcpParserUsage();
//		return -1;
//	}
//
//	std::string source_host = argv[1];
//	std::string source_port = argv[2];
//	std::string destination_host = argv[3];
//	std::string destination_port = argv[4];
extern "C"
//__declspec(dllexport)  void startParserService(std::string source_host, std::string source_port, std::string destination_host, std::string destination_port){
__declspec(dllexport)  void startParserService(char* source_host, char* source_port, char* destination_host, char* destination_port){
	//Define input class (an AisInputSource)
	//STEPX: choose the correct type of input source
	AisTcpStreamInputSource aisInputSource(source_host, source_port);
	tcpParser<AisTcpWriter, AisSatSentenceParser>(aisInputSource, destination_host, destination_port);
	
//	return 0;
}

