#include <osmium/handler.hpp>
#include <osmium/io/pbf_input.hpp>
#include <osmium/visitor.hpp>
#include <leveldb/db.h>
#include <osmium/osm/item_type.hpp>
#include <string>
#include <vector>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

class Handler : public osmium::handler::Handler {
  public:
    Handler() {
    }

    void relation(const osmium::Relation& relation) {
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

      //cout << nodeBuffer.GetString()  << endl;
    }

    // void way(const osmium::Way& way) {
    //   auto const& tags = way.tags();

    //   StringBuffer wayBuffer;
    //   Writer<StringBuffer> wayWriter(wayBuffer);

    //   wayWriter.StartObject();
    //   wayWriter.Key("type");
    //   wayWriter.String("Feature");
    //   wayWriter.Key("properties");
    //   wayWriter.StartObject();
    //   wayWriter.Key("id");
    //   wayWriter.String(to_string(way.id()));
    //   for (auto& tag : tags) {
    //     wayWriter.Key(tag.key());
    //     wayWriter.String(tag.value());
    //   }
    //   wayWriter.EndObject();
    //   wayWriter.Key("geometry");
    //   wayWriter.StartObject();
    //   wayWriter.Key("type");
    //   wayWriter.String("Polygon");
    //   wayWriter.Key("coordinates");
    //   wayWriter.StartArray();
    //   for (auto& node : way.nodes()) {
    //     //string id = to_string(node.ref());

    //     //wayWriter.StartArray();
    //     //wayWriter.Double(lon);
    //     //wayWriter.Double(lat);
    //     //wayWriter.EndArray();
    //   }
    //   wayWriter.EndArray();
    //   wayWriter.EndObject();
    //   wayWriter.EndObject();
    // }
};