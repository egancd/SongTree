CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
TARGET   = song_recommender

OBJS = main.o csv_loader.o recommender.o display.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o:        main.cpp        songs.hpp csv_loader.hpp recommender.hpp display.hpp
csv_loader.o:  csv_loader.cpp  csv_loader.hpp  songs.hpp
recommender.o: recommender.cpp recommender.hpp songs.hpp
display.o:     display.cpp     display.hpp     songs.hpp

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
