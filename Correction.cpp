#include "Correction.h"

//ratio between fov in pixels and meters
double FovRatio(TTeleData telemetry, int imgWidth, double FOVAngleRads)
{
	double rheight = telemetry.realHeight;
	double fovwhalf = rheight * tan (FOVAngleRads); //0.7068583470578;
	double fovw = 2 * fovwhalf;
	
	return fovw/imgWidth; // [m]/[px]
}

//provide coord of a pixel(x,y) and get UTM values
utm_t pixelToUTM(TTeleData data1, cv::Point pt, cv::Size imgSize, double FOVAngleRads)
{
	//Extract latitude and longitude information for central pixel
	latlon_t latlonC;
	latlonC.lat = data1.latitude;
	latlonC.lon = data1.longitude;
	
	//UTM conversion of the centre
	utm_t utmC;
	LLtoUTM(latlonC, &utmC);

	//Getting dx,dy in pixels (for a chosen pixel)
	double dx = (imgSize.width/2) - pt.x;
	double dy = (imgSize.height/2) - pt.y;

	//Calculate FOV ratio meters per pixel ([m/px]).
	double fovRatio = FovRatio(data1, imgSize.width, FOVAngleRads);

	//Differences in meters between centre point (C) and selected point (vector, from centre).
	long double dE = dx*fovRatio; //[px] * [m/px] = [m] 
	long double dN = -(dy*fovRatio); //negative because of x,y to N,E space origin change.

	//applying rotation
	double yawRads = deg2rad(data1.yaw);
	double cs = cos(yawRads);
	double sn = sin(yawRads);

	double E = (dE * cs) - (dN * sn);
	double N = (dE * sn) + (dN * cs);
	
	//Calculation of the new UTM coord.
	utm_t point;

	//HACK: This holds true only for small variations (which happens to be the case).
	point.zoneNumber = utmC.zoneNumber;
	point.hemisphere = utmC.hemisphere;

	point.east = utmC.east + E;
	point.north = utmC.north + N;
	
	return point;
}

//two pixels are give as parameters and get the Euclidean distance
double distanceCalculate(int x1, int y1, int x2, int y2)
{
	int x = x1 - x2;
	int y = y1 - y2;

	double dist;
	
	dist = pow(x,2) + pow(y,2);			//calculating the euclidean distance

	dist = sqrt(dist);					//sqrt function in math.h

	return dist;
}

//given two cv::Points, do the same (euclidean distance).
double distanceCalculate(cv::Point pt1, cv::Point pt2){
	return distanceCalculate(pt1.x, pt1.y, pt2.x, pt2.y);
}

//checks if a coord lays within the screen
bool Exhausted(utm_t coord)
{
     //TODO: convert utm to pixels


     // Define values for true
	if (coord.north >= 0 && coord.north <= 640 && coord.east >=0 && coord.east <= 480)
     {
         return true;
     }
     // Otherwise false
     else 
     {
         return false;
     }
}

//calculate where the new search window is located
cv::Rect CorrectAll(cv::Rect winPrev, TTeleData curr, TTeleData prev, cv::Size imgSize, double FOVAngleRads)
{
	//FOV ratio [m/px]
	double ratio = FovRatio(curr, imgSize.width, FOVAngleRads);

	//UTM structures to convert previous and current Telemetry structures.
	utm_t utmprev;
	utm_t utmcurr;

	//Conversion:
	latlon_t llcurr;
	llcurr.lat = curr.latitude;
	llcurr.lon = curr.longitude;
	LLtoUTM(llcurr, &utmcurr);

	latlon_t llprev;
	llprev.lat = prev.latitude;
	llprev.lon = prev.longitude;
	LLtoUTM(llprev, &utmprev);

	//calling the correctXY function
	cv::Rect r1 = CorrectXY(winPrev, utmcurr, utmprev, ratio);

	//calling the correctZ function
	cv::Rect r2 = CorrectZ(r1, curr.realHeight, prev.realHeight);

	//calling the correctYaw function
	cv::Rect r3 = CorrectYaw(r2, curr.yaw, prev.yaw, imgSize);

	//stay inside the image
    cv::Rect corr;
	        
	corr.x = MAX(r3.x, 0);
    corr.y = MAX(r3.y, 0);
	corr.width = MIN(r3.width, imgSize.width - r3.x);
	corr.height = MIN(r3.height, imgSize.height - r3.y);
	
	return corr;
}


//calculate velocities
cv::Rect CorrectXY(cv::Rect rect, utm_t curr, utm_t prev, double fovRatio)
{
	double invFovRatio = (1/fovRatio);
	
	double N = curr.north - prev.north;
	double E = curr.east - prev.east;
	
	double dx =  E * invFovRatio;
	double dy = -N * invFovRatio; //negative because of cartesian to geo.
	
	cv::Rect corr;

	corr.x = rect.x + (int)dx;
	corr.y = rect.y + (int)dy;
	corr.width = rect.width;
	corr.height = rect.height;

	return corr;
}

//adjusting the bbox based on alt values
cv::Rect CorrectZ(cv::Rect rect, double currHeight, double prevHeight)
{
	
	double factor = prevHeight / currHeight;

	cv::Rect corr;
	
	//TODO: Change this, so that size change is with respect to the centre of rect.

	corr.x = rect.x;
	corr.y = rect.y;
	corr.height = (int)(rect.height * factor);
	corr.width = (int)(rect.width * factor);
	
	return corr;
}

//pixel only displacement of the bbox
cv::Rect CorrectYaw(cv::Rect rect, double currYaw, double prevYaw, cv::Size imgSize)
{
	double diffYaw = currYaw - prevYaw;
	
	//degrees to radians
	double diffYawRad = (diffYaw * math_pi) / 180.0;

	//Coordinates of the centre of the search window with ref to TopLeft corner of image
	int wcx = rect.x + (rect.width / 2);
	int wcy = rect.y + (rect.height / 2);

	//Coordinates of the centre of the search window with ref to centre of image
	int dcx = wcx - (imgSize.width / 2);
	int dcy = wcy - (imgSize.height / 2);

	//2D Euclidean vector rotations
	double cs = -cos(diffYawRad);
	double sn = -sin(diffYawRad);

	//Apply rotation to the vector by yaw [rad].
	double x = (dcx * cs) - (dcy * sn);
	double y = (dcx * sn) + (dcy * cs);

	//Coordinates of the TopLeft corner of search window with refer to centre of search window
	double tlx = (imgSize.width / 2) - x;
	double tly = (imgSize.height / 2) - y;
	
	cv::Rect corr;
	
	corr.x = (int)(tlx - rect.width/2);
	corr.y = (int)(tly - rect.height/2);
	corr.width = rect.width;
	corr.height = rect.height;

	return corr;
}
