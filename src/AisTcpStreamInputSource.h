#ifndef AisTcpStreamInputSource_h
#define AisTcpStreamInputSource_h

#include <iostream>
#include <vector>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <AisInputSource.h>

using boost::asio::ip::tcp;

/**
This class handles grabbing external AIS data
The class can grab data from a TCP port
*/
class AisTcpStreamInputSource: public AisInputSource{
public:
    //requires that the input be a string <hostname>:<port-number>
    //For example "localhost:8080"
	AisTcpStreamInputSource(std::string hostColonPort)
	{
        std::vector<std::string> hostAndPort;
        boost::algorithm::split(hostAndPort, hostColonPort, boost::is_any_of(":"));

        if(hostAndPort.size()==2)
        {
            connect(hostAndPort[0], hostAndPort[1]);
        }
        else
        {
            connect("","");
        }
	}

	AisTcpStreamInputSource(std::string host, std::string port)
	{
        connect(host, port);
	}

    void connect(std::string host, std::string port){
        try
        {
            aisDebug("Attemping to connect to: " + host + " port: " + port);
            stream.connect(host, port);
            if (!stream)
            {
                std::cout << "Unable to connect: " << stream.error().message() << std::endl;
            }
            else
            {
                aisDebug("Connected");
            }
        }
        catch (std::exception& e)
        {
            std::cout << "Exception: " << e.what() << std::endl;
        }
    }

	/**
	Returns true if the input source is ready
	to start providing data, false otherwhise
	*/
	bool isReady(){
		return stream.good();
	}
	
	/**
	Returns the next sentence from the AIS
	input source as a string.
	*/
	std::string getNextSentence(){
		std::string sentence;
		try
		{
			std::getline(stream, sentence);
		}
		catch(std::exception& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
			return "";
		}
		return sentence;
	}

protected:
	tcp::iostream stream;
};

#endif
