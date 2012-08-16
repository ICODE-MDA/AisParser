#ifndef AisPostgreSqlDatabaseWriter_h
#define AisPostgreSqlDatabaseWriter_h

#include <exception>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>

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
		m_work(static_cast<pqxx::work*>(0)),
		m_pipe(static_cast<pqxx::pipeline*>(0)),
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
					m_pipe->insert(m_sqlStatement);
				}
				m_pipe->complete();
				m_work->commit();
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

			if(m_currentIteration++ == m_iterations || m_iterations <= 0)
			{
				m_currentIteration = 1;
				m_pipe->insert(m_sqlStatement);
				m_sqlStatement = string("");
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
				m_work = std::shared_ptr<pqxx::work>(new pqxx::work(*m_con));
			}
			catch (const exception &e)
			{      
				std::cerr << "Work Creation Error : " << e.what() << std::endl;
				return false;
			} 

			try
			{
				m_pipe = std::shared_ptr<pqxx::pipeline>(new pqxx::pipeline(*m_work));
			}
			catch (const exception &e)
			{      
				std::cerr << "Pipeline Creation Error : " << e.what() << std::endl;
				return false;
			} 

			try
			{
				//Using prepared statements was ~ 1.8 times slower than using normal inserts,
				//and ~5.3 times slower than using multirow inserts
				//aisDebug("Preparing Query...");
				//const std::string sql = "INSERT INTO " + m_tableName + " VALUES(DEFAULT, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20, $21, $22, $23, $24, $25, $26)";
				//m_con->prepare("insert", sql);
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

	class StatementExecutor : public transactor<>
	{
		const std::string* m_statement;
	public:
		StatementExecutor(const std::string& statement):
		transactor<>("StatementExecutor"), m_statement(&statement)
		{}

		void operator()(argument_type &T)
		{
			T.exec(*m_statement);
		}
	};

	//class PreparedStatementExecutor : public transactor<>
	//{
	//	const AisMessage* m_message;
	//public:
	//	PreparedStatementExecutor(const AisMessage& message):
	//	transactor<>("PreparedStatementExecutor"), m_message(&message)
	//	{}

	//	void operator()(argument_type &T)
	//	{
	//		T.prepared("insert")(m_message->getMESSAGETYPE())(m_message->getMMSI())(m_message->getNAVSTATUS())
	//			(m_message->getROT())(m_message->getSOG())(m_message->getLON())(m_message->getLAT())(m_message->getCOG())
	//			(m_message->getTRUE_HEADING())(m_message->getDATETIME())(m_message->getIMO())(m_message->getVESSELNAME())
	//			(m_message->getVESSELTYPEINT())(m_message->getSHIPLENGTH())(m_message->getSHIPWIDTH())(m_message->getBOW())
	//			(m_message->getSTERN())(m_message->getPORT())(m_message->getSTARBOARD())(m_message->getDRAUGHT())
	//			(m_message->getDESTINATION())(m_message->getCALLSIGN())(m_message->getPOSACCURACY())(m_message->getETA())
	//			(m_message->getPOSFIXTYPE())(m_message->getSTREAMID()).exec();
	//	}
	//};

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
	std::shared_ptr<pqxx::work> m_work;
	std::shared_ptr<pqxx::pipeline> m_pipe;
};

#endif
