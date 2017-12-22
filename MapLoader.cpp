#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	vector<StreetSegment> m_streets;
};

MapLoaderImpl::MapLoaderImpl()
{
	
}

MapLoaderImpl::~MapLoaderImpl()
{

}

bool MapLoaderImpl::load(string mapFile)
{
	ifstream infile(mapFile);
	//File failed to open
	if (!infile)
	{
		return false;
	}
	//Read through every line in the file
	string st_name, coordline;
	string delimiter = " ";
	string lat1, lon1, lat2, lon2;
	string attract, att, a_name, alat, alon;
	string st;
	//get street name
	getline(infile, st_name);
	do
	{
		StreetSegment s1;
		s1.streetName = st_name;
		//get coordinates
		getline(infile, coordline);
		//get latitude of starting coordinate
		int pos = coordline.find(",");
		lat1 = coordline.substr(0, pos);
		coordline.erase(0, pos + delimiter.length()+1);
		//get longitude of starting coordinate
		pos = coordline.find(delimiter);
		lon1 = coordline.substr(0, pos);
		coordline.erase(0, pos + delimiter.length());
		//get latitude of ending coordinate
		pos = coordline.find(",");
		lat2 = coordline.substr(0, pos);
		coordline.erase(0, pos + delimiter.length());
		//get latitude of ending coordinate
		lon2 = coordline.substr(0, string::npos);
		//Create geocoord and geosegment
		GeoCoord start_coord(lat1, lon1);
		GeoCoord end_coord(lat2, lon2);
		GeoSegment seg(start_coord, end_coord);
		s1.segment = seg;
		//get attraction
		getline(infile, attract);
		if (stoi(attract) != 0)
		{
			for (int i = 0; i < stoi(attract); i++)
			{
				getline(infile, att);
				//get name of attraction
				int pos1 = att.find("|");
				a_name = att.substr(0, pos1);
				att.erase(0, pos1 + delimiter.length());
				//get latitude of attraction
				pos1 = att.find(",");
				alat = att.substr(0, pos1);
				att.erase(0, pos1 + delimiter.length() + 1);
				//get longitude of attraction
				alon = att.substr(0, string::npos);
				Attraction a1;
				GeoCoord coordinate(alat, alon);
				a1.geocoordinates = coordinate;
				a1.name = a_name;
				s1.attractions.push_back(a1);
			}
		}
		//Add street segment to vector
		m_streets.push_back(s1);
		
	} while (getline(infile, st_name));
	return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return(m_streets.size());
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	//if out of bounds
	if ((segNum < 0) || (segNum > getNumSegments() - 1))
	{
		return false;
	}
	//If inbounds, place in seg reference parameter
	seg.streetName = m_streets[segNum].streetName;
	seg.segment = m_streets[segNum].segment;
	seg.attractions = m_streets[segNum].attractions;
	return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
	return m_impl->getSegment(segNum, seg);
}
