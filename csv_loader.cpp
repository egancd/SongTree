#include "csv_loader.hpp"
#include <fstream>
#include <iostream>

// ── Internal helpers (file-local) ─────────────────────────────────────────────

// Trim leading/trailing whitespace and surrounding quotes
static string trim(const string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n\"");
    size_t end   = s.find_last_not_of(" \t\r\n\"");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

// Split a CSV line respecting double-quoted fields
static vector<string> splitCSV(const string& line)
{
    vector<string> fields;
    string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (c == '"')
        {
            // Escaped quote ("") inside a quoted field
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"')
            { field += '"'; ++i; }
            else
            { inQuotes = !inQuotes; }
        }
        else if (c == ',' && !inQuotes)
        { fields.push_back(field); field.clear(); }
        else
        { field += c; }
    }
    fields.push_back(field);
    return fields;
}

// ── Public function ───────────────────────────────────────────────────────────

// CSV column order:
// 0: index, 1: track_id, 2: artists, 3: album_name, 4: track_name,
// 5: explicit, 6: danceability, 7: energy, 8: speechiness,
// 9: acousticness, 10: valence, 11: track_genre

bool loadCSV(const string& path, vector<Song>& songs)
{
    ifstream file(path);
    if (!file.is_open())
    {
        cerr << "Error: cannot open \"" << path << "\"\n";
        return false;
    }

    string line;
    getline(file, line); // skip header row

    while (getline(file, line))
    {
        if (line.empty()) continue;

        auto f = splitCSV(line);
        if (f.size() < 12) continue;

        Song s;
        s.track_id   = trim(f[1]);
        s.artists    = trim(f[2]);
        s.album_name = trim(f[3]);
        s.track_name = trim(f[4]);

        string explStr = trim(f[5]);
        s.data.expl    = (explStr == "True" || explStr == "true" || explStr == "1");

        try {
            s.data.dance    = stof(trim(f[6]));
            s.data.energy   = stof(trim(f[7]));
            s.data.speech   = stof(trim(f[8]));
            s.data.acoustic = stof(trim(f[9]));
            s.data.valence  = stof(trim(f[10]));
        } catch (...) { continue; }

        s.data.genre = trim(f[11]);

        songs.push_back(s);
    }

    return !songs.empty();
}
