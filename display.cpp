#include "display.hpp"
#include <iostream>
#include <iomanip>

void printSong(const Song& s, int index)
{
    if (index >= 0) cout << "  [" << index << "] ";
    else            cout << "      ";
    cout << "\"" << s.track_name << "\"  by " << s.artists
         << "\n        Album: " << s.album_name
         << "  |  Genre: " << s.data.genre << "\n";
}

void printSongBrief(int rank, const Song& s)
{
    cout << "  " << rank << ". \"" << s.track_name << "\"  —  " << s.artists
         << "  [" << s.data.genre << "]\n";
}
