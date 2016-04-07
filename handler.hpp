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

using namespace rapidjson;
using namespace std;

static const auto decimal_to_radian = M_PI / 180;
static const string output_directory = "output";

struct TileFraction {
  double x;
  double y;
  uint z;
};

class Handler : public osmium::handler::Handler {
  public:
    Handler() {}

    TileFraction tileFractionFromLocation(const osmium::Location location, const uint z) {
      auto tileFraction = TileFraction();

      auto latSin = sin(location.lat() * decimal_to_radian);
      auto z2 = pow(2, z);
      tileFraction.x = z2 * (location.lon() / 360 + 0.5);
      tileFraction.y = z2 * (0.5 - 0.25 * log((1 + latSin) / (1 - latSin)) / M_PI);
      tileFraction.z = z;
      
      return tileFraction;
    }

    string quadKeyStringForLocation(const osmium::Location location, const uint z) {
      auto tileFraction = this->tileFractionFromLocation(location, z);
      return to_string(tileFraction.x) + "/" + to_string(tileFraction.y) + "/" + to_string(tileFraction.z);
    }

    void commitToFile(const string &fileName, const StringBuffer &nodeBuffer) {
      ofstream outfile;
      outfile.open("test.dat", ios::out | ios::app);
      outfile << nodeBuffer.GetString();
    }

    void node(const osmium::Node& node) {
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

      auto quadKey = this->quadKeyStringForLocation(node.location(), 1);

      this->commitToFile(quadKey, nodeBuffer);

      //cout << nodeBuffer.GetString()  << endl;
    }
};
