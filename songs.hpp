#include <string>

using namespace std;

struct Song
{
    string track_id;
    string track_name;
    string artists;
    string album_name;
    SongData data;
};

struct SongData
{
    bool expl;
    float dance;
    float energy;
    float speech;
    float acoustic;
    float valence;
    string genre;
};