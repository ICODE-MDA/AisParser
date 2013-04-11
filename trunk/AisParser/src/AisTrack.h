#ifndef AisTrack_h
#define AisTrack_h

#include <string>
#include <vector>
#include <algorithm>

#include <AisMessage.h>
#include <AisCoordinate.h>

using namespace std;

class AisTrack{
public:
	AisTrack(const AisMessage &m)
	{
		//hasStaticInformation = false;
		m_mmsi = m.getMMSI();
		addData(m);
	}
	
	void addStaticMessageInfo(const AisMessage &m)
	{
		
		for(int i=0; i<m_messages.size(); i++)
		{
			if(m.equalWithExceptionOfTime(m_messages[i]))
			{
				return;
			}
		}

		//if it gets here it was not found in current track, so add it
		m_messages.push_back(m);
	}

	void addData(const AisMessage &message) 
	{
		if((message.getMESSAGETYPE() == 5) || (message.getMESSAGETYPE() == 24))
		{
			addStaticMessageInfo(message);
		}
		else if (message.getMESSAGETYPE() == 19) // Message type has dynamic and static data, Add static data only here
		{
			//cout << "Message type 19 MMSI " << fixed << message.getMMSI() << endl;
			addStaticMessageInfo(message);
		}
		else
		{
			m_aisCoords.push_back(AisCoordinate(message.getLAT(), message.getLON(), message.getDATETIME()));
		}
	}
	
	static bool dateCompare(const AisCoordinate& lhs, const AisCoordinate& rhs)
	{
		return lhs.m_timestamp < rhs.m_timestamp;
	}

	void sortByDateTime()
	{
		std::sort(m_aisCoords.begin(), m_aisCoords.end(), dateCompare);
	}

	double getMMSI() const{
		return m_mmsi;
	}

	double getLAT() const
	{
		if(m_aisCoords.size() > 0)
		{
			return m_aisCoords[m_aisCoords.size()-1].m_lat;
		}
		else
		{
			return -999;
		}
	}

	double getLON() const
	{
		if(m_aisCoords.size() > 0)
		{
			return m_aisCoords[m_aisCoords.size()-1].m_lon;
		}
		else
		{
			return -999;
		}
	}

	AisCoordinate& operator[] (unsigned int idx)
	{
		return m_aisCoords[idx];
	}

	unsigned int size()
	{
		return m_aisCoords.size();
	}

	std::vector<AisMessage> m_messages;
private:
	double m_mmsi;
	std::vector<AisCoordinate> m_aisCoords;
};

#endif
