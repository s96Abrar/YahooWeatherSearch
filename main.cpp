#include <iostream>
#include "WeatherTree.h"
#include <fstream>  //allows istream/ostream
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
    int input;
    string city;
    string state;
    // Determine the size of the text file.
    //int fileSize = getFileSize(argv[1]);
    //cout << "about to create object\n";
    // Create a new communication network
    WeatherTree *mt = new WeatherTree();
    // Read each line and add it to tree
    mt->readFileIntoTree(string(argv[1]));

    // Flag used for exiting menu
    bool quit = false;

    // Used for input
    string title;

    while(quit != true)
    {
        displayMenu();
        cin >> input;

        //clear out cin
        cin.clear();
        cin.ignore(10000,'\n');

        switch (input)
        {
            // Find a movie
            /*
            case 1:
                cout << "Enter title:" << endl;
                getline(cin,title);
                mt->findMovie(title);
                break;
            */
            // Rent a movie
            case 1:
                cout << "Enter file name(including the extension like .txt):" << endl;
                getline(cin,title);
                mt->readFileIntoTree(title);
                break;
            case 2:
                mt->printAllWeather();
                break;
            case 3:
                cout<<"Enter city name"<<endl;
                getline(cin, city);
                cout<<"Enter state/country name"<<endl;
                getline(cin, state);
                mt->newQuery(city, state, false);
                break;
            // Print the inventory
            case 4:
                cout << "Enter city name:" << endl;
                getline(cin,title);
                mt->findCity(title);
                break;
            case 5:
                cout<<"Enter city name"<<endl;
                getline(cin, city);
                cout<<"Enter state/country name"<<endl;
                getline(cin, state);
                mt->newQuery(city, state, true);
                break;
            // Delete Node
            case 6:
                cout << "Enter city name:" << endl;
                getline(cin,title);
                mt->deleteWeatherNode(title);
                break;
            // Count Tree
            case 7:
                cout << "The number of saved cities is " << mt->countWeatherNodes() << endl;
                break;
            // Quit
            case 8:
                cout<<"Your current saved locations are: "<<endl;
                mt->printSavedLocations();
                break;
            case 9:
                cout << "Goodbye!" << endl;
                quit = true;
                break;
            // invalid input
            default:
                cout << "Invalid Input" << endl;
                cin.clear();
                cin.ignore(10000,'\n');
                break;
        }
    }
    // Free memory and return
    delete mt;


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

/*grabs the number of words in a file */
int getFileSize(char * fileName)
{
    ifstream in_stream;
    in_stream.open(fileName);
    int count = 0;
    string word;

    while (!in_stream.eof())
    {
        getline(in_stream,word);
        count++;
    }

    in_stream.close();

    return count;
}



