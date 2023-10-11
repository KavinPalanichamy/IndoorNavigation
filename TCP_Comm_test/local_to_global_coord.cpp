#include <iostream>
#include <cmath>

// Constants for unit conversions and Earth's radius
const double DegreesToRadians = M_PI / 180;
const double RadiansToDegrees = 180/ M_PI;
const double EarthRadius = 6378160;  // Earth's radius in meters
const double TwoPi = 2.0 * M_PI;

// Structure to represent latitude, longitude, and altitude
struct LatLonAlt {
    double Latitude;
    double Longitude;
    double Altitude;
    
    LatLonAlt(double lat, double lon, double alt) : Latitude(lat), Longitude(lon), Altitude(alt) {}
};

// Function to calculate the end-point from a source with a given range and bearing
LatLonAlt CalculateDerivedPosition(LatLonAlt source, double range, double bearing) {
    double latA = source.Latitude * DegreesToRadians;
    double lonA = source.Longitude * DegreesToRadians;
    double angularDistance = range / EarthRadius;
    double trueCourse = bearing * DegreesToRadians;

    double lat = asin(sin(latA) * cos(angularDistance) + cos(latA) * sin(angularDistance) * cos(trueCourse));

    double dlon = atan2(sin(trueCourse) * sin(angularDistance) * cos(latA), cos(angularDistance) - sin(latA) * sin(lat));

    double lon = fmod(lonA + dlon + M_PI, TwoPi) - M_PI;

    return LatLonAlt(lat * RadiansToDegrees, lon * RadiansToDegrees, source.Altitude);
}

int main() {
    // Example usage
    LatLonAlt source(38.031389, 119.3254785, 0); // San Francisco coordinates
    double range = 2860.358458; // 1 kilometer
    double bearing = 0.00; // 45 degrees

    LatLonAlt destination = CalculateDerivedPosition(source, range, bearing);
    std::cout << "Destination: Latitude " << destination.Latitude << ", Longitude " << destination.Longitude << ", Altitude " << destination.Altitude << std::endl;

    return 0;
}
