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
This version has been developed and tested on Ubuntu(Debian based linux) but it may run in other operating systems depending on the avalibilities of the required libraries
# How to run
The applications should be fairly easy to run if you are running linux(Debian based) with the latest version of the g++ compiler installed.
All right so this is what you need to do:  
1. Get all the dependencies listed in the dependencies section.  
2(a). Code::Blocks: Open the .cbp file Make sure you set the correct linker settings for curl(to add it go to Settings->Compiler->Linker Settings then in Other linker options: type "-lcurl"). If you have setup everything properly upto this point you should be able to compile and run it  
2(b). Terminal: Using terminal cd into "CSCI2270_FinalProject/Build_using_terminal". Then build using "g++ -o YahooWeatherSearch main.cpp WeatherTree.cpp -lcurl -l json -std=c++11", this should generate a file called YahooWeatherSearch that you can run using "./YahooWeatherSearch locations.txt"  
3. The program takes one command line argument, the name of the file with locations that you want to load on startup. You can run it without the filename if you want. You can change the file in Code::Blocks using Project->Set programs' arguments... or in terminal using "./YahooWeatherSearch filename"  
4. You can setup text file opened on startup file with cities that you want to load the weather for on program startup(The default is "locations.txt" ), I have put 4 cities in there, if you run it without modifying the file they should load automatically. You can change the contents by adding cities using the format city(state/region), country or city, country/region Example "Boulder(CO), United States" or "London, UK".   
5. The main menu should be fairly easy to understand, it has the following features:  
    ->"Open locations file": This allows you to open a file with locations in it and load weather data for those places like the locations.txt  
    ->"Show weather for all saved locations": Shows the saved weather data for all saved locations  
    ->"View current weather details of a location from the web": Allows you to check the weather of a place not in your saved locations without adding it after you check it out.  
    ->"View weather from saved data": Allows you to view the local weather data for a saved location.  
    ->"Add place to saved locations": Allows you to add a place to the saved locations and store its data locally  
    ->"Delete a place form saved locations": Deletes a place from saved locations.  
    ->"Count the number of places in saved locations": Counts the number of places in saved locations.  
    ->"Show all saved locations": Shows a list of all the saved locations.  
    ->"Quit": I'm fairly certain you can figure this one out.  
# Open issues/bugs
Haven't found any but please do report them if you do

