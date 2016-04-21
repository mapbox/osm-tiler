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
#include "boost/filesystem.hpp"

using namespace rapidjson;
using namespace std;
//using boost::filesystem; 

static const auto decimal_to_radian = M_PI / 180;
static const string output_directory = "output";

struct Tile {
  uint x;
  uint y;
  uint z;
};

class Handler : public osmium::handler::Handler {
  bool geojson;
  bool pbf;
  uint z;

  public:
    Handler(bool createGeojson, bool createPbf, uint tileZ) {
      geojson = createGeojson;
      pbf = createPbf;
      z = tileZ;
    }

    Tile tileFromLocation(const osmium::Location location) {
      auto tile = Tile();

      auto latSin = sin(location.lat() * decimal_to_radian);
      auto z2 = pow(2, z);
      tile.x = floor(z2 * (location.lon() / 360 + 0.5));
      tile.y = floor(z2 * (0.5 - 0.25 * log((1 + latSin) / (1 - latSin)) / M_PI));
      tile.z = z;

      return tile;
    }

    string quadKeyStringForLocation(const osmium::Location location) {
      auto tile = this->tileFromLocation(location);
      return to_string(tile.x) + "" + to_string(tile.y) + "/" + to_string(tile.z);
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

    void commitToFile(const Tile &tile, const StringBuffer &nodeBuffer) {
      string filename = "nodes.json";
      string path = output_directory + "/" + to_string(tile.x) + "/" + to_string(tile.y) + "/" + filename;

      ofstream myfile;
      myfile.open(path, ios::app);
      myfile << nodeBuffer.GetString() << endl;
      myfile.close();
    }

    StringBuffer stringBufferForNode(const osmium::Node& node, const int z = 1) {
      auto const& tags = node.tags();
      double lon = node.location().lon();
      double lat = node.location().lat();

      StringBuffer nodeBuffer;
      Writer<StringBuffer> nodeWriter(nodeBuffer);

      nodeWriter.StartObject();
      nodeWriter.Key("type");
      nodeWriter.String("Feature");
      nodeWriter.Key("properties");
      nodeWriter.StartObject();
      nodeWriter.Key("id");
      nodeWriter.String(to_string(node.id()));
      for (auto& tag : tags) {
        nodeWriter.Key(tag.key());
        nodeWriter.String(tag.value());
      }
      nodeWriter.EndObject();
      nodeWriter.Key("geometry");
      nodeWriter.StartObject();
      nodeWriter.Key("type");
      nodeWriter.String("Point");
      nodeWriter.Key("coordinates");
      nodeWriter.StartArray();
      nodeWriter.Double(lon);
      nodeWriter.Double(lat);
      nodeWriter.EndArray();
      nodeWriter.EndObject();
      nodeWriter.EndObject();

      return nodeBuffer;
    }

    void node(const osmium::Node& node) {
      if(geojson) {
        StringBuffer nodeBuffer = this->stringBufferForNode(node);
        auto tile = this->tileFromLocation(node.location());
        this->makeDirectoryForTile(tile);
        this->commitToFile(tile, nodeBuffer);
      }
    }

    void way(osmium::Way& way) {
      if(geojson) {
        auto const& tags = way.tags();

        bool polygon;
        const char * area = tags.get_value_by_key("area");
        const char * highway = tags.get_value_by_key("highway");
        const char * building = tags.get_value_by_key("building");

        // is way a polygon or a linestring?
        if(area && strcmp(area, "yes") == 1) polygon = true;
        else if (area && strcmp(area, "yes") == 0) polygon = false;
        else if(building) polygon = true;
        else if(highway) polygon = false;
        else if (way.is_closed()) polygon = true;
        else polygon = false;

        StringBuffer wayBuffer;
        Writer<StringBuffer> wayWriter(wayBuffer);

        wayWriter.StartObject();
        wayWriter.Key("type");
        wayWriter.String("Feature");
        wayWriter.Key("properties");
        wayWriter.StartObject();
        wayWriter.Key("id");
        wayWriter.String(to_string(way.id()));
        for (auto& tag : tags) {
          wayWriter.Key(tag.key());
          wayWriter.String(tag.value());
        }
        wayWriter.EndObject();
        wayWriter.Key("geometry");
        wayWriter.StartObject();
        wayWriter.Key("type");
        if(polygon) {
          wayWriter.String("Polygon");
        } else {
          wayWriter.String("LineString");
        }
        wayWriter.Key("coordinates");
        wayWriter.StartArray();
        if(polygon) {
          wayWriter.StartArray();
        }
        for (auto& node : way.nodes()) {
          auto location = node.location();
          if(location) {
            wayWriter.StartArray();
            wayWriter.Double(node.location().lon());
            wayWriter.Double(node.location().lat());
            wayWriter.EndArray();
          } else {
            // ignore ways that are missing nodes
            return;
          }
        }
        if(polygon) {
          wayWriter.EndArray();
        }
        wayWriter.EndArray();
        wayWriter.EndObject();
        wayWriter.EndObject();

        //cout << wayBuffer.GetString() << endl;
      }
    }

    void relation(const osmium::Relation& relation) {
      if(geojson) {
        auto const& tags = relation.tags();
        const char * relationType = tags.get_value_by_key("type");
        const char * restrictionType = tags.get_value_by_key("restriction");

        StringBuffer relationBuffer;
        Writer<StringBuffer> relationWriter(relationBuffer);

        if (relationType && strcmp(relationType, "restriction") == 0 && (!restrictionType || strcmp(restrictionType, "no_u_turn") != 0)) {
          relationWriter.StartObject();
          relationWriter.String("type");
          if (restrictionType) {
            relationWriter.String(string(restrictionType));
          } else {
            return;
          }
          relationWriter.String("members");
          relationWriter.StartArray();
          for (auto& rm : relation.members()) {
            relationWriter.StartObject();
            relationWriter.String("role");
            relationWriter.String(string(rm.role()));
            relationWriter.String("type");
            relationWriter.String(string(osmium::item_type_to_name(rm.type())));
            relationWriter.String("ref");
            relationWriter.String(to_string(rm.ref()));
            relationWriter.EndObject();
          }
          relationWriter.EndArray();
          relationWriter.EndObject();

          //cout << relationBuffer.GetString() << endl;
        }
      }
    }
};
