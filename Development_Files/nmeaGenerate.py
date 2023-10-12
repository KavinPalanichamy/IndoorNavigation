from pynmeagps import NMEAMessage, GET

# Latitude, Longitude, and Altitude values
latitude = 43.123456789  # Replace with your latitude value
longitude = -2.987654321  # Replace with your longitude value
altitude = 100.0  # Replace with your altitude value

# Create an NMEAMessage (GPGGA) with the specified values
gpgga_msg = NMEAMessage('GP', 'GGA', GET, lat=latitude, NS='N', lon=longitude, EW='W', alt=altitude, hpnmeamode=1)

# Print the GPGGA message
print(gpgga_msg)
