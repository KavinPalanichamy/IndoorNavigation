#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <math.h>
#include "dwm_api.h"
#include "hal.h"
#include <iomanip>
#include <cmath>
#include <ctime>
#include <fstream>

//------------------Structure Definitions and Initialization ------------

// Structure to represent Geodetic coordinate -->  latitude, longitude, and altitude (Degree,Degree,metres)
struct Geodetic_Coordinate {
    double Latitude;
    double Longitude;
    double Altitude;
    double bearing;

    Geodetic_Coordinate (double lat, double lon , double altitude, double angle ): Latitude(lat) , Longitude(lon), Altitude(altitude), bearing(angle){}
};

//Structure to represent local Coordinate ---> x_loc , y_loc , z_loc ( metres, metres, metres)
struct Local_Coordinate {

    double x_loc;
    double y_loc;
    double z_loc;
  

    Local_Coordinate(double x, double y, double z): x_loc(x) , y_loc(y),z_loc(z){}
};



// ---------------------------------------------------------------------------

std::string createNMEA_Message(Geodetic_Coordinate SendPosition) {

    //Systemtime

   std::time_t now = std::time(nullptr);
    struct tm* timeinfo = std::localtime(&now);
    int hours = timeinfo->tm_hour;
    int minutes = timeinfo->tm_min;
    int seconds = timeinfo->tm_sec;
    int tenths = (std::clock() * 10 / CLOCKS_PER_SEC) % 10;
    std::string formatted_time = 
        (hours < 10 ? "0" : "") + std::to_string(hours) +
        (minutes < 10 ? "0" : "") + std::to_string(minutes) +
        (seconds < 10 ? "0" : "") + std::to_string(seconds) + "."+
        (tenths< 10 ? "0" : "") + std::to_string(tenths);
    std::string time = formatted_time;

    //Variable required for NMEA

    double latitude = SendPosition.Latitude;
    double longitude = SendPosition.Longitude;
    char latitudeDirection = 'N';
    char longitudeDirection = 'E';
    int fixQuality = 2;
    int numSatellites = 11;
    double hdop = 1.40;
    double altitude = 83.2;
    char altitudeUnit = 'M';
    double geoidSeparation = 34.4;
    char geoidUnit = 'M';
    double ageOfDGPS = 0.7;
    int referenceStationID = 0;

    

    // Create NMEA string

    std::ostringstream ggaString;
    ggaString << "$GNGGA," << time << ",";
    ggaString << std::fixed << std::setprecision(5) << latitude << "," << latitudeDirection << ",";
    ggaString << std::fixed << std::setprecision(5) << "0"<<longitude << "," << longitudeDirection << ",";
    ggaString << fixQuality << "," << numSatellites << "," << std::fixed << std::setprecision(2) << hdop << ",";
    ggaString << std::fixed << std::setprecision(1) << altitude << "," << altitudeUnit << ",";
    ggaString << std::fixed << std::setprecision(1) << geoidSeparation << "," << geoidUnit << ",";
    ggaString << std::fixed << std::setprecision(1) << ageOfDGPS << "," << std::setw(4) << std::setfill('0') << referenceStationID;

    // Calculate the NMEA checksum
    std::string ggaMessage = ggaString.str();
int checksum = 0;
bool skipChecksumCalculation = true; // Flag to skip checksum calculation until '$' is encountered

for (char c : ggaMessage) {
    if (c == '$') {
        skipChecksumCalculation = false;
        continue;
    }
    
    if (!skipChecksumCalculation) {
        checksum ^= c;
    }
}
    // Add the checksum and terminate the NMEA message
    ggaString << "*" << std::hex << std::setw(2) << std::setfill('0') << checksum;
    ggaString << "\r\n";

    return ggaString.str();
}


void writeToNotepad( const std::string& data) {
    const std::string& fileName = "/home/user/Documents/gnss_feed.txt";
    std::ofstream file(fileName, std::ios_base::app); // Open the file in append mode
    if (!file) {
        std::cerr << "Error opening the file!" << std::endl;
        return;
    }

    file << data << std::endl; // Writing with a newline character
    file.close();
}





/* !

* @function               local2geodetic
* @description            Converts local coordinates to global geodetic coordinates 
* @param                  Geodetic_Coordinate Structure, Local Coordinate structure 
* @result                 The new global Geodetic_coordinate representing the new position of the Mobile robot

*/
// Function to calculate the end-point from a anchor_origin with a given displacement_from_origin and total_bearing


Geodetic_Coordinate local2geodetic(Geodetic_Coordinate anchor_origin, Local_Coordinate UV_local ) {

    //Mathmetical constants 
    const double DegreesToRadians = M_PI / 180;
    const double RadiansToDegrees = 180/ M_PI;
    const double EarthRadius = 6378160;  // Earth's radius in meters
    const double TwoPi = 2.0 * M_PI;

  
    double total_bearing;

    //Finding the total_bearing and the displacement of the mobile robot 
    double local_bearing = atan2(UV_local.y_loc,UV_local.x_loc);
    double displacement_from_origin = sqrt((UV_local.x_loc * UV_local.x_loc)+(UV_local.y_loc * UV_local.y_loc));
    total_bearing = ((local_bearing)*RadiansToDegrees + anchor_origin.bearing+270);

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



/* !

* @function         tcp_connect
* @description      Pushes data from client side
* @param            Pointer to a the message character to be sent 
* @result           The message is pushed to the tcp socket

*/

int tcp_connect(const char* message) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6100);
    serverAddress.sin_addr.s_addr = inet_addr("192.168.15.145");

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error connecting to the server");
        close(clientSocket);
        return 1;
    }

    if (send(clientSocket, message, strlen(message), 0) == -1) {
        perror("Error sending data to the server");
        close(clientSocket);
        return 1;
    }

    close(clientSocket);

    return 0;
}

