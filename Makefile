all:
	g++ -Wall -g -o YahooWeatherSearch main.cpp WeatherTree.cpp -lcurl -l json-c -std=c++11

clean:
	rm -rf YahooWeatherSearch
