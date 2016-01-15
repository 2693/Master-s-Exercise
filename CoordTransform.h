#ifndef _COORDTRANSFORM_H_
#define _COORDTRANSFORM_H_

/* Conversion from LL to UTM and back*/

#define _USE_MATH_DEFINES

#include <cmath>
#include <ctype.h>

#define math_pi 3.14159265358979323846

// Datatype definitions
//----------------------

typedef struct {
	double lat;
	double lon;
}latlon_t;

typedef struct {
	int zoneNumber;
	char hemisphere;
	double east;
	double north;
}utm_t;

// fill inline definitions
//----------------------

inline latlon_t value2ll( double lat, double lon)
{
	latlon_t f;

	f.lat = lat;
	f.lon = lon;
	return f;
}

inline latlon_t array2ll( double pos[] )
{
	latlon_t f;

	f.lat = pos[0];
	f.lon = pos[1];
	return f;
}

inline utm_t value2utm( double east, double north, int zoneNumber, char hemisphere )
{
	utm_t f;
	f.east = east;
	f.north = north;
	f.zoneNumber = zoneNumber;
	f.hemisphere = hemisphere;

	return f;
}

inline utm_t array2utm( double pos[], char hemisphere )
{
	utm_t f;
	f.east = pos[0];
	f.north = pos[1];
	f.zoneNumber = (int) pos[2];
	f.hemisphere = hemisphere;

	return f;
}

// constants definition
//----------------------
const double UTMScaleFactor = 0.9996;
const double C_WGS84_a = 6378137.000;
const double C_WGS84_b = 6356752.31414;

// main functions
//----------------------
void UTMtoLL(const utm_t utmPos, latlon_t *llPos );
void LLtoUTM (const latlon_t llPos, utm_t *utmPos);

// helper functions
//----------------------
double deg2rad(double deg);
double rad2deg(double rad);

#endif /* _COORDTRANSFORM_ */