double type_converter(double input ) {
    // Extract digits before and after the decimal point
    int digits_before_decimal = static_cast<int>(input);
    double digits_after_decimal = input - digits_before_decimal;
    
    // Multiply digits before the decimal by 100 and add digits after the decimal
    double result = (digits_before_decimal * 100) + (digits_after_decimal * 60);
    
    

    return result;
}



int main(void) {

    //Init. Tag
    int wait_period = 1000;
    dwm_cfg_tag_t cfg_tag;
    dwm_cfg_t cfg_node;

    HAL_Print("dwm_init(): dev%d\n", HAL_DevNum());
    dwm_init();

    HAL_Print("Setting to tag: dev%d.\n", HAL_DevNum());
    cfg_tag.low_power_en = 0;
    cfg_tag.meas_mode = DWM_MEAS_MODE_TWR;
    cfg_tag.loc_engine_en = 1;
    cfg_tag.common.led_en = 1;
    cfg_tag.common.ble_en = 1;
    cfg_tag.common.uwb_mode = DWM_UWB_MODE_ACTIVE;
    cfg_tag.common.fw_update_en = 0;
    HAL_Print("dwm_cfg_tag_set(&cfg_tag): dev%d.\n", HAL_DevNum());
    dwm_cfg_tag_set(&cfg_tag);

    HAL_Print("Wait 2s for node to reset.\n");
    HAL_Delay(2000);
    dwm_cfg_get(&cfg_node);

    HAL_Print("Comparing set vs. get: dev%d.\n", HAL_DevNum());
    if ((cfg_tag.low_power_en != cfg_node.low_power_en)
        || (cfg_tag.meas_mode != cfg_node.meas_mode)
        || (cfg_tag.loc_engine_en != cfg_node.loc_engine_en)
        || (cfg_tag.common.led_en != cfg_node.common.led_en)
        || (cfg_tag.common.ble_en != cfg_node.common.ble_en)
        || (cfg_tag.common.uwb_mode != cfg_node.common.uwb_mode)
        || (cfg_tag.common.fw_update_en != cfg_node.common.fw_update_en)) {
        HAL_Print("low_power_en        cfg_tag=%d : cfg_node=%d\n", cfg_tag.low_power_en, cfg_node.low_power_en);
        HAL_Print("meas_mode           cfg_tag=%d : cfg_node=%d\n", cfg_tag.meas_mode, cfg_node.meas_mode);
        HAL_Print("loc_engine_en       cfg_tag=%d : cfg_node=%d\n", cfg_tag.loc_engine_en, cfg_node.loc_engine_en);
        HAL_Print("common.led_en       cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.led_en, cfg_node.common.led_en);
        HAL_Print("common.ble_en       cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.ble_en, cfg_node.common.ble_en);
        HAL_Print("common.uwb_mode     cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.uwb_mode, cfg_node.common.uwb_mode);
        HAL_Print("common.fw_update_en cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.fw_update_en, cfg_node.common.fw_update_en);
        HAL_Print("\nConfiguration failed.\n\n");
    } else {
        HAL_Print("\nConfiguration succeeded.\n\n");
    }
    //End of tag Initialization

    //Successively receive Location in regular intervals ( 1s )
    dwm_loc_data_t loc;
    dwm_pos_t pos;
    loc.p_pos = &pos;
    Local_Coordinate UV_local(0,77.30,0);
    while (1) {
        HAL_Print("Wait %d ms...\n", wait_period);
        HAL_Delay(wait_period);

        HAL_Print("dwm_loc_get(&loc):\n");

        if (dwm_loc_get(&loc) == RV_OK) {
            UV_local.x_loc = (loc.p_pos->x)/1000;
            UV_local.y_loc = (loc.p_pos->y)/1000;
            UV_local.z_loc=(loc.p_pos->z)/1000;
        }
    
    Geodetic_Coordinate anchor_origin(52.1937073, 20.9211908, 0,0); 
    Geodetic_Coordinate destination = local2geodetic(anchor_origin, UV_local);

    // Set the precision for output
    std::cout << std::fixed << std::setprecision(9);

    //std::cout << "Destination: Latitude " << destination.Latitude << ", Longitude " << destination.Longitude << ", Altitude " << destination.Altitude << std::endl;

    destination.Latitude = type_converter(destination.Latitude);
    destination.Longitude = type_converter(destination.Longitude);
   
    std::string send_NMEA_String = createNMEA_Message(destination);

    //Type conversion for sending via TCP
    char charArray[send_NMEA_String.length() + 1]; // +1 for the null-terminator
    strcpy(charArray, send_NMEA_String.c_str());
    std::cout<<charArray;
    writeToNotepad(send_NMEA_String);
    

    //tcp_connect(charArray);

    }
    }

