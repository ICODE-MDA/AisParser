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
	AisPostgreSqlDatabaseWriter(std::string username, std::string password, std::string hostname, std::string databaseName, std::string staticTableName, std::string dynamicTableName, int iterations = 100000):
	m_con(static_cast<pqxx::connection*>(0)),
		m_staticIteration(1),
		m_dynamicIteration(1),
		m_username(username),
		m_password(password),
		m_hostname(hostname),
		m_databaseName(databaseName),
		m_staticTableName(staticTableName),
		m_dynamicTableName(dynamicTableName),
		m_iterations(iterations),
		m_staticSQLStatement(""),
		m_dynamicSQLStatement("")
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
				aisDebug("trying to execute update any remaining static entries");

				if(m_staticSQLStatement != "")
				{
					//execute statement to add any remainign
					//aisDebug("executing multirow insert start");
					StatementExecutor statementExecutor(m_staticSQLStatement);
					m_con->perform(statementExecutor);
					m_staticSQLStatement = string("");
					//aisDebug("executing multirow insert end");
					//m_sqlPreparedStatement->executeUpdate();
				}
			}
			catch(const exception &e)
			{
				cerr << "Error on Iteration: " << m_staticIteration << endl;
				cerr << "Error : " << e.what() << endl;
			}

			try
			{
				aisDebug("trying to execute update any remaining dynamic entries");

				if(m_dynamicSQLStatement != "")
				{
					//execute statement to add any remainign
					//aisDebug("executing multirow insert start");
					StatementExecutor statementExecutor(m_dynamicSQLStatement);
					m_con->perform(statementExecutor);
					m_dynamicSQLStatement = string("");
					//aisDebug("executing multirow insert end");
					//m_sqlPreparedStatement->executeUpdate();
				}
			}
			catch(const exception &e)
			{
				cerr << "Error on Iteration: " << m_dynamicIteration << endl;
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

	//Main writeEntry function
	bool writeEntry(const AisMessage& message)
	{
		int message_type = message.getMESSAGETYPE();
		if (message_type == 5 || message_type == 24)	//Static message
		{
			//aisDebug("*** writing to static table ***");
			return writeStaticEntry(message);
		}
		else if (message_type == 1 || message_type == 2 || message_type == 3 || message_type == 4 || message_type == 18 || message_type == 19) //Dynamic message with location
		{
			//aisDebug("*** writing to dynamic table ***");
			return writeDynamicEntry(message);
		}
		else
		{
			aisDebug("Message type not handled");
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

	/**
	 Write dynamic entry 
	 */
	bool writeDynamicEntry(const AisMessage& message)
	{
		string version = "'TEST'";
		
		try
		{	
			if(m_dynamicIteration == 1 || m_iterations <= 0)
			{
				m_dynamicSQLStatement = "INSERT INTO " + m_dynamicTableName + " VALUES(DEFAULT, ";
			}
			else
			{
				m_dynamicSQLStatement+= ", (DEFAULT, ";
			}
			m_dynamicSQLStatement +=
					version + "," + 
					boost::lexical_cast<std::string>(message.getMESSAGETYPE()) + ", " +
					boost::lexical_cast<std::string>(message.getMMSI()) + ", " +
					boost::lexical_cast<std::string>(message.getNAVSTATUS()) + ", " + 
					boost::lexical_cast<std::string>(message.getROT()) + ", " + 
					boost::lexical_cast<std::string>(message.getSOG()) + ", " +
					boost::lexical_cast<std::string>(message.getPOSACCURACY()) + ", " +
					// Postgresql geography type 4326 (WGS projection ) is used for lat. and lon. which cover large areas
					"ST_SetSRID(ST_Point(" +
					  boost::lexical_cast<std::string>(message.getLON()) + ", " +
					  boost::lexical_cast<std::string>(message.getLAT()) + "),4326)::geography, " +
					boost::lexical_cast<std::string>(message.getCOG()) + ", " +
					boost::lexical_cast<std::string>(message.getTRUE_HEADING()) + ", " +
					"to_timestamp(" + boost::lexical_cast<std::string>(message.getDATETIME()) + "), " +
					"'" + sanitize(boost::lexical_cast<std::string>(message.getSTREAMID()))+ "')";
					
			//cout << m_dynamicSQLStatement << endl;
			if(m_dynamicIteration++ == m_iterations || m_iterations <= 0)
			{
				m_dynamicIteration = 1;

				//aisDebug("executing multirow insert start");
				StatementExecutor statementExecutor(m_dynamicSQLStatement);
				m_con->perform(statementExecutor);
				m_dynamicSQLStatement = string("");
				//aisDebug("executing multirow insert end");
			}
			return true;
		}
		catch(const exception &e)
		{
			cerr << "Error on Iteration: " << m_dynamicIteration << endl;
			cerr << "PostgreSQL Error : " << e.what() << endl;
			cerr << m_dynamicSQLStatement << endl << endl;
			return false;
		}
	}

	/**
	 Write static entry
	 */
	bool writeStaticEntry(const AisMessage& message)
	{
		string version = "'TEST'";
		string mmsi, imo, callsign, vesselname, unique_ID;
		try
		{	
			//Generate unique ID using the 4 fields
			mmsi = boost::lexical_cast<std::string>(message.getMMSI());
			imo = boost::lexical_cast<std::string>(message.getIMO());;
			callsign = sanitize(boost::lexical_cast<std::string>(message.getCALLSIGN()));
			vesselname = sanitize(boost::lexical_cast<std::string>(message.getVESSELNAME()));
			unique_ID = genUniqueID(mmsi, imo, callsign, vesselname);

			//Check if current unique ID exists in the table already
			string m_query = "SELECT ais_static_id, extract(epoch from latest_timestamp) as latest_timestamp,";
			m_query +=	"message_type, mmsi, imo, callsign, vessel_name, vessel_type, antenna_position_bow, ";
			m_query += "antenna_position_stern, antenna_position_port, antenna_position_starboard, length, width, draught, ";
			m_query += "destination, extract(epoch from eta) as eta, epfd FROM " + m_staticTableName + " WHERE UNIQUE_ID = '" + unique_ID + "'";
			
			//Try connecting using non-transaction method in order to obtain query results
			std::string connection_string = "user=" + m_username + " password=" + m_password + " dbname=" + m_databaseName + " hostaddr=" + m_hostname;
			pqxx::connection c(connection_string);
			pqxx::work w(c);
			pqxx::result r = w.exec(m_query);
			//cout << "RESULT IS: " << r.size() << endl;	//result size should only be 1 row if below is implemented completely

			if (r.size() == 0)		// No existing unique ID, check for special cases, otherwise simply push new entry
			{
				//Check for special cases

				//Push new entry
				return addNewStaticEntry(message, version, mmsi, imo, callsign, vesselname, unique_ID);
			}
			else	// Unique ID exists in table, so do some matching
			{
				//Check for changes in existing entry with matching unique ID and entry occurs latter in time
				pqxx::tuple currentRow = r[0];
				if (atoi(currentRow["latest_timestamp"].c_str()) < message.getDATETIME())
				{
					
					return checkStaticChanges(message, version, mmsi, imo, callsign, vesselname, unique_ID, r);
				}
			}
		}
		catch(const exception &e)
		{
			cerr << "Error on Iteration: " << m_staticIteration << endl;
			cerr << "PostgreSQL Error : " << e.what() << endl;
			return false;
		}
	}

	/**
	 Add a new static entry
	 */
	bool addNewStaticEntry(const AisMessage& message, string version, string mmsi, string imo, string callsign, string vesselname, string unique_ID)
	{
		//aisDebug("Unique ID does not exist, pushing new static vessel row");

		//Check iteration number
		if(m_staticIteration == 1 || m_iterations <= 0)
		{
			m_staticSQLStatement = "INSERT INTO " + m_staticTableName + " VALUES(DEFAULT, ";
		}
		else
		{
			m_staticSQLStatement+= ", (DEFAULT, ";
		}

		//Build the static SQL statement
		m_staticSQLStatement +=
				version + "," +
				"'" + sanitize(unique_ID) + "'," +
				"to_timestamp(" + boost::lexical_cast<std::string>(message.getDATETIME()) + "), " +
				"to_timestamp(" + boost::lexical_cast<std::string>(message.getDATETIME()) + "), " +
				boost::lexical_cast<std::string>(message.getMESSAGETYPE()) + ", " +
				mmsi + ", " +
				imo + ", " +
				"'" + callsign + "', " + 
				"'" + vesselname + "', " +
				boost::lexical_cast<std::string>(message.getVESSELTYPEINT()) + ", " +
				boost::lexical_cast<std::string>(message.getBOW()) + ", " +
				boost::lexical_cast<std::string>(message.getPORT()) + ", " +
				boost::lexical_cast<std::string>(message.getSTARBOARD()) + ", " +
				boost::lexical_cast<std::string>(message.getSTERN()) + ", " +
				boost::lexical_cast<std::string>(message.getSHIPLENGTH()) + ", " +
				boost::lexical_cast<std::string>(message.getSHIPWIDTH()) + ", " +
				boost::lexical_cast<std::string>(message.getDRAUGHT()) + ", " +
				"'" + sanitize(boost::lexical_cast<std::string>(message.getDESTINATION())) + "', " +
				"to_timestamp(" + boost::lexical_cast<std::string>(message.getETA()) + "), " +
				boost::lexical_cast<std::string>(message.getPOSFIXTYPE()) + ", " + 
				"'" + sanitize(boost::lexical_cast<std::string>(message.getSTREAMID())) + "')";

		//cout <<m_staticSQLStatement << endl;
		if(m_staticIteration++ == m_iterations || m_iterations <= 0)
		{
			m_staticIteration = 1;	//Reset iteration number

			//aisDebug("executing multirow insert start");
			StatementExecutor statementExecutor(m_staticSQLStatement);
			m_con->perform(statementExecutor);	//execute the statement
			m_staticSQLStatement = string("");	//reset the statement
			//aisDebug("executing multirow insert end");
		}
		return true;
	}

	/**
	 Check for changes in the static entry with matching unique ID
	 */
	bool checkStaticChanges(const AisMessage& message, string version, string mmsi, string imo, string callsign, string vesselname, string unique_ID, pqxx::result r)
	{
		aisDebug("Unique ID exists, need to check for changes");

		pqxx::tuple row = r[0];		//extract a single row of the result

		//Do the check for differences

		/*	CAN'T CHECK FOR NAME, CALLSIGN, IMO, OR MMSI CHANGES BECAUSE THIS WILL CREATE A DIFFERENT UNIQUE ID THAN THE EXISTING ONE
		string old_vessel_name = row["vessel_name"].c_str();
		if (old_vessel_name != message.getVESSELNAME())
		{
		aisDebug("Vessel name has changed: \"" << message.getVESSELNAME() << "\" --> \"" << row["vessel_name"] << "\"");
		//Do change update and push to change table
		}
		*/

		if (atoi(row["vessel_type"].c_str()) != message.getVESSELTYPEINT())
		{
			aisDebug("Vessel type has changed: \"" << message.getVESSELTYPEINT() << "\" --> \"" << row["vessel_type"] << "\"");
			//Do change update and push to change table
			addNewChangeEntry(row, message, "Vessel_Type", 
				boost::lexical_cast<std::string>(message.getVESSELTYPEINT()),unique_ID);
			UpdateStaticEntry(row, message, "Vessel_Type",unique_ID);

		}
		if (atoi(row["antenna_position_bow"].c_str()) != message.getBOW())
		{
			aisDebug("Vessel antenna position to bow changed");
			//Do change update and push to change table
			addNewChangeEntry(row, message, "Antenna_Position_Bow", 
				boost::lexical_cast<std::string>(message.getBOW()),unique_ID);
			UpdateStaticEntry(row, message, "Antenna_Position_Bow",unique_ID);
		}
		if (atoi(row["antenna_position_stern"].c_str()) != message.getSTERN())
		{
			aisDebug("Vessel antenna position to stern changed");
			//Do change update and push to change table
			addNewChangeEntry(row, message, "Antenna_Position_Stern", 
				boost::lexical_cast<std::string>(message.getSTERN()),unique_ID);
			UpdateStaticEntry(row, message, "Antenna_Position_Stern",unique_ID);
		}
		if (atoi(row["antenna_position_port"].c_str()) != message.getPORT())
		{
			aisDebug("Vessel antenna position to port changed");
			//Do change update and push to change table
			addNewChangeEntry(row, message, "Antenna_Position_Port", 
				boost::lexical_cast<std::string>(message.getPORT()),unique_ID);
			UpdateStaticEntry(row, message, "antenna_position_port",unique_ID);
		}
		if (atoi(row["antenna_position_starboard"].c_str()) != message.getSTARBOARD())
		{
			aisDebug("Vessel antenna position to starboard changed");
			//Do change update and push to change table
			addNewChangeEntry(row, message, "Antenna_Position_Starboard", 
				boost::lexical_cast<std::string>(message.getSTARBOARD()),unique_ID);
			UpdateStaticEntry(row, message, "antenna_position_starboard",unique_ID);
		}
		if (atoi(row["length"].c_str()) != message.getSHIPLENGTH())
		{
			aisDebug("Vessel length changed");
			//Do change update and push to change table
			addNewChangeEntry(row, message, "Length", 
				boost::lexical_cast<std::string>(message.getSHIPLENGTH()),unique_ID);
			UpdateStaticEntry(row, message, "length",unique_ID);
		}
		if (atoi(row["width"].c_str()) != message.getSHIPWIDTH())
		{
			aisDebug("Vessel width changed");
			//Do change update and push to change table
			addNewChangeEntry(row, message, "Width", 
				boost::lexical_cast<std::string>(message.getSHIPWIDTH()),unique_ID);
			UpdateStaticEntry(row, message, "width",unique_ID);
		}
		if (atoi(row["draught"].c_str()) != message.getDRAUGHT())
		{
			aisDebug("Vessel draught changed");
			//Do change update and push to change table
			addNewChangeEntry(row, message, "Draught", 
				boost::lexical_cast<std::string>(message.getDRAUGHT()),unique_ID);
			UpdateStaticEntry(row, message, "draught",unique_ID);
		}
		string old_destination = row["destination"].c_str();
		if (old_destination != message.getDESTINATION())
		{
			aisDebug("Vessel destination changed");
			//Do change update and push to change table
			addNewChangeEntry(row, message, "Destination", 
				boost::lexical_cast<std::string>(message.getDESTINATION()),unique_ID);
			UpdateStaticEntry(row, message, "destination",unique_ID);
		}
		if (atoi(row["eta"].c_str()) != message.getETA())
		{
			aisDebug("Vessel ETA has changed: \"" << message.getETA() << "\" --> \"" << row["eta"] << "\"");
			//Do change update and push to change table
			addNewChangeEntry(row, message, "Eta", 
				boost::lexical_cast<std::string>(message.getETA()),unique_ID);
			UpdateStaticEntry(row, message, "eta",unique_ID);
		}
		/*
		if (atoi(row["epfd"].c_str()) != message.getEPFD())
		{
		aisDebug("Vessel EPFD changed");
		//Do change update and push to change table
		}
		*/
	}
	// Add new entries to AIS_Change table for existing unique ID when the static messages change
	bool addNewChangeEntry(pqxx::tuple row, const AisMessage& message, string tag_name, string new_value,
		string unique_ID)
		
	{
		string version = "'TEST'";
		string CHANGE_TABLE_NAME = "ais_change";
		
		if(m_changeIteration == 1 || m_iterations <= 0)
		{
			m_changeSQLStatement = "INSERT INTO " + CHANGE_TABLE_NAME + " VALUES(DEFAULT, ";
		}
		else
		{
			m_staticSQLStatement+= ", (DEFAULT, ";
		}

		//Build the change SQL statement
		m_changeSQLStatement += boost::lexical_cast<std::string>(row["ais_static_id"].c_str()) + "," +
				version + "," +
				"'" + sanitize(unique_ID) + "'," +
				"to_timestamp(" + boost::lexical_cast<std::string>(message.getDATETIME()) + "), " +
				"'" + tag_name + "'," ;
		if (tag_name == "Destination")
		{
			m_changeSQLStatement += "'" + boost::lexical_cast<std::string>(row[tag_name].c_str()) + "'," +
			"'" + sanitize(new_value) + "')";
		} else if  (tag_name == "Eta")
		{
			m_changeSQLStatement += "to_timestamp(" + boost::lexical_cast<std::string>(row[tag_name].c_str()) + ")," +
			"to_timestamp(" + new_value + "))";
		} else
		{
			m_changeSQLStatement += boost::lexical_cast<std::string>(row[tag_name].c_str()) + "," +
			sanitize(new_value) + ")";
		}
		//cout << "changeIteration " << m_changeSQLStatement << endl;
		if(m_changeIteration++ == m_iterations || m_iterations <= 0)
		{
			m_changeIteration = 1;	//Reset iteration number

			//aisDebug("executing multirow insert start");
			StatementExecutor statementExecutor(m_changeSQLStatement);
			m_con->perform(statementExecutor);	//execute the statement
			m_changeSQLStatement = string("");	//reset the statement
			//aisDebug("executing multirow insert end");
		}
		return true;
	}
	bool UpdateStaticEntry(pqxx::tuple row, const AisMessage& message, string tag_name, string unique_ID)
	{
		string version = "'TEST'";
		
		m_changeSQLStatement = "UPDATE " + m_staticTableName + " SET ";
		

		//Build the update SQL statement
		m_changeSQLStatement += tag_name + " = ";
		if (tag_name == "Destination")
		{
			m_changeSQLStatement += "'" + sanitize(boost::lexical_cast<std::string>(row[tag_name].c_str())) + "'";
			
		} else if  (tag_name == "Eta")
		{
			m_changeSQLStatement += "to_timestamp(" + boost::lexical_cast<std::string>(row[tag_name].c_str()) + ")";

		} else
		{
			m_changeSQLStatement +=  boost::lexical_cast<std::string>(row[tag_name].c_str());
		}
		m_changeSQLStatement += ", Latest_Timestamp = ";
		m_changeSQLStatement += "to_timestamp(" + boost::lexical_cast<std::string>(message.getDATETIME()) + ") " ;

		m_changeSQLStatement += " WHERE AIS_Static_ID = "  + boost::lexical_cast<std::string>(row["ais_static_id"].c_str());
		cout << m_changeSQLStatement << endl;
		StatementExecutor statementExecutor(m_changeSQLStatement);
		m_con->perform(statementExecutor);	//execute the statement
		m_changeSQLStatement = string("");	//reset the statement
			
		return true;
	}
	/**
	Unique vessel ID generator
	*/
	string genUniqueID(string mmsi, string imo, string callsign, string vesselname)
	{
		int MAX_MMSI_LEN = 10;
		int MAX_IMO_LEN = 10;
		int MAX_CALL_SIGN_LEN = 7;
		int MAX_VESSEL_NAME_LEN = 20;

		string temp, uniqueID;

		temp = mmsi;
		boost::algorithm::trim(temp); // trim whitespace before and after string
		boost::algorithm::erase_all(temp, " "); // remove all whitespaces in string
		uniqueID = fillString(temp, MAX_MMSI_LEN); // insert fill characters up to maximum allowed string size

		temp = imo;
		boost::algorithm::trim(temp);
		boost::algorithm::erase_all(temp, " "); // remove all whitespaces in string
		uniqueID += fillString(temp, MAX_IMO_LEN);

		temp = callsign;
		boost::algorithm::trim(temp);
		boost::algorithm::erase_all(temp, " "); // remove all whitespaces in string
		uniqueID += fillString(temp, MAX_CALL_SIGN_LEN);

		temp = vesselname;
		boost::algorithm::trim(temp);
		boost::algorithm::erase_all(temp, " "); // remove all whitespaces in string
		uniqueID += fillString(temp, MAX_VESSEL_NAME_LEN);

		return(uniqueID);
	}

	string fillString(string input, int MAX_LEN)
	{
		int fillLen;
		char DEFAULT_FILL = '|';
		string outputString;
		outputString.append(input);
		if (outputString.length() < MAX_LEN)
		{
			fillLen = MAX_LEN - outputString.length();
			outputString.append(fillLen, DEFAULT_FILL);
		} else if (outputString.length() > MAX_LEN)
		{
			cerr << "Error in fillString length check: " << outputString.length() << " > " << MAX_LEN  << " string = " << outputString << endl;
			exit(1);
		}
		return(outputString);
	}
	/*
	bool writeTargetEntry(const AisMessage& message)
	{
		string altitude ="0.0";
		string version = "'A'";
		string unique_ID = "1234";
		string AIS_Static_ID = "1";  // this needs to be ais_dynamic or MMSI ????
		try
		{	
			if(m_currentIteration == 1 || m_iterations <= 0)
			{
				//TODO: create targetTable name
				//m_sqlStatement = "INSERT INTO " + m_targetTableName + " VALUES(DEFAULT,";
			}
			else
			{
				m_sqlStatement+= ", (DEFAULT, ";
			}

			m_sqlStatement+= AIS_Static_ID + "," + version + "," +
				
				"to_timestamp(" +
				boost::lexical_cast<std::string>(message.getDATETIME())+ "), " + "'" +
				sanitize(boost::lexical_cast<std::string>(message.getSTREAMID())) + "'," +
				boost::lexical_cast<std::string>(message.getMESSAGETYPE()) + ", " +
				altitude + "," +
				"ST_SetSRID(ST_Point(" +
				boost::lexical_cast<std::string>(message.getLON())+ ", " +
				boost::lexical_cast<std::string>(message.getLAT())+ "),4326)::geography)";
				cout << m_sqlStatement << endl;
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
				boost::lexical_cast<std::string>(message.getCOG())+ ", " +
				boost::lexical_cast<std::string>(message.getSOG())+ ", " +
				boost::lexical_cast<std::string>(message.getTRUE_HEADING())+ ", " +
				boost::lexical_cast<std::string>(message.getPOSACCURACY())+ ", " +
				
				boost::lexical_cast<std::string>(message.getNAVSTATUS())+ ", " +
				boost::lexical_cast<std::string>(message.getROT())+ "')";

				
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
	*/

	void print()
	{
		cout << "Username: " << m_username << endl;
		cout << "Password: " << m_password << endl;
		cout << "Hostname: " << m_hostname << endl;
		cout << "Database Name: " << m_databaseName<< endl;
		cout << "Static Table Name: " << m_staticTableName<< endl;
		cout << "Dynamic Table Name: " << m_dynamicTableName<< endl;
		cout << "Iterations: " << m_iterations<< endl;
		cout << "Static Iteration: " << m_staticIteration<< endl;
		cout << "Dynamic Iteration: " << m_dynamicIteration<< endl;
		cout << "Initialized: " << m_initialized<< endl;

		if(m_con)
		{
			cout << "Connection is not null" << endl;
		}
	}

	class StatementExecutor : public pqxx::transactor<>
	{
	private:
		const std::string* m_statement;
	public:
		//Constructor
		StatementExecutor(const std::string& statement) : pqxx::transactor<>("StatementExecutor"), 
			m_statement(&statement)
		{
		}

		void operator()(argument_type &T)
		{
			T.exec(*m_statement);
		}
	};

	string m_username;
	string m_password;
	string m_hostname;
	string m_databaseName;
	string m_staticTableName;
	string m_dynamicTableName;
	int m_iterations;
	int m_staticIteration;
	int m_dynamicIteration;
	int m_changeIteration;
	bool m_initialized;
	
	string m_staticSQLStatement;
	string m_dynamicSQLStatement;
	string m_changeSQLStatement;
	std::shared_ptr<pqxx::connection>  m_con;
};

#endif
