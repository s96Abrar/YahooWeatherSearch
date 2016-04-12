#include <iostream>
#include "WeatherTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <curl/curl.h>
#include <sstream>
#include <fstream>
using namespace std;
/*
Function prototype:
WeatherTree:WeatherTree()

Function description:
This is the only constructor for the WeatherTree class, pretty much all it does is setup the Sentinel(nil) node

Example:
wt = new WeatherTree()
*/
WeatherTree::WeatherTree(){
    nil = new WeatherNode("","","","","","");
    root = nil;
    nil->isRed = false;
    nil->leftChild = nil->rightChild = nil;
}
/*
Function prototype:
WeatherTree::~WeatherTree()

Function description:
This method is the only destructor for the class, it clears all memory that is allocated for the class

Example:
WeatherTree* wt;
delete wt;
Post condition: all the nodes will be deleted from chain and memory freed. All memory allocated for the tree will be freed.
*/
WeatherTree::~WeatherTree(){
    //Recursive function that deletes all nodes
    DeleteAll(root);
}
/*
Function prototype:
void WeatherTree::readFileIntoTree(string fileName)

Function description:
This method reads locations from a file, checks the weather at those locations from the Yahoo weather API and adds the data to the tree.

Example:
WeatherTree* wt;
wt.readFileIntoTree("locations.txt")

Precondition: The filename passed to the function must be valid,otherwise the function will give an error saying that the file cannot be opened
and will not add anything to the tree. The locations in the file must also be valid otherwise no data will be found and nothing added to the tree
Post condition: The movie nodes from the file will be added to the tree and the tree will balance itself accordingly.
*/
void WeatherTree::readFileIntoTree(string fileName){
    ifstream in_stream;
    in_stream.open(fileName);
    if (!in_stream){
        cout << "Could not open file\n";
        return;
    }
    string city;
    string state;
    while (!in_stream.eof()){
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
/*
Function prototype:
void WeatherTree::printAllWeather()

Function description:
This method prints out the weather at all the locations in the tree

Example:
WeatherTree* wt;
wt.printAllWeather()

Precondition: The tree must be have at least one element in it otherwise it will print "Nothing to show"
*/
void WeatherTree::printAllWeather(){
    if(root != nil)
        printAllWeather(root);
    else
        cout<<"Nothing to show"<<endl;
}
/*
Function prototype:
void WeatherTree::printAllWeather(WeatherNode* node)

Function description:
This method is a recursive function that prints the weather at nodes that it traverses using in -order traversal

Example:
This function is automatically called from the public method printAllWeather()
*/
void WeatherTree::printAllWeather(WeatherNode * node){
    // Left Node.
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
/*
Function prototype:
void WeatherTree::newQuery(string city, string state, bool addToTree)

Function description:
This method queries the Yahoo Weather API and depending on the arguments adds or does not add it to the tree

Example:
WeatherTree* wt;
wt.newQuery(city, state, true)//If the details should be added to the tree
wt.newQuery(city, state, false)//If the details should NOT be added to the tree

Precondition: The tree must be initialized
Post condition: The function will print the weather details for the entered location and depending on the arguments add it or not add it to the tree
*/
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
    woeidQuery = woeidQuery + "%2C%20";
    istringstream stateStream(state);
    while(getline(stateStream, token, ' ')){
        woeidQuery = woeidQuery + token + "%20";
    }
    woeidQuery = woeidQuery.substr(0, woeidQuery.length()-3);
    woeidQuery = woeidQuery + "%22&format=json&callback=";
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
/*
Function prototype:
string WeatherTree::curlResponse(string query)

Function description:
This method gets a response from the Yahoo Weather API using the cURL library function calls and returns it as a string

Example:
This functions is automatically called from public method newQuery()

*/
string WeatherTree::curlResponse(string query){
    CURL *curl;
    CURLcode res;
    string readBuffer;
    curl = curl_easy_init();
    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, query.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}
/*
Function prototype:
size_t WeatherTree::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)

Function description:
This method gets the size of the curl query

Example:
This function is automatically called by the method curlResponse
*/

size_t WeatherTree::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
/*
Function prototype:
int WeatherTree::countLongestPath()

Function description:
This method counts the longest path in the tree

Example:
WeatherTree* wt;
wt.countLongestPath()

*/
int WeatherTree::countLongestPath(){
    int longestPath = countLongestPath(root);
    return longestPath;
}
/*
Function prototype:
int WeatherTree::countLongestPath(WeatherNode* node)

Function description:
This method is a recursive implementation that counts the longest path in a tree

Example:
This function is automatically called from the public method countLongestPath()

*/
int WeatherTree::countLongestPath(WeatherNode * node){
    if (node == nil)
        return 0;
    int longestRightPath = countLongestPath(node->rightChild);
    int longestLeftPath = countLongestPath(node->leftChild);
    if(longestLeftPath>longestRightPath)
        return longestLeftPath+1;
    else
        return longestRightPath+1;
}

/*
Function prototype:
void WeatherTree::DeleteAll(WeatherNode * node)

Function description:
A recursive function that deletes all nodes using post order traversal

Example:
This function is automatically called in the class destructor

Precondition: The tree must be initialized
Post condition: The function will delete all the nodes in the tree and free memory allocated by calling it
*/
void WeatherTree::DeleteAll(WeatherNode * node){
    if (node->leftChild != nil)
        DeleteAll(node->leftChild);
    if (node->rightChild != nil)
        DeleteAll(node->rightChild);
    delete node;
    return;
}

/*
Function prototype:
void WeatherTree::printSavedLocations()

Function description:
This method prints out all the locations that are saved in the tree

Example:
WeatherTree* wt;
wt.printSavedLocations()

*/
void WeatherTree::printSavedLocations(){
    if(root != nil){
         cout<<"Your current saved locations are: "<<endl;
        printSavedLocations(root);
        }
    else
        cout<<"Nothing to show"<<endl;
    return;
}

/*
Function prototype:
void WeatherTree::printAllWeather()

Function description:
This recursive functions prints out all the locations that are saved in the tree

Example:
Automatically called by the public method printSavedLocations()

*/
void WeatherTree::printSavedLocations(WeatherNode * node){
    if(node->leftChild!=nil)
        printSavedLocations(node->leftChild);
    cout<<node->name<< endl;
    if(node->rightChild!=nil)
        printSavedLocations(node->rightChild);
    return;
}
/*
Function prototype:
void WeatherTree::addWeatherNode(string name, string date, string current, string currentConditions, string high, string low)

Function description:
This function adds a node to the Red Black Tree

Example:
WeatherTree* wt;
wt->addWeatherNode(name, date, current, currentConditions, high, low)

Precondition: The tree must be initialized
Post condition: The function will add the node to the tree
*/
void WeatherTree::addWeatherNode(string name, string date, string current, string currentConditions, string high, string low){
    WeatherNode * newMovie = new WeatherNode(name, date,current, currentConditions, high, low);
    newMovie->leftChild = nil;
    newMovie->rightChild = nil;
    WeatherNode * x = root;
    WeatherNode * y = nil;
    if (root == nil){
        root = newMovie;
        root->parent = nil;
    }
    else{
        while (x != nil){
            y = x;
            if(newMovie->name.compare(x->name) < 0)
                x = x->leftChild;
            else
                x = x->rightChild;
        }
        newMovie->parent = y;
        if (newMovie->name.compare(y->name) < 0)
            y->leftChild = newMovie;
        else
            y->rightChild = newMovie;
    }
    rbInsertFix(newMovie);
    return;
}
/*
Function prototype:
void WeatherTree::rbInsertFix(WeatherNode* x)

Function description:
A helper function to balance the tree and make sure that it is matches the red black tree requirements

Example:
Automatically called by the public method addWeatherNode

Precondition: The tree will have Red Black Violations
Post condition: The tree will be balanced
*/
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
/*
Function prototype:
void WeatherTree::findCity(std::string title)

Function description:
A function that finds a city in the tree and prints it saved weather details

Example:
WeatherTree* wt;
wt->findCity(city)

Precondition: The tree must be initialized
*/
void WeatherTree::findCity(std::string title){
    WeatherNode * foundCity = searchMovieTree(root,title);
    if (foundCity != nil){
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
/*
Function prototype:
WeatherNode* WeatherTree::searchMovieTree(WeatherNode * node, std::string title)

Function description:
A recursive function that finds a place in the tree and returns a pointer to it

Example:
The function is automatically called by the public method findCity

Precondition: The tree must be initialized
*/
WeatherNode* WeatherTree::searchMovieTree(WeatherNode * node, std::string title){
    if (node == nil)
        return nil;
    else if (node->name == title)
        return node;
    else{
        if(title.compare(node->name) < 0)
            return searchMovieTree(node->leftChild,title);

        else
            return searchMovieTree(node->rightChild,title);
    }
}
/*
Function prototype:
void WeatherTree::deleteWeatherNode(std::string title)

Function description:
A function that deletes a node using its title

Example:
WeatherTree* wt
wt->deleteWeatherNode(city)

Precondition: The tree must be initialized and the node must be in the tree
Post condition: The node will be deleted from the tree if its is found
*/
void WeatherTree::deleteWeatherNode(std::string title){
    WeatherNode * foundMovie = searchMovieTree(root,title);
    if (foundMovie != nil)
        rbDelete(foundMovie);
    else{
        cout << "City not found in saved locations" << endl;
        cout << "Search with the format city(region if any), country" << endl;
        cout << "If you still can't find it, it's probably not in your saved locations" << endl;
    }
}
/*
Function prototype:
void WeatherTree::rbDelete(WeatherNode * z)

Function description:
This functions implements the delete from the tree given a pointer to the locations of the node. It also makes sure that the tree is balanced after

Example:
Automatically called by the public method deleteWeatherNode

Precondition: The pointer passed must be valid, this functions does not check for the validity of the input
Post condition: The node is deleted and the tree is rebalanced
*/
void WeatherTree::rbDelete(WeatherNode * z){
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
            while (y->leftChild != nil)
                y = y->leftChild ;
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
/*
Function prototype:
void WeatherTree::deleteWeatherNode(std::string title)

Function description:
A function that deletes a node using its title

Example:
Automatically called by rbDelete

Post condition: The node will be transplanted
*/
void WeatherTree::rbTransplant(WeatherNode * u, WeatherNode * v){
    if (u->parent == nil)
        root = v;
    else if (u == u->parent->leftChild)
        u->parent->leftChild = v;
    else
        u->parent->rightChild = v;
    v->parent = u->parent;

}
/*
Function prototype:
void WeatherTree::rbDeleteFixup(WeatherNode *x)

Function description:
This functions does the fixup for the rbDelete functions

Example:
Automatically called by rbDelete

Precondition: The pointer passed must be valid, this functions does not check for the validity of the input
Post condition: The tree is rebalanced
*/
void WeatherTree::rbDeleteFixup(WeatherNode *x){
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

/*
Function prototype:
int WeatherTree::countWeatherNodes()

Function description:
This function counts the number of nodes in the tree

Example:
WeatherTree* wt
wt->countWeatherNodes()

*/
int WeatherTree::countWeatherNodes(){
    int count = countWeatherNodes(root);
    return count;
}
/*
Function prototype:
int WeatherTree::countWeatherNodes(WeatherNode *node)

Function description:
This function is a recursive function that counts the nodes in a tree

Example:
WeatherTree* wt
wt->countWeatherNodes()

*/
int WeatherTree::countWeatherNodes(WeatherNode *node){
    if (node == nil)
        return 0;
    return countWeatherNodes(node->leftChild) + countWeatherNodes(node->rightChild) + 1;
}
/*
Function prototype:
void WeatherTree::leftRotate(WeatherNode *x)

Function description:
This function checks performs a left rotate on the tree

Example:
Called automatically by many functions
*/
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
/*
Function prototype:
void WeatherTree::rightRotate(WeatherNode *x)

Function description:
This function checks performs a right rotate on the tree

Example:
Called automatically by many functions
*/
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
/*
Function prototype:
int WeatherTree::rbValid(WeatherNode * node)

Function description:
This function checks if there are any red black tree violations in the tree.

Example:
WeatherTree* wt
wt->rbValid
*/
void WeatherTree::isValid(){
    cout<<rbValid(root);
}
/*
Function prototype:
int WeatherTree::rbValid(WeatherNode * node)

Function description:
This function checks if there are any red black tree violations in the tree.

Example:
Called automatically by the public method isValid()
*/
int WeatherTree::rbValid(WeatherNode * node){
    int lh = 0;
    int rh = 0;
    if (node == nil)
        return 1;

    else{
        if (node->isRed){
            if(node->leftChild->isRed || node->rightChild->isRed){
                cout << "This tree contains a red violation" << endl;
                return 0;
            }
        }
        if ((node->leftChild != nil && node->leftChild->name.compare(node->name) > 0) || (node->rightChild != nil && node->rightChild->name.compare(node->name) < 0)){
            cout << "This tree contains a binary tree violation" << endl;
            return 0;
        }
        lh = rbValid(node->leftChild);
        rh = rbValid(node->rightChild);
        if (lh != 0 && rh != 0 && lh != rh){
            cout << "This tree contains a black height violation" << endl;
            return 0;
        }
        if (lh != 0 && rh != 0){
                if (node->isRed)
                    return lh;
                else
                    return lh+1;
        }
        else
            return 0;
    }
}
