#include "provided.h"
#include <string>
#include <iostream>
#include "MyMap.h"
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<string, GeoCoord> m;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
	for (size_t i = 0; i < ml.getNumSegments(); i++)
	{
		StreetSegment s;
		ml.getSegment(i, s);
		//If there are attractions on the street segment
		if(!s.attractions.empty())
		{
			vector<Attraction>::iterator itr = s.attractions.begin();
			while (itr != s.attractions.end())
			{
				m.associate((*itr).name,(*itr).geocoordinates);
				itr++;
			}
		}
	}
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	const GeoCoord* coord = m.find(attraction);
	if (coord != nullptr)
	{
		gc.latitude = (*coord).latitude;
		gc.latitudeText = (*coord).latitudeText;
		gc.longitude = (*coord).longitude;
		gc.longitudeText = (*coord).longitudeText;
		return true;
	}
	return false;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
