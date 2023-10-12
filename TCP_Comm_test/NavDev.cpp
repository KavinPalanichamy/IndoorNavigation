#include <iostream>
#include <iomanip>
#include <cmath>

// Constants for unit conversions and Earth's radius


// Structure to represent latitude, longitude, and altitude
struct Geodetic_Coordinate {
    double Latitude;
    double Longitude;
    double Altitude;
    double bearing;

    Geodetic_Coordinate (double lat, double lon , double altitude, double angle ): Latitude(lat) , Longitude(lon), Altitude(altitude), bearing(angle){}
};

struct Local_Coordinate {

    double x_loc;
    double y_loc;
    double z_loc;
  

    Local_Coordinate(double x, double y, double z): x_loc(x) , y_loc(y),z_loc(z){}
};

// Function to calculate the end-point from a anchor_origin with a given displacement_from_origin and total_bearing


Geodetic_Coordinate local2geodetic(Geodetic_Coordinate anchor_origin, Local_Coordinate UV_local ) {
  
    double total_bearing;

    //Finding the total_bearing and the displacement of the mobile robot 
    double local_bearing = atan2(UV_local.x_loc,UV_local.y_loc);
    double displacement_from_origin = sqrt((UV_local.x_loc * UV_local.x_loc)+(UV_local.y_loc * UV_local.y_loc));
    total_bearing = local_bearing + anchor_origin.bearing;


    //Mathmetical constants 
    const double DegreesToRadians = M_PI / 180;
    const double RadiansToDegrees = 180/ M_PI;
    const double EarthRadius = 6378160;  // Earth's radius in meters
    const double TwoPi = 2.0 * M_PI;

    //Degrees to Radians 
    double latA = anchor_origin.Latitude * DegreesToRadians;
    double lonA = anchor_origin.Longitude * DegreesToRadians;
    double trueCourse = total_bearing * DegreesToRadians;

    double angularDistance = displacement_from_origin / EarthRadius;

    /*   ----- Haversine ( Inverse ) formula  --------
    
        y = arcsin((sin(y0) * cos(d/r)) + (cos(y0) * sin(d/r) * cos(theta)))
        x = x0 + arctan2((sin(theta) * sin(d/r) * cos(y0)), (cos(d/r) - (sin(y0) * sin(y))))
        where `x0` and `y0` are the initial coordinates, `d` is the distance traveled, `r` is the radius of the  Earth), 
        theta is the total_bearing, and `x` and `y` are the new coordinates  */


    double lat = asin(sin(latA) * cos(angularDistance) + cos(latA) * sin(angularDistance) * cos(trueCourse));

    double dlon = atan2(sin(trueCourse) * sin(angularDistance) * cos(latA), cos(angularDistance) - sin(latA) * sin(lat));

    double lon = fmod(lonA + dlon + M_PI, TwoPi) - M_PI;

    return Geodetic_Coordinate(lat * RadiansToDegrees, lon * RadiansToDegrees, anchor_origin.Altitude,total_bearing);
}

int main() {
    // Example usage
    Geodetic_Coordinate anchor_origin(38.031389, 119.354785, 0,0); 
    Local_Coordinate UV_local(150,4800,0);
    double displacement_from_origin = 2860.358458; 
    

    Geodetic_Coordinate destination = local2geodetic(anchor_origin, UV_local);

    // Set the precision for output
    std::cout << std::fixed << std::setprecision(9);

    std::cout << "Destination: Latitude " << destination.Latitude << ", Longitude " << destination.Longitude << ", Altitude " << destination.Altitude << std::endl;

    return 0;
}
