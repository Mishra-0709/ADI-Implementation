Vehicle Data Monitoring System

Overview

This project showcases a Vehicle Data Monitoring System with features like real-time display of speed, acceleration, temperature, and GPS coordinates. Additionally, it provides a visual representation of the vehicle's current location on Google Maps.

Features

Real-time data display:

Speed

Acceleration

Temperature

GPS Coordinates (Latitude and Longitude)

Integration with Google Maps to show vehicle location.

SOS button functionality for emergencies.

Lane capture and detection using the MAXFTHR78000's camera module.

Live data updates to the cloud with Thingspeak integration.

Web-based dashboard for authorized users to monitor data and receive alerts.

Technologies Used

Eclipse IDE: Used for programming the MAXFTHR78000 microcontroller.

MAXFTHR78000: A powerful development board for AI and embedded applications.

Camera Module: Captures lane images for lane detection functionality.

TFT Display: Outputs real-time vehicle data.

Arduino IDE: Used for sensory programming with the ESP32 Dev Module.

Thingspeak Cloud: Updates live sensor data to the cloud.

Enables real-time monitoring and alert generation.

Data is accessible via a secure, custom-built website.

Custom Website:

Displays live vehicle data.

Sends alerts and notifications based on significant data changes.

Provides SOS functionality for authorized users.

Google Maps API: Displays the vehicle's location on a map.

Setup Instructions

Hardware Setup:

Connect the MAXFTHR78000 board to your PC via USB.

Attach the OVM7692 camera module to the MAXFTHR78000 board for lane capture.

Ensure the TFT display is connected and powered on.

Connect the ESP32 Dev Module to sensors for data acquisition.

Software Setup:

Install Eclipse IDE and configure it for MAXFTHR78000 programming.

Install Arduino IDE and set up the ESP32 board support package.

Import the respective project files into Eclipse and Arduino IDE.

Ensure the required libraries and dependencies are included.

Cloud Integration:

Create a Thingspeak account and configure a channel for data upload.

Update the ESP32 code with your Thingspeak API key.

Web Dashboard Setup:

Host the custom website on a suitable platform.

Link it to the Thingspeak channel for live data updates.

Implement authentication for authorized user access.

Google Maps Integration:

Obtain an API key from the Google Cloud Console.

Update the API key in the source code where required.

Build and Run:

Compile the project in Eclipse IDE for MAXFTHR78000 programming.

Flash the sensory programming code onto the ESP32 using Arduino IDE.

Flash the MAXFTHR78000 program onto the board.

Open a terminal on the PC to view console logs.

Lane Detection Feature

The lane detection functionality is powered by the MAXFTHR78000's integrated AI capabilities.

The OVM7692 camera module captures live images of the road.

These images are processed to detect lane boundaries in real time.

Cloud and Web Features

Thingspeak Cloud:

Collects and logs live data from ESP32 sensors.

Updates data at regular intervals for seamless monitoring.

Custom Website:

Allows authorized users to access live data.

Provides alerts and warnings for drastic changes in parameters.

SOS functionality enables users to call for help in emergencies.

SOS Functionality

The SOS button can be pressed to trigger an emergency response.

It highlights the vehicle's current location on the map and sends an alert.

Notifications are accessible through the custom-built website.

Future Enhancements

Integration with additional cloud storage for logging historical data.

Adding machine learning models for advanced driver assistance systems (ADAS).

Support for more detailed analytics like fuel efficiency and engine diagnostics.

References

Maxim Integrated AI Documentation: GitHub Repository

Eclipse IDE User Guide: Eclipse Documentation

Thingspeak Cloud Documentation: Thingspeak Guide

For more details, refer to the project's presentation or contact the developer.
