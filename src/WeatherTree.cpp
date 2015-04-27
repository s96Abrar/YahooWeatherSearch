#include <iostream>
#include "WeatherTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <curl/curl.h>
#include <sstream>
#include <fstream>
using namespace std;

WeatherTree::WeatherTree()
{
    nil = new WeatherNode("","","","","","");
    root = nil;
    nil->isRed = false;
    nil->leftChild = nil->rightChild = nil;
}

WeatherTree::~WeatherTree()
{
    DeleteAll(root);
}
/* reads file into tree */
void WeatherTree::readFileIntoTree(string fileName)
{
    ifstream in_stream;
    in_stream.open(fileName);
    if (!in_stream)
    {
        cout << "Could not open file\n";
        return;
    }
    string city;
    string state;
    while (!in_stream.eof())
    {
        city = "";
        state = "";
        getline(in_stream, city, ',');
        getline(in_stream, state);
        if(city != "" && state != ""){
            state = state.substr(1, state.length());
            if(city.find('(') < 1000000){

                state = city.substr(city.find('(')+1);
                state = state.substr(0, state.length()-1);
                city = city.substr(0, city.length()-4);
            }
            newQuery(city, state, true);
            cout<<endl;
        }
    }

}
void WeatherTree::printAllWeather(){
    if(root != nil)
        printAllWeather(root);
    else
        cout<<"Nothing to show"<<endl;
}

void WeatherTree::printAllWeather(WeatherNode * node)
{
    // Left Node
    if(node->leftChild!=nil)
        printAllWeather(node->leftChild);
    // Value
    findCity(node->name);
    cout<<endl;
    // Right Node
    if(node->rightChild!=nil)
        printAllWeather(node->rightChild);

    return;
}

