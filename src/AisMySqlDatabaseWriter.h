#ifndef AisMySqlDatabaseWriter_h
#define AisMySqlDatabaseWriter_h

#include <exception>
#include <stdexcept>
#include <iostream>

#include <string>
#include <sstream>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/warning.h>
#include <cppconn/metadata.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/statement.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
//had to edit mysql/cppcon/config.h to #define _int_8

#include <AisMessage.h>
#include <AisWriter.h>

/**
Class for writing AIS messages to an MySql database.
Declare...check isReady()...writeEntry
*/
class AisMySqlDatabaseWriter : public AisWriter{
public:
	AisMySqlDatabaseWriter(std::string username, std::string password, std::string hostname, std::string databaseName, std::string tableName, int iterations = 100000)
	{
		m_currentIteration = 1;
		m_username=username;
		m_password=password;
		m_hostname = hostname;
		m_databaseName=databaseName;
		m_tableName=tableName;
		m_iterations = iterations;

		m_initialized = init();
	}

	~AisMySqlDatabaseWriter()
	{
		aisDebug("AisMySqlDatabaseWriter Destructor");
		if(m_initialized)
		{
			try
			{
				aisDebug("trying to execute update any remaining entries");
				//execute statement to add any remainign
				m_sqlPreparedStatement->executeUpdate();
			}
			catch(sql::SQLException &e)
			{
				cerr << "Error on Iteration: " << m_currentIteration << endl;
				cerr << "mysql Error : " << e.what() << endl;
			}

			disconnectFromDatabase();
		
		}
	}


	void disconnectFromDatabase()
	{
		cerr << "Disconnecting from database..." << endl;
		//Terminate Oracle connection
		try
		{
			m_con->close();
			delete m_con;
			delete m_sqlPreparedStatement;
		}
		catch(sql::SQLException &e)
		{
			cerr << "Error while freeing resources : " << e.what() << endl;
		}
		cerr << "Disconnected from database"<<endl;
	}

	void setNumberOfFields(int numFields)
	{
	
	}

	bool writeEntry(const AisMessage& message)
	{
		try
		{
			m_sqlPreparedStatement->setInt(1,     message.getMESSAGETYPE());
			m_sqlPreparedStatement->setInt(2,     message.getMMSI());
			m_sqlPreparedStatement->setInt(3,     message.getNAVSTATUS());
			m_sqlPreparedStatement->setDouble(4,  message.getROT());
			m_sqlPreparedStatement->setDouble(5,  message.getSOG());
			m_sqlPreparedStatement->setDouble(6,  message.getLON());
			m_sqlPreparedStatement->setDouble(7,  message.getLAT());
			m_sqlPreparedStatement->setDouble(8,  message.getCOG());
			m_sqlPreparedStatement->setInt(9,     message.getTRUE_HEADING());
			m_sqlPreparedStatement->setInt(10,    message.getDATETIME());
			m_sqlPreparedStatement->setInt(11,    message.getIMO());
			m_sqlPreparedStatement->setString(12, message.getVESSELNAME());
			m_sqlPreparedStatement->setInt(13,    message.getVESSELTYPEINT());
			m_sqlPreparedStatement->setInt(14, message.getSHIPLENGTH());
			m_sqlPreparedStatement->setInt(15, message.getSHIPWIDTH());
			m_sqlPreparedStatement->setInt(16, message.getBOW());
			m_sqlPreparedStatement->setInt(17, message.getSTERN());
			m_sqlPreparedStatement->setInt(18, message.getPORT());
			m_sqlPreparedStatement->setInt(19, message.getSTARBOARD());
			m_sqlPreparedStatement->setInt(20, message.getDRAUGHT());
			m_sqlPreparedStatement->setString(21, message.getDESTINATION());
			m_sqlPreparedStatement->setString(22, message.getCALLSIGN());
			m_sqlPreparedStatement->setInt(23, message.getPOSACCURACY());
			m_sqlPreparedStatement->setInt(24, message.getETA());
			m_sqlPreparedStatement->setInt(25,    message.getPOSFIXTYPE());
			m_sqlPreparedStatement->setString(26, message.getSTREAMID());
			
		
			m_currentIteration = 1;
			aisDebug("executing update start");
			m_sqlPreparedStatement->executeUpdate();
			aisDebug("executing update finish");
			return true;
		
		}
		catch(sql::SQLException &e)
		{
			cerr << "Error on Iteration: " << m_currentIteration << endl;
			cerr << "Oracle Error : " << e.what() << endl;
			return false;
		}
	}

	bool isReady()
	{
		return m_initialized;
	}

private:

	bool init()
	{
		aisDebug("Initializing database");
		try
		{
			m_driver = sql::mysql::get_driver_instance();
			if(!m_driver){
				throw std::runtime_error("Could not create MySql driver");
			}
			aisDebug(m_hostname);
			aisDebug(m_username);
			aisDebug(m_password);
			m_con = m_driver->connect(m_hostname, m_username, m_password);
			if(!m_con){
				throw std::runtime_error("Could not create Mysql connection");
			}
			m_con->setAutoCommit(true);
			try
			{
				sql::Statement * sqlStatement = m_con->createStatement();
				sqlStatement->execute("USE " + m_databaseName);
				delete sqlStatement;
			}
			catch (sql::SQLException &e)
			{      
				std::cerr << "Statement Execution Error : " << e.what() << std::endl;
				return false;
			} 
			try
			{
				m_sqlPreparedStatement = m_con->prepareStatement("INSERT INTO " + m_tableName + " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
				if(!m_sqlPreparedStatement){
					throw std::runtime_error("Could not create Mysql sql statement");
				}
				aisDebug("Query prepared successfully...");
			}
			catch (sql::SQLException &e)
			{      
				std::cerr << "Statement Creation Error : " << e.what() << std::endl;
				return false;
			} 

			

		}catch (sql::SQLException &e){      
			 std::cerr << "Mysql Error : " << e.what() << std::endl;
			 return false;
		}catch (std::exception &e){
			std::cerr << "Exception: " << e.what() << std::endl;
			return false;
		}

		aisDebug("Database initialized successfully");
		return true;

	}


	string m_username;
	string m_password;
	string m_hostname;
	string m_databaseName;
	string m_tableName;
	int m_iterations;
	int m_currentIteration;
	bool m_initialized;
	
	sql::Driver * m_driver;
	sql::Connection * m_con;
	sql::PreparedStatement * m_sqlPreparedStatement;
};

#endif
