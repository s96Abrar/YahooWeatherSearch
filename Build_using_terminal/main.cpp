#include <iostream>
#include "WeatherTree.h"
#include <fstream>
#include <string>
#include <json/json.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <cstdlib>
#include <curl/curl.h>
#include <sstream>
#include <algorithm>
using namespace std;

struct Movie{
    string ranking;
    string title;
    string quantity;
    string releaseYear;
    Movie(){};

    Movie(string in_ranking, string in_title, string in_year, string in_quantity)
    {
        ranking = in_ranking;
        title = in_title;
        releaseYear = in_year;
        quantity = in_quantity;
    }
};


void displayMenu();
int getFileSize(char * fileName);

int main(int argc, char*argv[])
{
    // Create an instance of the WeatherTree class
    WeatherTree *weatherData = new WeatherTree();
    // Reads the file that was given to it as a command line argument and adds it to the tree
    weatherData->readFileIntoTree(string(argv[1]));
    // Flag used for exiting menu
    bool quit = false;
    // Used for input
    string title;
    int input;
    string city;
    string state;
    while(quit != true)
    {
        displayMenu();
        cin >> input;
        //clear out cin
        cin.clear();
        cin.ignore(10000,'\n');

        switch (input)
        {
            //Add places form file to saved locations
            case 1:
                cout << "Enter file name(including the extension like .txt):" << endl;
                getline(cin,title);
                weatherData->readFileIntoTree(title);
                break;
            //Print all locally stored weather data
            case 2:
                weatherData->printAllWeather();
                break;
            //Search the Yahoo API for the weather information of a location
            case 3:
                cout<<"Enter city name"<<endl;
                getline(cin, city);
                cout<<"Enter state/country name"<<endl;
                getline(cin, state);
                weatherData->newQuery(city, state, false);
                break;
            // Find and print the data of a saved location
            case 4:
                cout << "Enter city name:" << endl;
                getline(cin,title);
                weatherData->findCity(title);
                break;
            //Search the Yahoo API for weather information of a location and add it to the tree
            case 5:
                cout<<"Enter city name"<<endl;
                getline(cin, city);
                cout<<"Enter state/country name"<<endl;
                getline(cin, state);
                weatherData->newQuery(city, state, true);
                break;
            // Delete a Node
            case 6:
                cout << "Enter city name:" << endl;
                getline(cin,title);
                weatherData->deleteWeatherNode(title);
                break;
            // Count Tree size
            case 7:
                cout << "The number of saved cities is " << weatherData->countWeatherNodes() << endl;
                break;
            // Print all names of all the saved locations
            case 8:
                weatherData->printSavedLocations();
                break;
            //Quit the program
            case 9:
                cout << "Goodbye!" << endl;
                quit = true;
                break;
            // Incase of invalid input
            default:
                cout << "Invalid Input" << endl;
                cin.clear();
                cin.ignore(10000,'\n');
                break;
        }
    }
    // Free memory and return
    delete weatherData;


    return 0;
}

/*displays a menu with options to enqueue and dequeue a message and transmit the entire message and quit*/
void displayMenu()
{
    cout << "======Main Menu=====" << endl;
    cout << "1. Open locations file" << endl;
    cout << "2. Show weather for all saved locations" << endl;
    cout << "3. View current weather details of a location from the web" << endl;
    cout << "4. View weather from saved data" << endl;
    cout << "5. Add place to saved locations" << endl;
    cout << "6. Delete a place form saved locations" << endl;
    cout << "7. Count the number of places in saved locations" << endl;
    cout << "8. Show all saved locations" << endl;
    cout << "9. Quit"<<endl;
    return;
}