void WeatherTree::newQuery(string city, string state, bool addToTree){
    cout<<"Loading weather data..."<<endl;
    transform(city.begin(), city.end(), city.begin(), ::tolower);
    transform(state.begin(), state.end(), state.begin(), ::tolower);
    istringstream cityStream(city);
    string woeidQuery = "https://query.yahooapis.com/v1/public/yql?q=select%20woeid%20from%20geo.places%20where%20text%3D%22";
    string token;
    while(getline(cityStream, token, ' ')){
        woeidQuery = woeidQuery + token + "%20";
    }
    woeidQuery = woeidQuery.substr(0, woeidQuery.length()-3);
    woeidQuery = woeidQuery + "%2C%20" + state + "%22&format=json&callback=";
    string woeidResponse = curlResponse(woeidQuery);
    json_object* queryResponse = json_tokener_parse(woeidResponse.c_str());
    json_object* queryMain = json_object_object_get(queryResponse, "query");
    int resultCount = json_object_get_int(json_object_object_get(queryMain, "count"));
    if(!json_object_is_type(json_object_object_get(queryMain, "results"), json_type_null)){
        string woeid;
        json_object* queryResults = json_object_object_get(queryMain, "results");
        json_object* queryPlaces = json_object_object_get(queryResults, "place");
        if(resultCount > 1){
            json_object* queryWoeid = json_object_array_get_idx(queryPlaces, 0);
            woeid = json_object_get_string(json_object_object_get(queryWoeid, "woeid"));
        }
        else{
            woeid = json_object_get_string(json_object_object_get(queryPlaces, "woeid"));
        }
        string weatherQuery = "https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20woeid%3D" + woeid + "&format=json&callback=";
        string weatherResponse = curlResponse(weatherQuery);
        json_object* query2Response = json_tokener_parse(weatherResponse.c_str());
        json_object* query2Main = json_object_object_get(query2Response, "query");
        json_object* query2Results = json_object_object_get(query2Main, "results");
        json_object* query2Channel = json_object_object_get(query2Results, "channel");
        json_object* query2Location= json_object_object_get(query2Channel, "location");
        string city = json_object_get_string(json_object_object_get(query2Location, "city"));
        string region = json_object_get_string(json_object_object_get(query2Location, "region"));
        string country = json_object_get_string(json_object_object_get(query2Location, "country"));
        string name;
        if(region != "")
            name = city + "(" + region + ")" + ", " + country;
        else
            name = city + ", " + country;
        json_object* query2Item = json_object_object_get(query2Channel, "item");
        json_object* query2Condition = json_object_object_get(query2Item, "condition");
        string date = json_object_get_string(json_object_object_get(query2Condition, "date"));
        string current = json_object_get_string(json_object_object_get(query2Condition, "temp"));
        current += " F";
        string currentConditions = json_object_get_string(json_object_object_get(query2Condition, "text"));
        json_object* query2Forecast = json_object_object_get(query2Item, "forecast");
        json_object* query2CurrentForecast = json_object_array_get_idx(query2Forecast, 0);
        string high = json_object_get_string(json_object_object_get(query2CurrentForecast, "high"));
        high += " F";
        string low = json_object_get_string(json_object_object_get(query2CurrentForecast, "low"));
        low += " F";
        cout<<"Weather forecast for : "<<name<<endl;
        cout<<"Date & Time: "<<date<<endl;
        cout<<"Current Temperature: "<<current<<endl;
        cout<<"Current Conditions "<<currentConditions<<endl;
        cout<<"Temperature High: "<<high<<endl;
        cout<<"Temperature Low: "<<low<<endl;
        if(addToTree){
            addWeatherNode(name, date, current, currentConditions, high, low);
            cout<<"City added to saved locations"<<endl;
            }
    }
    else{
        cout<<"Unable to find any results please try again"<<endl;
    }
}
size_t WeatherTree::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string WeatherTree::curlResponse(string query){
    CURL *curl;
    CURLcode res;
    string readBuffer;
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, query.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

int WeatherTree::countLongestPath()
{
    int longestPath = countLongestPath(root);
    return longestPath;
}
int WeatherTree::countLongestPath(WeatherNode * node)
{
    if (node == nil)
        return 0;
    int longestRightPath = countLongestPath(node->rightChild);
    int longestLeftPath = countLongestPath(node->leftChild);
    if(longestLeftPath>longestRightPath)
        return longestLeftPath+1;
    else
        return longestRightPath+1;
}

/* Used to delete all nodes in the tree */
void WeatherTree::DeleteAll(WeatherNode * node)
{
    // clean to the left
    if (node->leftChild != nil)
        DeleteAll(node->leftChild);
    // clean to the right
    if (node->rightChild != nil)
        DeleteAll(node->rightChild);
    // delete this node
    delete node;

    return;
}

/* Helper for the printMovieInventory recursive function */
void WeatherTree::printSavedLocations()
{
    if(root != nil){
         cout<<"Your current saved locations are: "<<endl;
        printSavedLocations(root);
        }
    else
        cout<<"Nothing to show"<<endl;
    return;
}

/* Prints the inventory(in order traversal) */
void WeatherTree::printSavedLocations(WeatherNode * node)
{
    // Left Node
    if(node->leftChild!=nil)
        printSavedLocations(node->leftChild);
    // Value
    cout<<node->name<< endl;
    // Right Node
    if(node->rightChild!=nil)
        printSavedLocations(node->rightChild);

    return;
}



void WeatherTree::addWeatherNode(string name, string date, string current, string currentConditions, string high, string low)
{
    // Create the node we will be inserting
    WeatherNode * newMovie = new WeatherNode(name, date,current, currentConditions, high, low);
    newMovie->leftChild = nil;
    newMovie->rightChild = nil;
    WeatherNode * x = root;
    WeatherNode * y = nil;

    // Do we have an empty tree?
    if (root == nil){
        root = newMovie;
        root->parent = nil;
    }

    // If the tree is not empty
    else
    {

        // Get to the end of the tree, where we need to add this node.
        while (x != nil)
        {
            // Add the current node to the traversal log before moving to next
            y = x;
            if(newMovie->name.compare(x->name) < 0)
                x = x->leftChild;
            else
                x = x->rightChild;

        }

        // set the parent of this node to be the previous node.
        newMovie->parent = y;

        // Determine which child to this previous node we are at.
        if (newMovie->name.compare(y->name) < 0)
            y->leftChild = newMovie;
        else
            y->rightChild = newMovie;

    }
    rbInsertFix(newMovie);
    //isValid();
    return;

}

void WeatherTree::rbInsertFix(WeatherNode* x){
            x->leftChild = nil;
            x->rightChild = nil;
            WeatherNode* y = NULL;
            x->isRed = true;
            while((x != root) && (x->parent->isRed == true)){
                if ( x->parent == x->parent->parent->leftChild ) {
                    y = x->parent->parent->rightChild;
                    if ( y->isRed == true ) {
                        x->parent->isRed = false;
                        y->isRed = false;
                        x->parent->parent->isRed = true;
                        x = x->parent->parent;
                    }
                    else {
                        if ( x == x->parent->rightChild ) {
                            x = x->parent;
                            leftRotate(x);
                        }
                        x->parent->isRed = false;
                        x->parent->parent->isRed = true;
                        rightRotate(x->parent->parent);
                    }
                }
                else {
                    y = x->parent->parent->leftChild;
                    if ( y->isRed == true ) {
                        x->parent->isRed = false;
                        y->isRed = false;
                        x->parent->parent->isRed = true;
                        x = x->parent->parent;
                    }
                    else {
                        if ( x == x->parent->leftChild) {
                            x = x->parent;
                            rightRotate(x);
                        }
                        x->parent->isRed = false;
                        x->parent->parent->isRed = true;
                        leftRotate(x->parent->parent);
                }
            }
        }
        root->isRed = false;
}

void WeatherTree::findCity(std::string title)
{
    // Find the movie
    WeatherNode * foundCity = searchMovieTree(root,title);
    if (foundCity != nil)
    {
        cout<<"Weather forecast for : "<<foundCity->name<<endl;
        cout<<"Date & Time: "<<foundCity->date<<endl;
        cout<<"Current Temperature: "<<foundCity->current<<endl;
        cout<<"Current Conditions "<<foundCity->currentConditions<<endl;
        cout<<"Temperature High: "<<foundCity->high<<endl;
        cout<<"Temperature Low: "<<foundCity->low<<endl;
    }
    else{
        cout << "City not found in saved locations" << endl;
        cout << "Search with the format city(region if any), country" << endl;
        cout << "If you still can't find it add it by clicking 3 in the main menu" << endl;
        }
    return;
}

WeatherNode* WeatherTree::searchMovieTree(WeatherNode * node, std::string title)
{
    // If the node is NULL, we just return. Failed to find node.
    if (node == nil)
        return nil;
    // Return this node if it is the one we are searching for
    else if (node->name == title)
        return node;

    // Else return the correct recursive call.
    else
    {
        if(title.compare(node->name) < 0)
            return searchMovieTree(node->leftChild,title);

        else
            return searchMovieTree(node->rightChild,title);
    }
}

void WeatherTree::deleteWeatherNode(std::string title)
{

    WeatherNode * foundMovie = searchMovieTree(root,title);

    // If the movie exists
    if (foundMovie != nil)
    {
        rbDelete(foundMovie);
    }
    // If it doesn't exist
    else{
        cout << "City not found in saved locations" << endl;
        cout << "Search with the format city(region if any), country" << endl;
        cout << "If you still can't find it, it's probably not in your saved locations" << endl;
    }




}
void WeatherTree::rbTransplant(WeatherNode * u, WeatherNode * v)
{
    if (u->parent == nil)
        root = v;
    else if (u == u->parent->leftChild)
        u->parent->leftChild = v;
    else
        u->parent->rightChild = v;
    v->parent = u->parent;

}

void WeatherTree::rbDelete(WeatherNode * z)
{
    WeatherNode * y = z;
    bool yOriginalColor = y->isRed;
    WeatherNode * x = nil;
    if (z->leftChild == nil){
        x = z->rightChild;
        rbTransplant(z,z->rightChild);
    }
    else if (z->rightChild == nil){
        x = z->leftChild;
        rbTransplant(z,z->leftChild);
    }
    else{
            y = z->rightChild;
            while (y->leftChild != nil){
                y = y->leftChild ;
            }
            yOriginalColor = y->isRed;
            x = y->rightChild;
            if (y->parent == z)
                x->parent = y;
            else{
                rbTransplant(y,y->rightChild);
                y->rightChild = z->rightChild;
                y->rightChild->parent = y;
            }
            rbTransplant(z,y);
            y->leftChild = z->leftChild;
            y->leftChild->parent = y;
            y->isRed = z->isRed;
    }
    delete z;
    if (yOriginalColor == false)
        rbDeleteFixup(x);

}

void WeatherTree::rbDeleteFixup(WeatherNode *x)
{
    WeatherNode * w = NULL;
    while ((x != root) && (x->isRed == false)){
        if (x == x->parent->leftChild){
            w = x->parent->rightChild;
            if (w->isRed){
                w->isRed = false;
                x->parent->isRed = true;
                leftRotate(x->parent);
                w = x->parent->rightChild;
            }
            if (w->leftChild->isRed == false && w->rightChild->isRed == false){
                w->isRed = true;
                x = x->parent;
            }
            else{
                if (w->rightChild->isRed == false){
                    w->leftChild->isRed = false;
                    w->isRed = true;
                    rightRotate(w);
                    w = x->parent->rightChild;
                }
                w->isRed = x->parent->isRed;
                x->parent->isRed = false;
                w->rightChild->isRed = false;
                leftRotate(x->parent);
                x = root;
            }
        }
        else{
            w = x->parent->leftChild;
            if (w->isRed){
                w->isRed = false;
                x->parent->isRed = true;
                rightRotate(x->parent);
                w = x->parent->leftChild;
            }
            if (w->leftChild->isRed == false && w->rightChild->isRed == false){
                w->isRed = true;
                x = x->parent;
            }
            else{
                if (w->leftChild->isRed == false){
                    w->rightChild->isRed = false;
                    w->isRed = true;
                    leftRotate(w);
                    w = x->parent->leftChild;
                }
                w->isRed = x->parent->isRed;
                x->parent->isRed = false;
                w->leftChild->isRed = false;
                rightRotate(x->parent);
                x = root;
            }
        }

    }
    x->isRed = false;
    isValid();
}

int WeatherTree::countWeatherNodes()
{
    // Determine the count
    int count = countWeatherNodes(root);
    return count;
}

int WeatherTree::countWeatherNodes(WeatherNode *node)
{
    if (node == nil)
        return 0;
    return countWeatherNodes(node->leftChild) + countWeatherNodes(node->rightChild) + 1;
}

int WeatherTree::rbValid(WeatherNode * node)
{
    int lh = 0;
    int rh = 0;

    // If we are at a nil node just return 1
    if (node == nil)
        return 1;

    else
    {
        // First check for consecutive red links.
        if (node->isRed)
        {
            if(node->leftChild->isRed || node->rightChild->isRed)
            {
                cout << "This tree contains a red violation" << endl;
                return 0;
            }
        }

        // Check for valid binary search tree.
        if ((node->leftChild != nil && node->leftChild->name.compare(node->name) > 0) || (node->rightChild != nil && node->rightChild->name.compare(node->name) < 0))
        {
            cout << "This tree contains a binary tree violation" << endl;
            return 0;
        }

        // Deteremine the height of let and right children.
        lh = rbValid(node->leftChild);
        rh = rbValid(node->rightChild);

        // black height mismatch
        if (lh != 0 && rh != 0 && lh != rh)
        {
            cout << "This tree contains a black height violation" << endl;
            return 0;
        }

        // If neither height is zero, incrament if it if black.
        if (lh != 0 && rh != 0)
        {
                if (node->isRed)
                    return lh;
                else
                    return lh+1;
        }

        else
            return 0;
    }
}

void WeatherTree::leftRotate(WeatherNode *x){
    WeatherNode * y = x->rightChild;
    x->rightChild = y->leftChild;
    if ( y->leftChild != nil)
        y->leftChild->parent = x;
    y->parent = x->parent;
   if ( x->parent == nil )
       root = y;
   else{
       if ( x == (x->parent)->leftChild ){
           x->parent->leftChild = y;
       }
       else{
           x->parent->rightChild = y;
       }
   }
   y->leftChild = x;
   x->parent = y;
}

void WeatherTree::rightRotate(WeatherNode *x){
    WeatherNode * y = x->leftChild;
    x->leftChild = y->rightChild;
    if ( y->rightChild != nil )
        y->rightChild->parent = x;
    y->parent = x->parent;
   if ( x->parent == nil )
       root = y;
   else{
       if ( x == (x->parent)->leftChild ){
           x->parent->leftChild = y;
       }
       else{
           x->parent->rightChild = y;
       }
   }
   y->rightChild = x;
   x->parent = y;
}
void WeatherTree::isValid(){
    cout<<rbValid(root);
}
