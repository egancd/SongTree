#include "recommender.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

// ── Internal helpers ──────────────────────────────────────────────────────────

static bool containsCI(const string& haystack, const string& needle)
{
    string h = haystack, n = needle;
    transform(h.begin(), h.end(), h.begin(), ::tolower);
    transform(n.begin(), n.end(), n.begin(), ::tolower);
    return h.find(n) != string::npos;
}

// Weighted Euclidean distance in audio-feature space.
// Lower score = more similar.
static float distance(const Song& a, const Song& b)
{
    const float W_DANCE    = 1.5f;
    const float W_ENERGY   = 1.5f;
    const float W_VALENCE  = 1.2f;
    const float W_ACOUSTIC = 1.0f;
    const float W_SPEECH   = 0.8f;
    const float W_GENRE    = 2.0f;  // same genre → 0 penalty, different → 1

    float genrePenalty = (a.data.genre == b.data.genre) ? 0.0f : 1.0f;

    float d = 0.0f;
    d += W_DANCE    * powf(a.data.dance    - b.data.dance,    2);
    d += W_ENERGY   * powf(a.data.energy   - b.data.energy,   2);
    d += W_VALENCE  * powf(a.data.valence  - b.data.valence,  2);
    d += W_ACOUSTIC * powf(a.data.acoustic - b.data.acoustic, 2);
    d += W_SPEECH   * powf(a.data.speech   - b.data.speech,   2);
    d += W_GENRE    * genrePenalty;

    return sqrtf(d);
}

// ── Public functions ──────────────────────────────────────────────────────────

vector<size_t> searchSongs(const vector<Song>& songs, const string& query)
{
    vector<size_t> results;
    for (size_t i = 0; i < songs.size(); ++i)
        if (containsCI(songs[i].track_name, query) ||
            containsCI(songs[i].artists,    query))
            results.push_back(i);
    return results;
}

vector<size_t> findSimilar(const vector<Song>& songs,
                           const Song& target,
                           const vector<string>& excluded,
                           int n)
{
    // ── 1. Score every eligible song ─────────────────────────────────────────
    vector<pair<float, size_t>> scored;
    scored.reserve(songs.size());
    for (size_t i = 0; i < songs.size(); ++i)
    {
        const string& tid = songs[i].track_id;
        if (tid == target.track_id) continue;
        if (find(excluded.begin(), excluded.end(), tid) != excluded.end()) continue;
        scored.push_back({ distance(target, songs[i]), i });
    }

    sort(scored.begin(), scored.end(),
         [](const pair<float,size_t>& a, const pair<float,size_t>& b)
         { return a.first < b.first; });

    // ── 2. Build de-duplicated candidate pool (top POOL_SIZE unique tracks) ──
    // De-duplicate by both track_id and track_name so different editions of
    // the same song don't both appear.
    const int POOL_SIZE = 50;
    vector<size_t> pool;
    vector<string> seenIds;
    vector<string> seenNames;
    for (auto& [dist, idx] : scored)
    {
        const string& tid  = songs[idx].track_id;
        const string& name = songs[idx].track_name;
        if (find(seenIds.begin(),   seenIds.end(),   tid)  != seenIds.end())   continue;
        if (find(seenNames.begin(), seenNames.end(), name) != seenNames.end()) continue;
        seenIds.push_back(tid);
        seenNames.push_back(name);
        pool.push_back(idx);
        if ((int)pool.size() == POOL_SIZE) break;
    }

    // ── 3. Randomly sample n from the pool ───────────────────────────────────
    // Seed with the current time so each call yields a different shuffle.
    auto seed = chrono::steady_clock::now().time_since_epoch().count();
    mt19937 rng(static_cast<mt19937::result_type>(seed));
    shuffle(pool.begin(), pool.end(), rng);

    pool.resize(min(n, (int)pool.size()));
    return pool;
}