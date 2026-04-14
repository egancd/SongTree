#pragma once
#include "songs.hpp"
#include <vector>
#include <string>

// Returns indices into `songs` whose track_name or artists contain `query`
// (case-insensitive substring match).
vector<size_t> searchSongs(const vector<Song>& songs, const string& query);

// Returns the indices of the `n` most similar songs to `target`,
// excluding the target itself, any track_id in `excluded`, and
// de-duplicating so no track_id appears twice in the results.
// Pass previously recommended track_ids in `excluded` to guarantee
// a fresh set of suggestions on every call for the same song.
vector<size_t> findSimilar(const vector<Song>& songs,
                           const Song& target,
                           const vector<string>& excluded,
                           int n = 6);
