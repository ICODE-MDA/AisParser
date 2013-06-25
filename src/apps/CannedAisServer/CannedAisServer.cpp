#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using boost::asio::ip::tcp;
using namespace std;


void usage()
{
	std::cerr << "This program will broadcast text on a TCP port specified on the command prompt" << endl;
	std::cerr << "Each AIS message must be no longer than 1024 characters" << std::endl;
	std::cerr << "\nUsage:" << endl;
	std::cerr << "CannedAisServer.exe <ais-file-to-broadcast> <port-to-broadcast-on> <ms-between-messages> <option keepRepeating (bool)>" << std::endl;
	std::cerr << "For example:\nCannedAisServer.exe 20120101.log 2401 10" << std::endl;
	std::cerr << "****WARNING****\nThis program loads the entire file into memory before broadcasting. " 
				 "It is meant for small amounts of data. Do not give large files as input as it may "
				 "result in system crashes.\n***************" << std::endl;
}

void loadData(string filename, vector<string> &messages)
{
	ifstream inFile(filename.c_str(), std::ios_base::in);
	if(!inFile.is_open())
	{
		cerr << "Could not open input file: " << filename << endl;
		return;
	}
	
	char message[1024];
	while(!inFile.eof())
	{
		inFile.getline(message,1024);
		messages.push_back(message);
	}
	
	inFile.close();
}

int main(int argc, char** argv)
{

	if(argc>5 || argc < 2)
	{
		usage();
		return -1;
	}
	
	string filename = argv[1];
	int port = boost::lexical_cast<int>(argv[2]);
	int msBetweenMessages = boost::lexical_cast<int>(argv[3]);
	int keepRepeating = 1;
	if (argc == 5) {
		keepRepeating = boost::lexical_cast<int>(argv[4]);
		
	}

	std::vector <std::string> messages;
	cout << "Loading data from: " << filename << endl;
	loadData(filename, messages);
	int numMessages = messages.size();

	if(numMessages<1)
	{
		cerr << "Did not load any messages to broadcast!" << endl;
		return -1;
	}

	cout << "Broadcasting " << numMessages << " messages on port: " << port << endl;
	int idx=0;
	
	try
	{
		boost::asio::io_service io_service;

		tcp::endpoint endpoint(tcp::v4(), port);
		tcp::acceptor acceptor(io_service, endpoint);
		tcp::iostream stream;
		boost::system::error_code ec;
		//cout << "Are you ready to send data?";
		//string ans;
		//cin >> ans;
		//cout << "Your answer " << ans << endl;
		acceptor.accept(*stream.rdbuf(), ec);
		
		
		while (!ec)
		{
			if (idx < numMessages) {
				cout << "Message #" << idx << ": " << messages[idx] << endl;
			}
			boost::asio::deadline_timer timer(io_service, boost::posix_time::milliseconds(msBetweenMessages));
			timer.wait();
			stream << messages[idx++] << endl;
			
			if(idx==numMessages){
				if (keepRepeating) {
					idx =0;
				} 
				else {
					cout << "Exit: End of file reached " << endl;
					exit(1);
				}
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

  return 0;
}
