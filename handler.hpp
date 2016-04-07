#include <osmium/handler.hpp>
#include <osmium/io/pbf_input.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/item_type.hpp>
#include <string>
#include <vector>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <cmath>

using namespace rapidjson;
using namespace std;

static const auto decimal_to_radian = M_PI / 180;

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

    std::string quadKeyStringForLocation(const osmium::Location location, const uint z) {
      auto tileFraction = this->tileFractionFromLocation(location, z);
      return std::to_string(tileFraction.x) + "/" + std::to_string(tileFraction.y) + "/" + std::to_string(tileFraction.z);
    }

    void node(const osmium::Node& node) {
      auto const& tags = node.tags();
      double lon = node.location().lon();
      double lat = node.location().lat();

      auto quadKey = this->quadKeyStringForLocation(node.location(), 1);

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

      //cout << nodeBuffer.GetString()  << endl;
    }
};