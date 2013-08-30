#ifndef AisTcpWriter_h
#define AisTcpWriter_h

#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <AisWriter.h>
#include <AisMessage.h>

using boost::asio::ip::tcp;

class AisTcpWriter : public AisWriter{
public:
	AisTcpWriter(std::string hostname, std::string port):
	  endpoint(tcp::v4(), boost::lexical_cast<int>(port)),
	  acceptor(io_service, endpoint)
	{
		try{
			aisDebug("Attemping to connect to: " + hostname + " port: " + port);
			connect(hostname, port);
			if (!stream)
			{
				std::cout << "Unable to connect: " << stream.error().message() << std::endl;
			}
			else
			{
				aisDebug("Connected");
			}
		}catch (std::exception& e)
        {
            std::cout << "Exception: " << e.what() << std::endl;
        }
	}

	~AisTcpWriter(){
		stream.close();
	}

	 void connect(std::string host, std::string port){
		//int portInt = boost::lexical_cast<int>(port);
		//endpoint = tcp::endpoint(tcp::v4(), portInt);
		//acceptor = tcp::acceptor(io_service, endpoint);
		acceptor.accept(*stream.rdbuf(), ec);
		 
		 //try
        //{
        //    aisDebug("Attemping to connect to: " + host + " port: " + port);
        //    stream.connect(host, port);
        //    if (!stream)
        //    {
        //        std::cout << "Unable to connect: " << stream.error().message() << std::endl;
        //    }
        //    else
        //    {
        //        aisDebug("Connected");
        //    }
        //}
        //catch (std::exception& e)
        //{
        //    std::cout << "Exception: " << e.what() << std::endl;
        //}
    }

	bool writeEntry(const AisMessage& message){
		//std::string = message.getJSON();
		//cerr << message.getJSON() << endl;
		
		stream << message.getJSON() << endl;
		return true;
	}

	bool isReady(){
		return stream.good();
	}

private:
	tcp::iostream stream;
	boost::asio::io_service io_service;
	tcp::endpoint endpoint;
	tcp::acceptor acceptor;
	boost::system::error_code ec;
};
#endif
