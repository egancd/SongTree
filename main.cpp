#include "songs.hpp"
#include "csv_loader.hpp"
#include "recommender.hpp"
#include "display.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
    // CSV path can be passed as an argument; defaults to dataset filename
    string csvPath = (argc > 1) ? argv[1] : "songtreedataset.csv";

    cout << "Loading song database...\n";
    vector<Song> songs;
    if (!loadCSV(csvPath, songs))
    {
        cerr << "Failed to load \"" << csvPath << "\".\n"
             << "Usage: " << argv[0] << " [path/to/songtreedataset.csv]\n";
        return 1;
    }
    cout << "Loaded " << songs.size() << " songs.\n\n";

    // Track every track_id we've already recommended so results stay fresh
    vector<string> recommended_ids;

    // ── Search loop ───────────────────────────────────────────────────────────
    while (true)
    {
        cout << "============================================================\n";
        cout << "Welcome to SongTree! Expand your taste starting with the songs and artists you know and love." << endl;
        cout << "Enter a song to start from below. Can't think of a song?\nJust enter an artist you love and we'll tell you which songs of theirs we have data on." << endl;
        cout << "Enter a song name or artist (or 'quit' to exit): ";
        string query;
        getline(cin, query);
        if (query.empty()) continue;

        string qLower = query;
        transform(qLower.begin(), qLower.end(), qLower.begin(), ::tolower);
        if (qLower == "quit" || qLower == "exit" || qLower == "q") break;

        vector<size_t> matches = searchSongs(songs, query);

        if (matches.empty())
        {
            cout << "No songs found matching \"" << query << "\". Try a different search.\n\n";
            continue;
        }

        // ── Show results (cap at 10) ──────────────────────────────────────────
        const int MAX_DISPLAY = 10;
        cout << "\nFound " << matches.size() << " match"
             << (matches.size() == 1 ? "" : "es")
             << (matches.size() > MAX_DISPLAY
                    ? " (showing first " + to_string(MAX_DISPLAY) + ")"
                    : "")
             << ":\n";

        int shown = min((int)matches.size(), MAX_DISPLAY);
        for (int i = 0; i < shown; ++i)
            printSong(songs[matches[i]], i);

        // ── User picks one ────────────────────────────────────────────────────
        int choice = -1;
        if (matches.size() == 1)
        {
            choice = 0;
            cout << "\nOnly one match found — selecting automatically.\n";
        }
        else
        {
            cout << "\nEnter the number of the song you meant"
                 << " (or 'back' to search again): ";
            string input;
            getline(cin, input);
            string il = input;
            transform(il.begin(), il.end(), il.begin(), ::tolower);
            if (il == "back" || il == "b") { cout << "\n"; continue; }

            try { choice = stoi(input); }
            catch (...) { choice = -1; }

            if (choice < 0 || choice >= shown)
            {
                cout << "Invalid selection. Returning to search.\n\n";
                continue;
            }
        }

        const Song& selected = songs[matches[choice]];

        // ── Confirm ───────────────────────────────────────────────────────────
        cout << "\nYou selected:\n";
        printSong(selected);
        cout << "  Features → dance: "  << fixed << setprecision(3) << selected.data.dance
             << "  energy: "            << selected.data.energy
             << "  valence: "           << selected.data.valence
             << "  acoustic: "          << selected.data.acoustic
             << "  speech: "            << selected.data.speech
             << "\n\nIs this the right song? (y/n): ";

        string confirm;
        getline(cin, confirm);
        string cl = confirm;
        transform(cl.begin(), cl.end(), cl.begin(), ::tolower);
        if (cl != "y" && cl != "yes") { cout << "Let's try again.\n\n"; continue; }

        // ── Recommend ─────────────────────────────────────────────────────────
        vector<size_t> recommended = findSimilar(songs, selected, recommended_ids, 6);

        // Remember these so they won't appear in future rounds
        for (size_t idx : recommended)
            recommended_ids.push_back(songs[idx].track_id);

        cout << "\n🎵  Because you liked \"" << selected.track_name
             << "\" by " << selected.artists << ",\n"
             << "    you might also enjoy:\n\n";

        for (int i = 0; i < (int)recommended.size(); ++i)
            printSongBrief(i + 1, songs[recommended[i]]);

        cout << "\n";
    }

    cout << "\nGoodbye!\n";
    return 0;
}
