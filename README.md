# YahooWeatherSearch
Project Summary:  
A simple command line application that fetches weather data for various locations from the Yahoo Weather API
The user will be able to make a file with places that he usually checks the weather for. The program will check the weather for those places on startup and add them to a balanced binary tree.
When the program is running the user will be able to search for, delete a location and add a place to the list
# Dependencies
1.json-c  
Install by typing sudo apt-get install libjson0 libjson0-dev in terminal  
2.curl  
Install by typing sudo apt-get install libcurl4-gnutls-dev  in terminal
curl also requires linking while compiling, to add it in codeblocks go to Settings->Compiler->Linker Settings then in Other linker options: type -lcurl  
*Probably should: sudo apt-get update  
#Instructions to run
The applications should be fairly easy to run if you are running linux(Debian based) with the latest version of Code::Blocks installed. It may work on other operating systems as well but I have not tested it, you can try but I am fairly certain that since the library of JSON used is linux specific it won't be easy.  
Allright so this is what you need to do:  
1. Get all the dependencies listed in the dependencies section.  
2. Make sure you set the correct linker settings(Instructions also given in the dependencies section for curl)  
3. If you have setup everything properly upto this point you should be able to compile and run it, the program takes one command line argument, the name of the file with locations that you want to load on startup, if the file is not openable then you will get an error saying so when you run it, if the file is empty then the program simply will not load anything from it.      
4. You can setup a locations.txt file with cities that you want to load the weather for on program startup, I have put 4 cities in there, if you run it without modifying the file they should load automatically. You can change the contents by adding cities using the format city(state/region), country or city, country/region.    
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
 

