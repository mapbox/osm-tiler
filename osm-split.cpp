#define RAPIDJSON_HAS_STDSTRING 1
#include <exception>
#include <vector>
#include <iostream>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/item_type.hpp>
#include <osmium/io/pbf_input.hpp>
#include <osmium/index/map/dummy.hpp>
#include <osmium/index/map/sparse_file_array.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include "handler.hpp"

typedef osmium::index::map::Dummy<osmium::unsigned_object_id_type, osmium::Location> index_neg_type;
typedef osmium::index::map::SparseFileArray<osmium::unsigned_object_id_type, osmium::Location> index_pos_type;
typedef osmium::handler::NodeLocationsForWays<index_pos_type, index_neg_type> location_handler_type;

int main(int argc, char** argv) {
  char cacheFile[] = "node-cache";

  try {
    std::string filename = argv[1];

    osmium::io::Reader reader(
      filename,
      osmium::osm_entity_bits::relation | osmium::osm_entity_bits::node | osmium::osm_entity_bits::way
    );

    int fd = open(cacheFile, O_RDWR);
    if (fd == -1) {
      std::cerr << "Can not open node cache file '" << cacheFile << "': " << strerror(errno) << "\n";
      return 1;
    }
    index_pos_type index_pos {fd};
    index_neg_type index_neg;
    location_handler_type location_handler(index_pos, index_neg);
    location_handler.ignore_errors();

    Handler handler;
    osmium::apply(reader, location_handler, handler);
    reader.close();

    std::cout << "Complete" << endl;

    return 0;
  } catch (std::exception const& ex) {
    std::clog << "error: " << ex.what() << "\n";
    return -1;
  }
}
