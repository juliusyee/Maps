#include "provided.h"
#include "support.h"
#include <string>
#include <vector>
#include <queue>
using namespace std;

//Implementation of the Struct Node
struct Node
{
	Node(){}
	Node(StreetSegment s, double g, double h)
	{
		s_segment = s;
		gCost = g;
		hCost = h;
		F = gCost + hCost;
		parent = nullptr;
	}
	Node* parent;
	StreetSegment s_segment;
	double gCost;
	double hCost;
	double F;
	bool operator == (const Node &other)
	{
		if (s_segment.segment == other.s_segment.segment)
		{
			return true;
		}
	}
};

class NavigatorImpl
{
public:
	NavigatorImpl();
	~NavigatorImpl();
	bool loadMapData(string mapFile);
	NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
	MapLoader m_mapLoader;
	AttractionMapper m_attractionMapper;
	SegmentMapper m_segmentMapper;
	bool isOnClosedList(Node n, vector<Node> v) const;
};

bool NavigatorImpl::isOnClosedList(Node n, vector <Node> v) const
{
	vector<Node>::iterator itr = v.begin();
	while (itr != v.end())
	{
		if ((*itr) == n)
		{
			return true;
		}
		itr++;
	}
	return false;
}

struct compare
{
	bool operator()(const Node &a, const Node &b)
	{
		return (a.F < b.F);
	}
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
	m_mapLoader.load(mapFile);
	//Load the map
	if (m_mapLoader.load(mapFile))
	{
		//Initialize the AttractionMapper object
		m_attractionMapper.init(m_mapLoader);

		//Initialize the SegmentMapper object
		m_segmentMapper.init(m_mapLoader);
		return true;
	}
	else
	{
		//Map could not be loaded!
		return false;
	}
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	//List of nodes that need to be checked
	priority_queue<Node,vector<Node>,compare> open_list;
	//List of nodes that have been checked
	vector<Node> closed_list;

	//Determine the start and end coordinates of the route
	GeoCoord startPoint;
	GeoCoord endPoint;
	bool sourceAttractionLoaded = m_attractionMapper.getGeoCoord(start, startPoint);
	bool destAttractionLoaded = m_attractionMapper.getGeoCoord(end, endPoint);

	//Check for error in source or destination location
	if (!sourceAttractionLoaded)
	{
		return NAV_BAD_SOURCE;
	}
	if (!destAttractionLoaded)
	{
		return NAV_BAD_DESTINATION;
	}

	//Get StreetSegment of start location
	vector<StreetSegment> potentialSegments = m_segmentMapper.getSegments(startPoint);
	StreetSegment startSegment;
	if (!potentialSegments.empty())
	{
		startSegment = m_segmentMapper.getSegments(startPoint)[0];
	}

	//Get StreetSegment of end location
	vector<StreetSegment> potentialSegments2 = m_segmentMapper.getSegments(endPoint);
	StreetSegment endSegment;
	if (!potentialSegments.empty())
	{
		endSegment = m_segmentMapper.getSegments(endPoint)[0];
	}

	//Create a starting node
	double distFromStartEnd = distanceEarthMiles(startSegment.segment.start, endPoint);
	double distFromEndEnd = distanceEarthMiles(startSegment.segment.end, endPoint);
	double hCost = distanceEarthMiles(startPoint, endPoint);

	//if end end is closer to destination
	GeoCoord CoordToMatch;
	if (distFromStartEnd > distFromEndEnd)
	{
		CoordToMatch = startSegment.segment.end;
	}
	else
	{
		CoordToMatch = startSegment.segment.start;
	}
	//Create the starting Node and push onto open list
	Node startNode(startSegment, 0, hCost);
	open_list.push(startNode);
	
	//While there's still more Nodes to explore and have not reached destination
	while (!open_list.empty())
	{
		Node current = open_list.top(); //lowest f value Node
		//If the neighbor is the end StreetSegment, we're done!!
		if (current.s_segment.segment == endSegment.segment)
		{
			//Create the last NavSegment
			GeoSegment g(CoordToMatch, endPoint);
			double distance = distanceEarthMiles(CoordToMatch, endPoint);
			string streetName = current.s_segment.streetName;
			string direction = getAngleString(angleOfLine(g));
			NavSegment end(direction, streetName, distance, g);
			directions.insert(directions.begin(),end);

			GeoCoord lastCoord = CoordToMatch;
			GeoCoord currentEnd = endPoint;
			string lastStreet = streetName;
			//Follow backwards through the path and create NavSegments for each
			while (current.parent != nullptr)
			{
				current = *(current.parent);
				if (lastCoord == current.s_segment.segment.start)
				{
					lastCoord = current.s_segment.segment.end;
					currentEnd = current.s_segment.segment.start;
				}
				else
				{
					lastCoord = current.s_segment.segment.start;
					currentEnd = current.s_segment.segment.end;
				}
				//Turn!!
				if (current.s_segment.streetName != lastStreet)
				{
					double angle = angleOfLine(current.s_segment.segment);
					if (angle < 180)
					{
						NavSegment turn("left", lastStreet);
						directions.insert(directions.begin(), turn);
					}
					else
					{
						NavSegment turn("right", lastStreet);
						directions.insert(directions.begin(), turn);
					}
				}
				else // same street
				{
					GeoSegment g(lastCoord, currentEnd);
					double distance = distanceEarthMiles(lastCoord, currentEnd);
					string streetName = current.s_segment.streetName;
					string direction = getAngleString(angleOfLine(g));
					NavSegment end(direction, streetName, distance, g);
					directions.insert(directions.begin(), end);
				}
			}
			directions[0].m_geoSegment.start = startPoint;
			return (NAV_SUCCESS);
		}
		//Establish the new coordinate to find all connecting segments to current
		if (CoordToMatch == current.s_segment.segment.start)
		{
			CoordToMatch = current.s_segment.segment.end;
		}
		else
		{
			CoordToMatch = current.s_segment.segment.start;
		}

		open_list.pop();
		closed_list.push_back(current);
		//Obtain neighboring StreetSegments
		vector<StreetSegment> v = m_segmentMapper.getSegments(CoordToMatch);
		vector<StreetSegment>::iterator itr = v.begin();
		while (itr != v.end())
		{
			GeoCoord neighbor_end;
			if (CoordToMatch == (*itr).segment.start)
			{
				neighbor_end = (*itr).segment.end;
			}
			else
			{
				neighbor_end = (*itr).segment.start;
			}
			double neighbor_gCost = current.gCost + distanceEarthMiles(CoordToMatch, neighbor_end);
			double neighbor_hCost = distanceEarthMiles(neighbor_end, endPoint);
			Node neighbor((*itr),neighbor_gCost,neighbor_hCost);
			neighbor.parent = &current;
			//If neighbor node is not on closed list (has not been checked)
			if (!isOnClosedList(neighbor, closed_list))
			{
				open_list.push(neighbor);
			}
		}
	}
	return (NAV_NO_ROUTE);
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
	m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
	delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
	return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
	return m_impl->navigate(start, end, directions);
}
