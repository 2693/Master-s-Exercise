#include "CoordTransform.h"

void UTMtoLL(const utm_t utmPos, latlon_t *llPos )
{
	double x = utmPos.east;
	double y = utmPos.north;
	int zone = utmPos.zoneNumber;
	char hemisphere = utmPos.hemisphere;

	double lambda0 = (-183.0 + (zone * 6.0)) / 180.0 * M_PI;
	x = x - 500000.0;
	x = x / UTMScaleFactor;
	y = y / UTMScaleFactor;

	double n = (C_WGS84_a - C_WGS84_b) / (C_WGS84_a + C_WGS84_b);
	double alpha_ = ((C_WGS84_a + C_WGS84_b) / 2.0) * (1 + (pow(n, 2.0) / 4.0) + (pow(n, 4.0) / 64.0));
	double y_ = y / alpha_;

	double beta_ = (3.0 * n / 2.0) + (-27.0 * pow(n, 3.0) / 32.0) + (269.0 * pow(n, 5.0) / 512.0);
	double gamma_ = (21.0 * pow(n, 2.0) / 16.0) + (-55.0 * pow(n, 4.0) / 32.0);
	double delta_ = (151.0 * pow(n, 3.0) / 96.0) + (-417.0 * pow(n, 5.0) / 128.0);
	double epsilon_ = (1097.0 * pow(n, 4.0) / 512.0);
	double phif = y_ + (beta_ * sin(2.0 * y_)) + (gamma_ * sin(4.0 * y_)) + (delta_ * sin(6.0 * y_)) + (epsilon_ * sin(8.0 * y_));
	double ep2 = (pow(C_WGS84_a, 2.0) - pow(C_WGS84_b, 2.0)) / (pow(C_WGS84_b, 2.0));
	double cf = cos(phif);
	double nuf2 = ep2 * pow(cf, 2.0);
	double Nf = pow(C_WGS84_a, 2.0) / (C_WGS84_b * sqrt(1.0 + nuf2));
	double NfPow = Nf;
	double tf = tan(phif);
	double tf2 = tf * tf;
	double tf4 = tf2 * tf2;

	double x1frac = 1.0 / (NfPow * cf);

	NfPow = NfPow * Nf;
	double x2frac = tf / (2.0 * NfPow);
	NfPow = NfPow * Nf;
	double x3frac = 1.0 / (6.0 * NfPow * cf);
	NfPow = NfPow * Nf;
	double x4frac = tf / (24.0 * NfPow);
	NfPow = NfPow * Nf;
	double x5frac = 1.0 / (120.0 * NfPow * cf);
	NfPow = NfPow * Nf;
	double x6frac = tf / (720.0 * NfPow);
	NfPow = NfPow * Nf;
	double x7frac = 1.0 / (5040.0 * NfPow * cf);
	NfPow = NfPow * Nf;
	double x8frac = tf / (40320.0 * NfPow);

	double x2poly = -1.0 - nuf2;
	double x3poly = -1.0 - 2 * tf2 - nuf2;
	double x4poly = 5.0 + 3.0 * tf2 + 6.0 * nuf2 - 6.0 * tf2 * nuf2 - 3.0 * (nuf2 * nuf2) - 9.0 * tf2 * (nuf2 * nuf2);
	double x5poly = 5.0 + 28.0 * tf2 + 24.0 * tf4 + 6.0 * nuf2 + 8.0 * tf2 * nuf2;
	double x6poly = -61.0 - 90.0 * tf2 - 45.0 * tf4 - 107.0 * nuf2 + 162.0 * tf2 * nuf2;
	double x7poly = -61.0 - 662.0 * tf2 - 1320.0 * tf4 - 720.0 * (tf4 * tf2);
	double x8poly = 1385.0 + 3633.0 * tf2 + 4095.0 * tf4 + 1575 * (tf4 * tf2);

	double lat = phif + x2frac * x2poly * (x * x) + x4frac * x4poly * pow(x, 4.0) + x6frac * x6poly * pow(x, 6.0) + x8frac * x8poly * pow(x, 8.0);
	double lon = lambda0 + x1frac * x + x3frac * x3poly * pow(x, 3.0) + x5frac * x5poly * pow(x, 5.0) + x7frac * x7poly * pow(x, 7.0);

	if (toupper(hemisphere) == 'S') {lat = (-1) * lat;}

	llPos->lat = lat * 180.0 / M_PI;
	llPos->lon = lon * 180.0 / M_PI; 
}

