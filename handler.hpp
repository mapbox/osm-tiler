#include <osmium/handler.hpp>
#include <osmium/io/pbf_input.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/item_type.hpp>
#include <string>
#include <vector>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <unordered_map>

using namespace rapidjson;
using namespace std;

static const auto decimal_to_radian = M_PI / 180;
static const string output_directory = "output";

typedef vector<string> TileVector;  

struct Tile {
  uint x;
  uint y;
  uint z;
};

class Handler : public osmium::handler::Handler {
  uint z;
  unordered_map<int,vector<string>> idx;

  public:
    Handler(uint tileZ) {
      z = tileZ;
    }

    Tile pointToTile(const double lon, const double lat) {
      auto tile = Tile();

      auto latSin = sin(lat * decimal_to_radian);
      auto z2 = pow(2, z);
      tile.x = floor(z2 * (lon / 360 + 0.5));
      tile.y = floor(z2 * (0.5 - 0.25 * log((1 + latSin) / (1 - latSin)) / M_PI));
      tile.z = z;

      return tile;
    }

    string xy (const double lon, const double lat) {
      auto tile = pointToTile(lon, lat);
      return to_string(tile.x) + "/" + to_string(tile.y);
    }

    int makeDirectoryForTile(const Tile &tile) {
      string curPath = "output";
      mkdir( curPath.c_str(), 0777); 

      curPath += "/" +  to_string(tile.x);
      mkdir( curPath.c_str(), 0777);
      
      curPath += "/" +  to_string(tile.y);
      mkdir( curPath.c_str(), 0777); 

      return true;
    }

    void node(const osmium::Node& node) {
      auto const& tags = node.tags();
      auto lon = node.location().lon();
      auto lat = node.location().lat();
      auto id = node.id();
      TileVector tiles = {string(xy(lon, lat))};

      pair<int,TileVector> nodeIndex(id, tiles);
      idx.insert(nodeIndex);

      StringBuffer nodeBuffer;
      Writer<StringBuffer> nodeWriter(nodeBuffer);

      nodeWriter.StartObject();

      nodeWriter.Key("id");
      nodeWriter.Int(id);
      nodeWriter.Key("lon");
      nodeWriter.Double(lon);
      nodeWriter.Key("lat");
      nodeWriter.Double(lat);
      nodeWriter.Key("version");
      nodeWriter.Int(node.version());
      nodeWriter.Key("timestamp");
      nodeWriter.Int(node.timestamp().seconds_since_epoch());
      nodeWriter.Key("user");
      nodeWriter.String(node.user());

      nodeWriter.Key("tags");
      nodeWriter.StartObject();
      for (auto& tag : tags) {
        nodeWriter.Key(tag.key());
        nodeWriter.String(tag.value());
      }
      nodeWriter.EndObject();
      nodeWriter.EndObject();

      //cout << nodeBuffer.GetString() << endl;
    }

    void way(const osmium::Way& way) {
      auto const& tags = way.tags();
      auto id = way.id();

      StringBuffer wayBuffer;
      Writer<StringBuffer> wayWriter(wayBuffer);

      wayWriter.StartObject();

      wayWriter.Key("id");
      wayWriter.Int(id);
      wayWriter.Key("version");
      wayWriter.Int(way.version());
      wayWriter.Key("timestamp");
      wayWriter.Int(way.timestamp().seconds_since_epoch());
      wayWriter.Key("user");
      wayWriter.String(way.user());
      wayWriter.Key("closed");
      wayWriter.Bool(way.is_closed());

      wayWriter.Key("node_refs");
      wayWriter.StartArray();
      for (auto& node : way.nodes()) {
        wayWriter.Int(node.ref());
      }
      wayWriter.EndArray();

      wayWriter.Key("tags");
      wayWriter.StartObject();
      for (auto& tag : tags) {
        wayWriter.Key(tag.key());
        wayWriter.String(tag.value());
      }
      wayWriter.EndObject();
      wayWriter.EndObject();

      //cout << wayBuffer.GetString() << endl;
    }

    void relation(const osmium::Relation& relation) {
      auto const& tags = relation.tags();
      auto id = relation.id();

      StringBuffer relationBuffer;
      Writer<StringBuffer> relationWriter(relationBuffer);

      relationWriter.StartObject();

      relationWriter.Key("id");
      relationWriter.Int(id);
      relationWriter.Key("version");
      relationWriter.Int(relation.version());
      relationWriter.Key("timestamp");
      relationWriter.Int(relation.timestamp().seconds_since_epoch());
      relationWriter.Key("user");
      relationWriter.String(relation.user());

      relationWriter.Key("members");
      relationWriter.StartArray();
      for (auto& member : relation.members()) {
        relationWriter.StartObject();
        relationWriter.Key("id");
        relationWriter.Int(member.ref());
        relationWriter.Key("type");
        relationWriter.String(item_type_to_name(member.type()));
        relationWriter.Key("role");
        relationWriter.String(member.role());
        relationWriter.EndObject();
      }
      relationWriter.EndArray();

      relationWriter.Key("tags");
      relationWriter.StartObject();
      for (auto& tag : tags) {
        relationWriter.Key(tag.key());
        relationWriter.String(tag.value());
      }
      relationWriter.EndObject();
      relationWriter.EndObject();

      //cout << relationBuffer.GetString() << endl;
    }
};
