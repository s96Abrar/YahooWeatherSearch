#include <iostream>
#include "MovieTree.h"
#include <fstream>  //allows istream/ostream
#include <string>
#include <json/json.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <cstdlib>
#include <curl/curl.h>
#include <sstream>

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
static string readBuffer;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void displayMenu();
int getFileSize(char * fileName);
void readFileIntoTree(MovieTree * mt, char * fileName);

int main(int argc, char*argv[])
{
    cout<<"Enter city name";
    string city;
    getline(cin, city);
    istringstream cityStream(city);
    string woeidQuery = "https://query.yahooapis.com/v1/public/yql?q=select%20woeid%20from%20geo.places%20where%20text%3D%22";
    string token;
    while(getline(cityStream, token, ' ')){
        woeidQuery = woeidQuery + token + "%20";
    }
    woeidQuery = woeidQuery.substr(0, woeidQuery.length()-3);
    while(getline(cityStream, token, ',')){
        woeidQuery = woeidQuery + "%2C%20" + token + "%22&format=json&callback=";
    }
    CURL *curl;
    CURLcode res;
    string readBuffer;
    //woeidQuery = string("https://query.yahooapis.com/v1/public/yql?q=select%20woeid%20from%20geo.places%20where%20text%3D%22") + "san" + "%20" + "francisco"  + "%2C%20" + "ca" + "%22&format=json&callback=";
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, woeidQuery.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        //cout << readBuffer << std::endl;
    }
    json_object* queryResponse = json_tokener_parse(readBuffer.c_str());
    json_object* queryMain = json_object_object_get(queryResponse, "query"); /*Getting the array if it is a key value pair*/
    json_object* queryResults = json_object_object_get(queryMain, "results");
    json_object* queryPlaces = json_object_object_get(queryResults, "place");
    json_object* queryWoeid = json_object_array_get_idx(queryPlaces, 0);
    string woeid = json_object_get_string(json_object_object_get(queryWoeid, "woeid"));
    cout<<woeid;
    int input;
    // Determine the size of the text file.
    //int fileSize = getFileSize(argv[1]);
    //cout << "about to create object\n";
    // Create a new communication network
    MovieTree *mt = new MovieTree();

    // Read each line and add it to tree
    readFileIntoTree(mt, argv[1]);

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
                cout << "Enter title:" << endl;
                getline(cin,title);
                mt->rentMovie(title);
                break;
            // Print the inventory
            case 2:
                mt->printMovieInventory();
                mt->isValid();
                break;

            // Delete Node
            case 3:
                cout << "Enter title:" << endl;
                getline(cin,title);
                mt->deleteMovieNode(title);
                break;
            // Count Tree
            case 4:
                cout << "Tree contains: " << mt->countMovieNodes() << " nodes." << endl;
                break;
            // Quit
            case 5:
                cout<<mt->countLongestPath()<<endl;
                break;
            case 6:
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

    // Write our JSON object to a file
    ofstream myfile;
    myfile.open("Assignment7Output.txt");
    myfile << json_object_to_json_string_ext(mt->getJsonObject(), JSON_C_TO_STRING_PRETTY);
    myfile.close();


    // Free memory and return
    delete mt;

    return 0;
}

/*displays a menu with options to enqueue and dequeue a message and transmit the entire message and quit*/
void displayMenu()
{
    cout << "======Main Menu=====" << endl;
    //cout << "1. Find a movie" << endl;
    cout << "1. Rent a movie" << endl;
    cout << "2. Print the inventory" << endl;
    cout << "3. Delete a movie" << endl;
    cout << "4. Count the movies" << endl;
    cout << "5. Count longest path" << endl;
    cout << "6. Quit"<<endl;
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

/* reads file into tree */
void readFileIntoTree(MovieTree * mt, char * fileName)
{
    ifstream in_stream;
    //cout << fileName << endl;
    in_stream.open(fileName);
    if (!in_stream)
    {
        cout << "Could not open file\n";
        return;
    }
    string ranking;
    string title;
    string releaseYear;
    string quantity;

    while (!in_stream.eof())
    {
        title ="";
        getline(in_stream, ranking, ',');
        getline(in_stream, title, ',');
        getline(in_stream, releaseYear, ',');
        getline(in_stream, quantity); // "\n" is the default delimiter
        if (title != "")
        {
            //cout << "Adding: " << title << endl;
            mt->addMovieNode(atoi(ranking.c_str()),title,atoi(releaseYear.c_str()),atoi(quantity.c_str()));
        }
    }
}

