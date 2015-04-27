#ifndef MOVIETREE_H
#define MOVIETREE_H

#include <json/json.h>

struct WeatherNode{
    std::string name;
    std::string date;
    std::string current;
    std::string currentConditions;
    std::string high;
    std::string low;
    bool isRed;
    WeatherNode *parent;
    WeatherNode *leftChild;
    WeatherNode *rightChild;

    WeatherNode(){};


    WeatherNode(std::string in_name, std::string in_date, std::string in_current, std::string in_currentConditions, std::string in_high,std::string in_low){
        name = in_name;
        date = in_date;
        current = in_current;
        currentConditions = in_currentConditions;
        high = in_high;
        low = in_low;
    }

};

class WeatherTree
{

    public:
        WeatherTree();
        virtual ~WeatherTree();
        void printSavedLocations();//
        void printAllWeather();
        int countWeatherNodes();
        void deleteWeatherNode(std::string city);
        void addWeatherNode(std::string name, std::string date, std::string current, std::string currentConditions, std::string high,std::string low);
        void findCity(std::string city);
        void isValid();
        int countLongestPath();
        void newQuery(std::string city,std::string state, bool);
        void readFileIntoTree(std::string);


    protected:

    private:
        void DeleteAll(WeatherNode * node); //use this for the post-order traversal deletion of the tree
        void printSavedLocations(WeatherNode * node);
        void rbAddFixup(WeatherNode * node); // called after insert to fix tree
        void leftRotate(WeatherNode * x);
        void rbDelete(WeatherNode * x);
        void rightRotate(WeatherNode * x);
        void rbDeleteFixup(WeatherNode * node);
        void rbTransplant(WeatherNode * u, WeatherNode * v);
        int rbValid(WeatherNode * node);
        void rbInsertFix(WeatherNode* node);
        int countWeatherNodes(WeatherNode *node);
        int countLongestPath(WeatherNode *node);
        WeatherNode* searchMovieTree(WeatherNode * node, std::string title);
        WeatherNode *root;
        WeatherNode *nil;
        std::string curlResponse(std::string query);
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
        void printAllWeather(WeatherNode* node);
        // Count of how many operations we have done.
        //including the json_object in the class makes it global within the class, much easier to work with



};

#endif // MOVIETREE_H