void LLtoUTM (const latlon_t llPos, utm_t *utmPos)
{  
	int zone;

	zone = (int) (((llPos.lon + 180.0)/6.0) + 1.0);
	if ( (llPos.lat >= 56.0) && (llPos.lat < 64.0) && (llPos.lon >= 3.0) && (llPos.lon < 12.0) )
	{
		zone = 32;
	}
	if ( (llPos.lat >= 72.0) && (llPos.lat < 84.0) )
	{
		if		( (llPos.lon >= 0.0) && (llPos.lon < 9.0) ) {zone = 31;}
		else if ( (llPos.lon >= 9.0) && (llPos.lon < 21.0) ) {zone = 33;}
		else if ( (llPos.lon >= 21.0) && (llPos.lon < 33.0) ) {zone = 35;}
		else if ( (llPos.lon >= 33.0) && (llPos.lon < 42.0) ) {zone = 37;}
	}

	double phi = llPos.lat / 180 * M_PI;
	double lambda = llPos.lon / 180 * M_PI;
	double lambda0 = (-183.0 + (zone * 6.0)) / 180.0 * M_PI;

	/* Precalculate ep2 */
	double ep2 = (pow(C_WGS84_a, 2.0) - pow(C_WGS84_b, 2.0)) / pow(C_WGS84_b, 2.0);

	/* Precalculate nu2 */
	double nu2 = ep2 * pow(cos(phi), 2.0);

	/* Precalculate N */
	double N = pow(C_WGS84_a, 2.0) / (C_WGS84_b * sqrt(1.0 + nu2));

	/* Precalculate t */
	double t = tan(phi);
	double t2 = t * t;
	double tmp = (t2 * t2 * t2) - pow(t, 6.0);

	/* Precalculate l */
	double l = lambda - lambda0;

	/* Precalculate coefficients for l**n in the equations below
	so a normal human being can read the expressions for easting
	and northing
	-- l**1 and l**2 have coefficients of 1.0 */
	double l3coef = 1.0 - t2 + nu2;
	double l4coef = 5.0 - t2 + 9 * nu2 + 4.0 * (nu2 * nu2);
	double l5coef = 5.0 - 18.0 * t2 + (t2 * t2) + 14.0 * nu2 - 58.0 * t2 * nu2;
	double l6coef = 61.0 - 58.0 * t2 + (t2 * t2) + 270.0 * nu2 - 330.0 * t2 * nu2;
	double l7coef = 61.0 - 479.0 * t2 + 179.0 * (t2 * t2) - (t2 * t2 * t2);
	double l8coef = 1385.0 - 3111.0 * t2 + 543.0 * (t2 * t2) - (t2 * t2 * t2);

	/* Calculate easting (x) */
	double xy0 = N * cos(phi) * l
		+ (N / 6.0 * pow(cos(phi), 3.0) * l3coef * pow(l, 3.0))
		+ (N / 120.0 * pow(cos(phi), 5.0) * l5coef * pow(l, 5.0))
		+ (N / 5040.0 * pow(cos(phi), 7.0) * l7coef * pow(l, 7.0));

	/* Calculate northing (y) */

	/* Precalculate n */
	double n = (C_WGS84_a - C_WGS84_b) / (C_WGS84_a + C_WGS84_b);

	/* Precalculate alpha */
	double alpha = ((C_WGS84_a + C_WGS84_b) / 2.0) * (1.0 + (pow(n, 2.0) / 4.0) + (pow(n, 4.0) / 64.0));

	/* Precalculate beta */
	double beta = (-3.0 * n / 2.0) + (9.0 * pow(n, 3.0) / 16.0) + (-3.0 * pow(n, 5.0) / 32.0);

	/* Precalculate gamma */
	double gamma = (15.0 * pow(n, 2.0) / 16.0) + (-15.0 * pow(n, 4.0) / 32.0);

	/* Precalculate delta */
	double delta = (-35.0 * pow(n, 3.0) / 48.0) + (105.0 * pow(n, 5.0) / 256.0);

	/* Precalculate epsilon */
	double epsilon = (315.0 * pow(n, 4.0) / 512.0);

	/* Now calculate the sum of the series and return */
	double ArcLengthOfMeridian = alpha
		* (phi + (beta * sin(2.0 * phi))
		+ (gamma * sin(4.0 * phi))
		+ (delta * sin(6.0 * phi))
		+ (epsilon * sin(8.0 * phi)));

	double xy1 = ArcLengthOfMeridian
		+ (t / 2.0 * N * pow(cos(phi), 2.0) * pow(l, 2.0))
		+ (t / 24.0 * N * pow(cos(phi), 4.0) * l4coef * pow(l, 4.0))
		+ (t / 720.0 * N * pow(cos(phi), 6.0) * l6coef * pow(l, 6.0))
		+ (t / 40320.0 * N * pow(cos(phi), 8.0) * l8coef * pow(l, 8.0));

	/* Adjust easting and northing for UTM system. */
	xy0 = xy0 * UTMScaleFactor + 500000.0;
	xy1 = xy1 * UTMScaleFactor;
	if (xy1 < 0.0) xy1 = xy1 + 10000000.0;

	utmPos->east = xy0;
	utmPos->north = xy1;
	utmPos->zoneNumber = zone;
	if (llPos.lat >= 0.0) { utmPos->hemisphere = 'N'; } else { utmPos->hemisphere = 'S'; }
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double deg) {
	return (deg * math_pi / 180);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts radians to decimal degrees             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double rad2deg(double rad) {
	return (rad * 180 / math_pi);
}