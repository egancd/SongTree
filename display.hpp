#pragma once
#include "songs.hpp"

// Print a song with full details.
// If index >= 0, a selection number "[index]" is prepended.
void printSong(const Song& s, int index = -1);

// Print a compact one-line summary with a rank number.
void printSongBrief(int rank, const Song& s);
