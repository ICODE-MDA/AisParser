#ifndef AisTrackSet_h
#define AisTrackSet_h

#include <vector>

#include <AisMessage.h>
#include <AisTrack.h>

using namespace std;

class AisTrackSet{
public:
	AisTrackSet()
	{}

	~AisTrackSet(){}

	void add(const AisMessage& m)
	{
		//check if any tracks mmmsi == m.getMMSI();
		if(!alreadyHaveShip(m))
		{
			aisTracks.push_back(AisTrack(m));
		}
	}

	//assumes a height of 0
	//returns in units of meters squared
	//uses wgs84
	double distanceSquaredBetweenPoints(double lat1, double lon1, double lat2, double lon2)
	{
		//http://en.wikipedia.org/wiki/Geodetic_system WGS84
		double theEccentricitySquared = .00669437999014;
		double theA = 6378137.0;// meters
		double radiansPerDegree = 3.141592653589793238462643/180;

		double sin_latitude1 = std::sin(lat1*radiansPerDegree);
		double cos_latitude1 = std::cos(lat1*radiansPerDegree);
		double N1 = theA / sqrt( 1.0 - theEccentricitySquared*sin_latitude1*sin_latitude1);
		double x1 = (N1)*cos_latitude1*std::cos(lon1*radiansPerDegree);
		double y1 = (N1)*cos_latitude1*std::sin(lon1*radiansPerDegree);
		double z1 = (N1*(1-theEccentricitySquared))*sin_latitude1;


		double sin_latitude2 = std::sin(lat2*radiansPerDegree);
		double cos_latitude2 = std::cos(lat2*radiansPerDegree);
		double N = theA / sqrt( 1.0 - theEccentricitySquared*sin_latitude2*sin_latitude2);
		double x2 = (N)*cos_latitude2*std::cos(lon2*radiansPerDegree);
		double y2 = (N)*cos_latitude2*std::sin(lon2*radiansPerDegree);
		double z2 = (N*(1-theEccentricitySquared))*sin_latitude2;

		double magSquared = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2);
		return magSquared;
	}

	bool messageBelongsToCurrentTrack(const AisMessage& m, std::vector<AisTrack>::iterator t)
	{
		if(m.getMMSI() == t->getMMSI())
		{
			if(t->getLAT() != -999 && m.getLAT() != -999 )//&& t->getLON() != -999 && m.getLON() != -999
			{
				//check if distance from the last point added to the track is small. This would imply they come from the same track
				//If the distance is far away, this means it doesn't come from the same track.
				//TODO: The points in the track aren't necessarily given by timestamp (there may be points in the track that are close enough,
				//to think that it is the same track, but the one that we check tells it to start a new track because it is too far away.
				double magSquared = distanceSquaredBetweenPoints(t->getLAT(), t->getLON(), m.getLAT(), m.getLON());
				if(magSquared > 1000000000000) //if distance^2 between points is > (1000km)^2 == if distance > 1000 km
				{
					return false;
				}
			}

			return true;
		}
		
		return false;
	}

	bool alreadyHaveShip(const AisMessage& message)
	{

		std::vector<AisTrack>::iterator itr = aisTracks.begin();

		while(itr != aisTracks.end()){
			if(messageBelongsToCurrentTrack(message, itr))
			{
				itr->addData(message);
				return true;
			}
			itr++;
		}
		return false;
	}

	unsigned int size()
	{
		return aisTracks.size();
	}

	AisTrack& operator[] (unsigned int idx)
	{
		return aisTracks[idx];
	}
private:
	std::vector<AisTrack> aisTracks;
};

#endif
