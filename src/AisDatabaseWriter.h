#ifndef AisDatabaseWriter_h
#define AisDatabaseWriter_h

#include <exception>
#include <iostream>

#include <occi.h>

#include <AisMessage.h>
#include <AisWriter.h>

/**
Class for writing AIS messages to a database.
Declare...check isReady()...writeEntry
*/
class AisDatabaseWriter : public AisWriter{
public:
	AisDatabaseWriter(std::string username, std::string password, std::string hostname, std::string databaseName, std::string tableName, int iterations = 100000)
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

	~AisDatabaseWriter()
	{
		aisDebug("AisDatabaseWriter Destructor");
		if(m_initialized)
		{
			try
			{
				aisDebug("trying to execute update any remaining entries");
				//execute statement to add any remainign
				m_sqlStatement->executeUpdate();
			}
			catch(oracle::occi::SQLException e)
			{
				cerr << "Error on Iteration: " << m_currentIteration << endl;
				cerr << "Oracle Error : " << e.getMessage().c_str() << endl;
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
			m_con->terminateStatement(m_sqlStatement);
			m_env->terminateConnection(m_con);
			m_env->terminateEnvironment(m_env);
		}
		catch(oracle::occi::SQLException e)
		{
			cerr << "Error while freeing resources : " << e.getMessage().c_str() << endl;
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
			m_sqlStatement->setInt(1,     message.getMESSAGETYPE());
			m_sqlStatement->setInt(2,     message.getMMSI());
			m_sqlStatement->setInt(3,     message.getNAVSTATUS());
			m_sqlStatement->setDouble(4,  message.getROT());
			m_sqlStatement->setDouble(5,  message.getSOG());
			m_sqlStatement->setDouble(6,  message.getLON());
			m_sqlStatement->setDouble(7,  message.getLAT());
			m_sqlStatement->setDouble(8,  message.getCOG());
			m_sqlStatement->setInt(9,     message.getTRUE_HEADING());
			m_sqlStatement->setInt(10,    message.getDATETIME());
			m_sqlStatement->setInt(11,    message.getIMO());
			m_sqlStatement->setString(12, message.getVESSELNAME());
			m_sqlStatement->setInt(13,    message.getVESSELTYPEINT());
			m_sqlStatement->setInt(14, message.getSHIPLENGTH());
			m_sqlStatement->setInt(15, message.getSHIPWIDTH());
			m_sqlStatement->setInt(16, message.getBOW());
			m_sqlStatement->setInt(17, message.getSTERN());
			m_sqlStatement->setInt(18, message.getPORT());
			m_sqlStatement->setInt(19, message.getSTARBOARD());
			m_sqlStatement->setInt(20, message.getDRAUGHT());
			m_sqlStatement->setString(21, message.getDESTINATION());
			m_sqlStatement->setString(22, message.getCALLSIGN());
			m_sqlStatement->setInt(23, message.getPOSACCURACY());
			m_sqlStatement->setInt(24, message.getETA());
			m_sqlStatement->setInt(25,    message.getPOSFIXTYPE());
			m_sqlStatement->setString(26, message.getSTREAMID());

			if(m_currentIteration < m_iterations){
				m_sqlStatement->addIteration();
				m_currentIteration++;
				return true;
			}else{
				m_currentIteration = 1;
				aisDebug("executing update start");
				m_sqlStatement->executeUpdate();
				aisDebug("executing update finish");
				return true;
			}
		}
		catch(oracle::occi::SQLException e)
		{
			cerr << "Error on Iteration: " << m_currentIteration << endl;
			cerr << "Oracle Error : " << e.getMessage().c_str() << endl;
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
			m_env = oracle::occi::Environment::createEnvironment(oracle::occi::Environment::DEFAULT);
			if(!m_env){
				throw std::exception("Could not create Oracle environment");
			}
			std::string connectionString = m_hostname + "/" + m_databaseName;
			m_con = m_env->createConnection (m_username, m_password, connectionString);
			if(!m_con){
				throw std::exception("Could not create Oracle connection");
			}
		
			try
			{
				m_sqlStatement = m_con->createStatement();
				if(!m_sqlStatement){
					throw std::exception("Could not create Oracle sql statement");
				}
				m_sqlStatement->setAutoCommit(TRUE);      
			}
			catch (oracle::occi::SQLException &e)
			{      
				std::cerr << "Statement Creation Error : " << e.getMessage().c_str() << std::endl;
				return false;
			} 
		}catch (oracle::occi::SQLException &e){      
			 std::cerr << "Oracle Connect Error : " << e.getMessage().c_str() << std::endl;
			 return false;
		}catch (std::exception &e){
			std::cerr << "Exception: " << e.what() << std::endl;
			return false;
		}


		try
		{
			m_sqlStatement->setSQL("INSERT INTO " + m_tableName + " VALUES(:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17, :18, :19, :20, :21, :22, :23, :24, :25, :26)");
			m_sqlStatement->setMaxParamSize(12, 120);
			m_sqlStatement->setMaxParamSize(21, 120);
			m_sqlStatement->setMaxParamSize(22, 120);
			m_sqlStatement->setMaxParamSize(26, 120);
			m_sqlStatement->setMaxIterations(m_iterations);
		}
		catch(oracle::occi::SQLException e)
		{
			if(e.getErrorCode() != 955)
			{
				cerr << "Oracle Error: " << e.getMessage().c_str() << endl;
			}
			return false;
		}
		aisDebug("Query prepared successfully...");
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
	
	oracle::occi::Environment* m_env; // OCCI environment
	oracle::occi::Connection* m_con;  // OCCI connection
	oracle::occi::Statement* m_sqlStatement; // OCCI statement
};

#endif