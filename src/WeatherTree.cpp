#include <iostream>
#include "WeatherTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <curl/curl.h>
#include <sstream>
using namespace std;

WeatherTree::WeatherTree()
{
    opCount = 1;
    nil = new MovieNode(0, "", 0, 0);
    root = nil;
    nil->isRed = false;
    nil->leftChild = nil->rightChild = nil;
    Assignment7Output = json_object_new_object();
}

WeatherTree::~WeatherTree()
{
    DeleteAll(root);

    // Clean up json object
    json_object_put(Assignment7Output);
}
void WeatherTree::newQuery(){
    cout<<"Enter city name"<<endl;
    string city;
    getline(cin, city);
    transform(city.begin(), city.end(), city.begin(), ::tolower);
    cout<<"Enter state name"<<endl;
    string state;
    getline(cin, state);
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
    json_object* queryMain = json_object_object_get(queryResponse, "query"); /*Getting the array if it is a key value pair*/
    if(!json_object_is_type(json_object_object_get(queryMain, "results"), json_type_null)){
    json_object* queryResults = json_object_object_get(queryMain, "results");
    json_object* queryPlaces = json_object_object_get(queryResults, "place");
    json_object* queryWoeid = json_object_array_get_idx(queryPlaces, 0);
    string woeid = json_object_get_string(json_object_object_get(queryWoeid, "woeid"));
    string weatherQuery = "https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20woeid%3D" + woeid + "&format=json&callback=";
    string weatherResponse = curlResponse(weatherQuery);
    json_object* query2Response = json_tokener_parse(weatherResponse.c_str());
    cout<<json_object_to_json_string_ext(query2Response, JSON_C_TO_STRING_PRETTY)<<endl;

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
    json_object * newJSON = json_object_new_object();
    int longestPath = countLongestPath(root);
    json_object *jsonOperation = json_object_new_string("height");
    json_object_object_add(newJSON,"operation",jsonOperation);
    json_object *jsonOutput = json_object_new_string(to_string(longestPath).c_str());
    json_object_object_add(newJSON,"output",jsonOutput);
    json_object_object_add(Assignment7Output,to_string(opCount).c_str(),newJSON);
    opCount++;
    return longestPath;
}
int WeatherTree::countLongestPath(MovieNode * node)
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
void WeatherTree::DeleteAll(MovieNode * node)
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

void WeatherTree::initJson()
{
    Assignment7Output = json_object_new_object();
}

/* Helper for the printMovieInventory recursive function */
void WeatherTree::printMovieInventory()
{
    // Create the json object for this operation
    json_object * newJSON = json_object_new_object();
    json_object * travLog = json_object_new_array();

    printMovieInventory(root,travLog);

    // Update our json object
    json_object *jsonOperation = json_object_new_string("traverse");
    json_object_object_add(newJSON,"operation",jsonOperation);
    json_object_object_add(newJSON,"output",travLog);
    json_object_object_add(Assignment7Output,to_string(opCount).c_str(),newJSON);

    opCount++;

    return;
}

/* Prints the inventory(in order traversal) */
void WeatherTree::printMovieInventory(MovieNode * node, json_object * traverseLog)
{

    // Left Node
    if(node->leftChild!=nil)
        printMovieInventory(node->leftChild,traverseLog);

    // Value
    cout<<"Movie: "<<node->title<< endl;

    // Update the traversal log
    json_object *curTitle = json_object_new_string(node->title.c_str());
    json_object_array_add(traverseLog, curTitle);

    // Right Node
    if(node->rightChild!=nil)
        printMovieInventory(node->rightChild,traverseLog);

    return;
}


void WeatherTree::addMovieNode(int ranking, std::string title, int releaseYear, int quantity)
{
    //cout << "entered!\n";
    // Create the json object for this operation
    json_object * newJSON = json_object_new_object();
    // Create a log for the traversal
    json_object * travLog = json_object_new_array();

    // Create the node we will be inserting
    MovieNode * newMovie = new MovieNode(ranking,title,releaseYear,quantity);
    newMovie->leftChild = nil;
    newMovie->rightChild = nil;
    MovieNode * x = root;
    MovieNode * y = nil;

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
            // Add the current node to the traversal log before moving to next.
            json_object *curTitle = json_object_new_string(x->title.c_str());
            json_object_array_add(travLog, curTitle);

            y = x;
            if(newMovie->title.compare(x->title) < 0)
                x = x->leftChild;
            else
                x = x->rightChild;

        }

        // set the parent of this node to be the previous node.
        newMovie->parent = y;

        // Determine which child to this previous node we are at.
        if (newMovie->title.compare(y->title) < 0)
            y->leftChild = newMovie;
        else
            y->rightChild = newMovie;

    }
    // Update our json object
    json_object *jsonOperation = json_object_new_string("add");
    json_object_object_add(newJSON,"operation",jsonOperation);
    json_object *jsonTitle = json_object_new_string(title.c_str());
    json_object_object_add(newJSON,"parameter",jsonTitle);
    json_object_object_add(newJSON,"output",travLog);
    json_object_object_add(Assignment7Output,to_string(opCount).c_str(),newJSON);
    opCount++;
    rbInsertFix(newMovie);
    //isValid();
    return;

}
void WeatherTree::rbInsertFix(MovieNode* x){
            x->leftChild = nil;
            x->rightChild = nil;
            MovieNode* y = NULL;
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

void WeatherTree::findMovie(std::string title)
{
    // Create a traversal log
    json_object * travLog = json_object_new_array();
    // Find the movie
    MovieNode * foundMovie = searchMovieTree(root,title,travLog);
    if (foundMovie != nil)
    {
        cout << "Movie Info:" << endl;
        cout << "===========" << endl;
        cout << "Ranking:" << foundMovie->ranking << endl;
        cout << "Title:" << foundMovie->title << endl;
        cout << "Year:" << foundMovie->year << endl;
        cout << "Quantity:" << foundMovie->quantity << endl;
    }
    else
        cout << "Movie not found." << endl;

    return;
}

MovieNode* WeatherTree::searchMovieTree(MovieNode * node, std::string title, json_object * traverseLog)
{
    // Add the current node to the traverse log
    if (node != nil)
    {
        json_object *curTitle = json_object_new_string(node->title.c_str());
        json_object_array_add(traverseLog, curTitle);
    }

    // If the node is NULL, we just return. Failed to find node.
    if (node == nil)
        return nil;
    // Return this node if it is the one we are searching for
    else if (node->title == title)
        return node;

    // Else return the correct recursive call.
    else
    {
        if(title.compare(node->title) < 0)
            return searchMovieTree(node->leftChild,title,traverseLog);

        else
            return searchMovieTree(node->rightChild,title,traverseLog);
    }
}

void WeatherTree::rentMovie(std::string title)
{
    // Create the json object for this operation
    json_object * newJSON = json_object_new_object();

    int stockOutput = 0;

    json_object * travLog = json_object_new_array();
    MovieNode * foundMovie = searchMovieTree(root,title,travLog);

    // If the movie exists.
    if (foundMovie != NULL)
    {
        // If it's in stock.
        if (foundMovie->quantity > 0)
        {
            cout << "Movie has been rented." << endl;
            foundMovie->quantity--;
            stockOutput = foundMovie->quantity;

            // Update our json object
            json_object *jsonOperation = json_object_new_string("rent");
            json_object_object_add(newJSON,"operation",jsonOperation);
            json_object *jsonTitle = json_object_new_string(title.c_str());
            json_object_object_add(newJSON,"parameter",jsonTitle);
            json_object *jsonOutput = json_object_new_string(to_string(stockOutput).c_str());
            json_object_object_add(newJSON,"output",jsonOutput);
            json_object_object_add(Assignment7Output,to_string(opCount).c_str(),newJSON);

            opCount++;

            //change this to print information
            cout << "Movie Info:" << endl;
            cout << "===========" << endl;
            cout << "Ranking:" << foundMovie->ranking << endl;
            cout << "Title:" << foundMovie->title << endl;
            cout << "Year:" << foundMovie->year << endl;
            cout << "Quantity:" << foundMovie->quantity << endl;
            // If the stock is 0 delete the movie
            if (foundMovie->quantity == 0)
                deleteMovieNode(foundMovie->title);

        }
        // If it's not in stock.
        else
            cout << "Movie out of stock." << endl;

    }
    // If it doesn't exist.
    else
        cout << "Movie not found." << endl;

}

void WeatherTree::deleteMovieNode(std::string title)
{

    // Create the json object for this operation
    json_object * newJSON = json_object_new_object();

    json_object * travLog = json_object_new_array();
    MovieNode * foundMovie = searchMovieTree(root,title,travLog);

    // If the movie exists
    if (foundMovie != nil)
    {
        rbDelete(foundMovie);
        json_object *jsonOperation = json_object_new_string("delete");
        json_object_object_add(newJSON,"operation",jsonOperation);
        json_object *jsonTitle = json_object_new_string(title.c_str());
        json_object_object_add(newJSON,"parameter",jsonTitle);
        json_object_object_add(newJSON,"output",travLog);
        json_object_object_add(Assignment7Output,to_string(opCount).c_str(),newJSON);
        opCount++;

    }
    // If it doesn't exist
    else
        cout << "Movie not found." << endl;



}
void WeatherTree::rbTransplant(MovieNode * u, MovieNode * v)
{
    if (u->parent == nil)
        root = v;

    else if (u == u->parent->leftChild)
        u->parent->leftChild = v;
    else
        u->parent->rightChild = v;
    v->parent = u->parent;

}

void WeatherTree::rbDelete(MovieNode * z)
{
    MovieNode * y = z;
    bool yOriginalColor = y->isRed;
    MovieNode * x = nil;
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

void WeatherTree::rbDeleteFixup(MovieNode *x)
{
    MovieNode * w = NULL;
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

int WeatherTree::countMovieNodes()
{
    // Create the json object for this operation
    json_object * newJSON = json_object_new_object();

    // Determine the count
    int count = countMovieNodes(root);

    // Update our json object
    json_object *jsonOperation = json_object_new_string("count");
    json_object_object_add(newJSON,"operation",jsonOperation);
    json_object *jsonOutput = json_object_new_string(to_string(count).c_str());
    json_object_object_add(newJSON,"output",jsonOutput);
    json_object_object_add(Assignment7Output,to_string(opCount).c_str(),newJSON);
    opCount++;

    return count;
}

int WeatherTree::countMovieNodes(MovieNode *node)
{
    if (node == nil)
        return 0;
    return countMovieNodes(node->leftChild) + countMovieNodes(node->rightChild) + 1;
}

json_object* WeatherTree::getJsonObject()
{
    return Assignment7Output;
}
int WeatherTree::rbValid(MovieNode * node)
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
        if ((node->leftChild != nil && node->leftChild->title.compare(node->title) > 0) || (node->rightChild != nil && node->rightChild->title.compare(node->title) < 0))
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

void WeatherTree::leftRotate(MovieNode *x){
    MovieNode * y = x->rightChild;
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

void WeatherTree::rightRotate(MovieNode *x){
    MovieNode * y = x->leftChild;
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
