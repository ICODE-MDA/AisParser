#ifndef AisCoordinate_h
#define AisCoordinate_h

class AisCoordinate
{
public:
	AisCoordinate()
	{
		m_lat = 0;
		m_lon = 0;
		m_timestamp = 0;
	}
	
	AisCoordinate(double lat, double lon, int timestamp ){
		m_lat = lat;
		m_lon = lon;
		m_timestamp = timestamp;
	}

	double m_lat;
	double m_lon;
	int m_timestamp;
};

#endif
