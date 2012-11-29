#ifndef AisPostgreSqlDatabaseWriter_h
#define AisPostgreSqlDatabaseWriter_h

#include <exception>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>

#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

#include <AisMessage.h>
#include <AisWriter.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>


/**
Class for writing AIS messages to an PostgreSql database.
Declare...check isReady()...writeEntry
*/
class AisPostgreSqlDatabaseWriter : public AisWriter{
public:
	AisPostgreSqlDatabaseWriter(std::string username, std::string password, std::string hostname, std::string databaseName, std::string tableName, int iterations = 100000):
	m_con(static_cast<pqxx::connection*>(0)),
		m_currentIteration(1),
		m_username(username),
		m_password(password),
		m_hostname(hostname),
		m_databaseName(databaseName),
		m_tableName(tableName),
		m_iterations(iterations),
		m_sqlStatement("")
	{
		m_initialized = init();
	}

	~AisPostgreSqlDatabaseWriter()
	{
		aisDebug("AisPostgreSqlDatabaseWriter Destructor");
		if(m_initialized)
		{
			try
			{
				aisDebug("trying to execute update any remaining entries");

				if(m_sqlStatement != "")
				{
					//execute statement to add any remainign
					//aisDebug("executing multirow insert start");
					StatementExecutor statementExecutor(m_sqlStatement);
					m_con->perform(statementExecutor);
					m_sqlStatement = string("");
					//aisDebug("executing multirow insert end");
					//m_sqlPreparedStatement->executeUpdate();
				}
			}
			catch(const exception &e)
			{
				cerr << "Error on Iteration: " << m_currentIteration << endl;
				cerr << "Error : " << e.what() << endl;
			}

			disconnectFromDatabase();
		
		}
	}


	void disconnectFromDatabase()
	{
		cerr << "Disconnecting from database..." << endl;
		//Terminate Database connection
		try
		{
			m_con->disconnect();
		}
		catch(const exception &e)
		{
			cerr << "Error while freeing resources : " << e.what() << endl;
		}
		cerr << "Disconnected from database"<<endl;
	}

	void setNumberOfFields(int numFields)
	{
	
	}
	
	std::string sanitize(const std::string& in)
	{
		return m_con->esc(in);
	}


