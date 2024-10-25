# Seamless Navigation: Integration of GPS and UWB-Based Indoor Positioning Systems for Enhanced Localization for Mobile Robots

**Contributors:**  
Kavin Palanichamy   
Arkadiusz Perski  
Rafal  
Marcin  

## Abstract
This project explores the fusion of Global Positioning System (GPS) and Ultra-Wideband (UWB) technology to improve indoor navigation. GPS lacks accuracy indoors due to signal obstructions, while UWB offers centimeter-level precision. By seamlessly transitioning between GPS and UWB, this system ensures continuous and accurate positioning for mobile robots across indoor and outdoor environments.

**Keywords:** Indoor Navigation, Ultra-Wide Band, Mobile Robotics

## Video demonstrating the noise free location data acquisition of the mobile robot using UWB beacons

https://github.com/user-attachments/assets/bf38b609-2ee9-4685-a4ea-dfb90e64ee9b

## 1. UWB-Based Indoor Positioning System

### 1.1 Introduction
UWB transmits high-frequency pulses, enabling precise real-time location tracking, even in environments with high signal interference. UWB anchors are strategically deployed for accurate triangulation, allowing for high-precision tracking within centimeter-level accuracy.

### 1.2 Programming UWB Modules by Qorvo
Qorvo's UWB modules come with firmware and an API, allowing flexible programming as tags or anchors:
- **Anchors**: Stationary, calculate tag positions by triangulating signals.
- **Tags**: Mobile, attached to objects requiring tracking.

### 1.3 Calculating the Local Coordinate
Using Qorvo's API, a program on a Raspberry Pi retrieves coordinates, converting signals into readable location data.

## 2. Network Structure
The core navigation software on a Raspberry Pi sends data in NMEA format to the robot. The robot then displays the exact position on a high-resolution map.
![125](https://github.com/user-attachments/assets/eed195a6-dddd-41a9-a969-39685a36e94e)

## 3. Converting Local to Global Coordinates

### 3.1 Map Display
Local coordinates are converted to global coordinates using the Haversine formula, facilitating accurate digital map plotting.

### 3.2 Bearing Calculations
The system calculates both local and total bearing for orientation adjustments:
- **BearingTotal**: Angle between Geographic North and robot's direction.
- **Displacement**: Euclidean distance from the origin.

## 4. Switching Modes between GNSS and UWB
The robot defaults to GNSS, switching to UWB when within a high-quality UWB anchor network, ensuring reliable localization.

## 5. Testing Setup
Testing was conducted in a corridor with limited GNSS and configured UWB coverage. Results showed the robot maintained stable UWB navigation indoors and smoothly transitioned to GNSS outdoors.

## 6. Results
The robot's trial locations displayed UWB's reliability indoors, while GNSS displayed scattered results due to signal interference. The system successfully switched to GNSS outdoors.

## References
1. [Qorvo UWB-based sensor link](https://www.qorvo.com/products/p/DWM1001-DEV)
2. [Haversine Formula](https://en.wikipedia.org/wiki/Haversine_formula)
