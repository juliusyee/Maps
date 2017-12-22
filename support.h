#ifndef SUPPORT_INCLUDED
#define SUPPORT_INCLUDED

#include "provided.h"
#include <string>
#include <iostream>
using namespace std;

//Custom Operator for == for GeoSegment structs
inline 
bool operator ==(const GeoSegment &a, const GeoSegment &b)
{
	if ((a.start.latitude == b.start.latitude) && (a.end.latitude == b.end.latitude) && (a.start.longitude == b.start.longitude) && (a.end.longitude == b.end.longitude))
	{
		return true;
	}
	return false;
}

//Custom Operator for == for GeoCoord objects
inline
bool operator==(const GeoCoord &a, const GeoCoord &b)
{
	if ((a.latitude == b.latitude) && (a.longitude == b.longitude))
	{
		return true;
	}
	return false;
}

inline
string getAngleString(double angle) 
{
	if ((angle > 0) && (angle <= 22.5))
	{
		return("east");
	}
	else if ((angle > 22.5) && (angle <= 67.5))
	{
		return("northeast");
	}
	else if ((angle > 67.5) && (angle <= 112.5))
	{
		return ("north");
	}
	else if ((angle > 112.5) && (angle <= 157.5))
	{
		return("northwest");
	}
	else if ((angle > 157.5) && (angle <= 202.5))
	{
		return("west");
	}
	else if ((angle > 202.5) && (angle <= 247.5))
	{
		return("southwest");
	}
	else if ((angle > 247.5) && (angle <= 292.5))
	{
		return("south");
	}
	else if ((angle > 292.5) && (angle <= 337.5))
	{
		return("southeast");
	}
	else if ((angle > 337.5) && (angle <= 67.5))
	{
		return("east");
	}
}

#endif