	bool writeEntry(const AisMessage& message)
	{
		try
		{	
			if(m_currentIteration == 1 || m_iterations <= 0)
			{
				m_sqlStatement = "INSERT INTO " + m_tableName + " VALUES(DEFAULT, ";
			}
			else
			{
				m_sqlStatement+= ", (DEFAULT, ";
			}

			m_sqlStatement+=
				boost::lexical_cast<std::string>(message.getMESSAGETYPE()) + ", " +
				boost::lexical_cast<std::string>(message.getMMSI())+ ", " +
				boost::lexical_cast<std::string>(message.getNAVSTATUS())+ ", " +
				boost::lexical_cast<std::string>(message.getROT())+ ", " +
				boost::lexical_cast<std::string>(message.getSOG())+ ", " +
				boost::lexical_cast<std::string>(message.getLON())+ ", " +
				boost::lexical_cast<std::string>(message.getLAT())+ ", " +
				boost::lexical_cast<std::string>(message.getCOG())+ ", " +
				boost::lexical_cast<std::string>(message.getTRUE_HEADING())+ ", " +
				boost::lexical_cast<std::string>(message.getDATETIME())+ ", " +
				boost::lexical_cast<std::string>(message.getIMO())+ ", '" +
				sanitize(boost::lexical_cast<std::string>(message.getVESSELNAME()))+ "', " +
				boost::lexical_cast<std::string>(message.getVESSELTYPEINT())+ ", " +
				boost::lexical_cast<std::string>(message.getSHIPLENGTH())+ ", " +
				boost::lexical_cast<std::string>(message.getSHIPWIDTH())+ ", " +
				boost::lexical_cast<std::string>(message.getBOW())+ ", " +
				boost::lexical_cast<std::string>(message.getSTERN())+ ", " +
				boost::lexical_cast<std::string>(message.getPORT())+ ", " +
				boost::lexical_cast<std::string>(message.getSTARBOARD())+ ", " +
				boost::lexical_cast<std::string>(message.getDRAUGHT())+ ", '" +
				sanitize(boost::lexical_cast<std::string>(message.getDESTINATION()))+ "', '" +
				sanitize(boost::lexical_cast<std::string>(message.getCALLSIGN()))+ "', " +
				boost::lexical_cast<std::string>(message.getPOSACCURACY())+ ", " +
				boost::lexical_cast<std::string>(message.getETA())+ ", " +
				boost::lexical_cast<std::string>(message.getPOSFIXTYPE())+ ", '" +
				sanitize(boost::lexical_cast<std::string>(message.getSTREAMID()))+ "')";
			//cout << m_sqlStatement << endl;
			if(m_currentIteration++ == m_iterations || m_iterations <= 0)
			{
				m_currentIteration = 1;

				//aisDebug("executing multirow insert start");
				StatementExecutor statementExecutor(m_sqlStatement);
				m_con->perform(statementExecutor);
				m_sqlStatement = string("");
				//aisDebug("executing multirow insert end");
			}
			return true;

		}
		catch(const exception &e)
		{
			cerr << "Error on Iteration: " << m_currentIteration << endl;
			cerr << "PostgreSQL Error : " << e.what() << endl;
			return false;
		}
	}
	bool writeDynamicEntry(const AisMessage& message)
	{
		//time_t seconds;
		//struct tm * ptm;
		try
		{	
			if(m_currentIteration == 1 || m_iterations <= 0)
			{
				//m_sqlStatement = "INSERT INTO " + m_tableName + " VALUES(DEFAULT, ";
				m_sqlStatement = "INSERT INTO " + m_tableName + " VALUES(DEFAULT,'A',1,1, ";
			}
			else
			{
				m_sqlStatement+= ", (DEFAULT, ";
			}

			m_sqlStatement+=
				//boost::lexical_cast<std::string>(message.getMESSAGETYPE()) + ", " +
				//boost::lexical_cast<std::string>(message.getMMSI())+ ", " +
				//seconds = message.getDATETIME();
				//ptm = gmtime(&seconds);
				//cout << "asctime " << asctime(ptm) << endl;
				
				//boost::lexical_cast<std::string>(message.getLON())+ ", " +
				//boost::lexical_cast<std::string>(message.getLAT())+ ", " +
			//Need to add version, ais_static_id and gen_unique_id 	
				boost::lexical_cast<std::string>(message.getCOG())+ ", " +
				boost::lexical_cast<std::string>(message.getSOG())+ ", " +
				boost::lexical_cast<std::string>(message.getTRUE_HEADING())+ ", " +
				boost::lexical_cast<std::string>(message.getPOSACCURACY())+ ", " +
				"to_timestamp(" +
				boost::lexical_cast<std::string>(message.getDATETIME())+ "), " +
				boost::lexical_cast<std::string>(message.getNAVSTATUS())+ ", " +
				//boost::lexical_cast<std::string>(message.getROT())+ "')";
				boost::lexical_cast<std::string>(message.getROT())+ ",1')";
			//Need to add Sentence_type

				/*boost::lexical_cast<std::string>(message.getIMO())+ ", '" +
				sanitize(boost::lexical_cast<std::string>(message.getVESSELNAME()))+ "', " +
				boost::lexical_cast<std::string>(message.getVESSELTYPEINT())+ ", " +
				boost::lexical_cast<std::string>(message.getSHIPLENGTH())+ ", " +
				boost::lexical_cast<std::string>(message.getSHIPWIDTH())+ ", " +
				boost::lexical_cast<std::string>(message.getBOW())+ ", " +
				boost::lexical_cast<std::string>(message.getSTERN())+ ", " +
				boost::lexical_cast<std::string>(message.getPORT())+ ", " +
				boost::lexical_cast<std::string>(message.getSTARBOARD())+ ", " +
				boost::lexical_cast<std::string>(message.getDRAUGHT())+ ", '" +
				sanitize(boost::lexical_cast<std::string>(message.getDESTINATION()))+ "', '" +
				sanitize(boost::lexical_cast<std::string>(message.getCALLSIGN()))+ "', " +
				
				boost::lexical_cast<std::string>(message.getETA())+ ", " +
				boost::lexical_cast<std::string>(message.getPOSFIXTYPE())+ ", '" +
				sanitize(boost::lexical_cast<std::string>(message.getSTREAMID()))+ "')";*/
			cout << m_sqlStatement << endl;
			if(m_currentIteration++ == m_iterations || m_iterations <= 0)
			{
				m_currentIteration = 1;

				//aisDebug("executing multirow insert start");
				StatementExecutor statementExecutor(m_sqlStatement);
				m_con->perform(statementExecutor);
				m_sqlStatement = string("");
				//aisDebug("executing multirow insert end");
			}
			return true;

		}
		catch(const exception &e)
		{
			cerr << "Error on Iteration: " << m_currentIteration << endl;
			cerr << "PostgreSQL Error : " << e.what() << endl;
			return false;
		}
	}
	bool writeStaticEntry(const AisMessage& message)
	{
		try
		{	
			if(m_currentIteration == 1 || m_iterations <= 0)
			{
				m_sqlStatement = "INSERT INTO " + m_tableName + " VALUES(DEFAULT, ";
			}
			else
			{
				m_sqlStatement+= ", (DEFAULT, ";
			}

			m_sqlStatement+=
				//boost::lexical_cast<std::string>(message.getMESSAGETYPE()) + ", " +
				
				
				
				//boost::lexical_cast<std::string>(message.getLON())+ ", " +
				//boost::lexical_cast<std::string>(message.getLAT())+ ", " +
			//Need to add version, gen_unique_id.  Need to add update capability to existing table	
				boost::lexical_cast<std::string>(message.getIMO())+ ", '" +
				boost::lexical_cast<std::string>(message.getMMSI())+ ", " +
				sanitize(boost::lexical_cast<std::string>(message.getCALLSIGN()))+ "', " +
				boost::lexical_cast<std::string>(message.getDATETIME())+ ", " +
				sanitize(boost::lexical_cast<std::string>(message.getVESSELNAME()))+ "', " +
				boost::lexical_cast<std::string>(message.getDRAUGHT())+ ", '" +
				boost::lexical_cast<std::string>(message.getVESSELTYPEINT())+ ", " +
				boost::lexical_cast<std::string>(message.getBOW())+ ", " +
				boost::lexical_cast<std::string>(message.getPORT())+ ", " +
				boost::lexical_cast<std::string>(message.getSTARBOARD())+ ", " +
				boost::lexical_cast<std::string>(message.getSTERN())+ ", " +
				boost::lexical_cast<std::string>(message.getSHIPLENGTH())+ ", " +
				boost::lexical_cast<std::string>(message.getSHIPWIDTH())+ ", " +
				sanitize(boost::lexical_cast<std::string>(message.getDESTINATION()))+ "', '" +
				boost::lexical_cast<std::string>(message.getETA())+ ", " +
				boost::lexical_cast<std::string>(message.getPOSFIXTYPE())+ "')";
				/*boost::lexical_cast<std::string>(message.getCOG())+ ", " +
				boost::lexical_cast<std::string>(message.getSOG())+ ", " +
				boost::lexical_cast<std::string>(message.getTRUE_HEADING())+ ", " +
				boost::lexical_cast<std::string>(message.getPOSACCURACY())+ ", " +
				
				boost::lexical_cast<std::string>(message.getNAVSTATUS())+ ", " +
				boost::lexical_cast<std::string>(message.getROT())+ "')";*/

				//sanitize(boost::lexical_cast<std::string>(message.getSTREAMID()))+ "')";
			//cout << m_sqlStatement << endl;
			if(m_currentIteration++ == m_iterations || m_iterations <= 0)
			{
				m_currentIteration = 1;

				//aisDebug("executing multirow insert start");
				StatementExecutor statementExecutor(m_sqlStatement);
				m_con->perform(statementExecutor);
				m_sqlStatement = string("");
				//aisDebug("executing multirow insert end");
			}
			return true;

		}
		catch(const exception &e)
		{
			cerr << "Error on Iteration: " << m_currentIteration << endl;
			cerr << "PostgreSQL Error : " << e.what() << endl;
			return false;
		}
	}
	bool writeTargetEntry(const AisMessage& message)
	{
		try
		{	
			if(m_currentIteration == 1 || m_iterations <= 0)
			{
				m_sqlStatement = "INSERT INTO " + m_tableName + " VALUES(DEFAULT, ";
			}
			else
			{
				m_sqlStatement+= ", (DEFAULT, ";
			}

			m_sqlStatement+=
				
				
				boost::lexical_cast<std::string>(message.getDATETIME())+ ", " +
				sanitize(boost::lexical_cast<std::string>(message.getSTREAMID()))+ "')";
				boost::lexical_cast<std::string>(message.getMESSAGETYPE()) + ", " +
				boost::lexical_cast<std::string>(message.getLON())+ ", " +
				boost::lexical_cast<std::string>(message.getLAT())+ ", " +

				
			//Need to add version, gen_unique_id.  Need to add update capability to existing table	
				boost::lexical_cast<std::string>(message.getIMO())+ ", '" +
				boost::lexical_cast<std::string>(message.getMMSI())+ ", " +
				sanitize(boost::lexical_cast<std::string>(message.getCALLSIGN()))+ "', " +
				
				sanitize(boost::lexical_cast<std::string>(message.getVESSELNAME()))+ "', " +
				boost::lexical_cast<std::string>(message.getDRAUGHT())+ ", '" +
				boost::lexical_cast<std::string>(message.getVESSELTYPEINT())+ ", " +
				boost::lexical_cast<std::string>(message.getBOW())+ ", " +
				boost::lexical_cast<std::string>(message.getPORT())+ ", " +
				boost::lexical_cast<std::string>(message.getSTARBOARD())+ ", " +
				boost::lexical_cast<std::string>(message.getSTERN())+ ", " +
				boost::lexical_cast<std::string>(message.getSHIPLENGTH())+ ", " +
				boost::lexical_cast<std::string>(message.getSHIPWIDTH())+ ", " +
				sanitize(boost::lexical_cast<std::string>(message.getDESTINATION()))+ "', '" +
				boost::lexical_cast<std::string>(message.getETA())+ ", " +
				boost::lexical_cast<std::string>(message.getPOSFIXTYPE())+ "')";
				/*boost::lexical_cast<std::string>(message.getCOG())+ ", " +
				boost::lexical_cast<std::string>(message.getSOG())+ ", " +
				boost::lexical_cast<std::string>(message.getTRUE_HEADING())+ ", " +
				boost::lexical_cast<std::string>(message.getPOSACCURACY())+ ", " +
				
				boost::lexical_cast<std::string>(message.getNAVSTATUS())+ ", " +
				boost::lexical_cast<std::string>(message.getROT())+ "')";*/

				
			//cout << m_sqlStatement << endl;
			if(m_currentIteration++ == m_iterations || m_iterations <= 0)
			{
				m_currentIteration = 1;

				//aisDebug("executing multirow insert start");
				StatementExecutor statementExecutor(m_sqlStatement);
				m_con->perform(statementExecutor);
				m_sqlStatement = string("");
				//aisDebug("executing multirow insert end");
			}
			return true;

		}
		catch(const exception &e)
		{
			cerr << "Error on Iteration: " << m_currentIteration << endl;
			cerr << "PostgreSQL Error : " << e.what() << endl;
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
			std::string connection_string = "user=" + m_username + " password=" + m_password + " dbname=" + m_databaseName + " hostaddr=" + m_hostname;
			m_con = std::shared_ptr<pqxx::connection>(new pqxx::connection(connection_string));
			
			if(!m_con){
				throw std::runtime_error("Could not create PostgreSql connection");
			}

			try
			{
				//m_con->prepare("insert_message", "INSERT INTO " + m_tableName + " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
				
				
				//m_sqlPreparedStatement = m_con->prepareStatement("INSERT INTO " + m_tableName + " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
				//if(!m_sqlPreparedStatement){
				//	throw std::runtime_error("Could not create Mysql sql statement");
				//}
				//aisDebug("Query prepared successfully...");
			}
			catch (const exception &e)
			{      
				std::cerr << "Statement Creation Error : " << e.what() << std::endl;
				return false;
			} 


		}catch (std::exception &e){
			std::cerr << "Exception: " << e.what() << std::endl;
			return false;
		}

		aisDebug("Database initialized successfully");
		return true;

	}

	void print()
	{
		cout << "Username: " << m_username << endl;
		cout << "Password: " << m_password << endl;
		cout << "Hostname: " << m_hostname << endl;
		cout << "Database Name: " << m_databaseName<< endl;
		cout << "Table Name: " << m_tableName<< endl;
		cout << "Iterations: " << m_iterations<< endl;
		cout << "Current Iteration: " << m_currentIteration<< endl;
		cout << "Initialized: " << m_initialized<< endl;

		if(m_con)
		{
			cout << "Connection is not null" << endl;
		}
	}

	class StatementExecutor : public pqxx::transactor<>
	{
		const std::string* m_statement;
	public:
		StatementExecutor(const std::string& statement):
		pqxx::transactor<>("StatementExecutor"), m_statement(&statement)
		{}

		void operator()(argument_type &T)
		{
			T.exec(*m_statement);
		}
	};

	string m_username;
	string m_password;
	string m_hostname;
	string m_databaseName;
	string m_tableName;
	int m_iterations;
	int m_currentIteration;
	bool m_initialized;
	
	string m_sqlStatement;
	std::shared_ptr<pqxx::connection>  m_con;
};

#endif
