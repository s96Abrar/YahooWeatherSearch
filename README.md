# Yahoo! Weather Search
Project Summary:  
A simple command line application that fetches weather data for various locations from the Yahoo Weather API
The user can make a plain text file with places that he usually checks the weather for. The program will check the weather for those places on startup and add them to a balanced binary tree.
When the program is running the user can do various operations on the data such as searching adding and deleting locations as well as checking for weather of a given location without adding it to the tree
# Dependencies
1.json-c  
Install by typing "sudo apt-get install libjson0 libjson0-dev" in terminal  
2.curl  
Install by typing "sudo apt-get install libcurl4-gnutls-dev" in terminal  

*Probably should do a "sudo apt-get update" before installing the libraries  
# System requirements
Linux and apt-get
# How to run
The applications should be fairly easy to run if you are running linux(Debian based) with the latest version of the g++ compiler installed.
All right so this is what you need to do:  
1. Get all the dependencies listed in the dependencies section.  
2. "g++ -o YahooWeatherSearch main.cpp WeatherTree.cpp -lcurl -l json -std=c++11", this should generate a file called YahooWeatherSearch that you can run using "./YahooWeatherSearch locations.txt"
# Open issues/bugs
Haven't found any but please do report them if you do
