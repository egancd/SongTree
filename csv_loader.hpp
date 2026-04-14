#pragma once
#include "songs.hpp"
#include <vector>
#include <string>

// Loads all songs from a CSV file into the provided vector.
// Returns true on success, false if the file could not be opened or was empty.
bool loadCSV(const string& path, vector<Song>& songs);
