#include <osmium/handler.hpp>
#include <osmium/io/pbf_input.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/item_type.hpp>
#include <string>
#include <vector>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

class Handler : public osmium::handler::Handler {
  bool geojson;
  bool pbf;

  public:
    Handler(bool createGeojson, bool createPbf) {
      geojson = createGeojson;
      pbf = createPbf;
    }

    void node(const osmium::Node& node) {
      auto const& tags = node.tags();
      double lon = node.location().lon();
      double lat = node.location().lat();

      if(geojson) {
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

        cout << nodeBuffer.GetString()  << endl;
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

        cout << wayBuffer.GetString() << endl;
      }
    }
};