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
#include <boost/program_options.hpp>
#include "handler.hpp"

#define VERSION "v1.0.0\n"

using namespace boost::program_options;

typedef osmium::index::map::Dummy<osmium::unsigned_object_id_type, osmium::Location> index_neg_type;
typedef osmium::index::map::SparseFileArray<osmium::unsigned_object_id_type, osmium::Location> index_pos_type;
typedef osmium::handler::NodeLocationsForWays<index_pos_type, index_neg_type> location_handler_type;

int main(int argc, char** argv) {
  char cacheFile[] = "node-cache";

  try {
    options_description desc("\nusage:  osm-split [options] <file>");

    desc.add_options()
    ("help,h", "show help")
    ("version,v", "show version number")
    ("zoom,z", "zoom level of tiles")
    ("pbf,p", "output tiled osm.pbf files")
    ("geojson,g", "output tiled line-delimitted geojson files");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if(vm.count("help")) {
      cout << desc;
    } else if(vm.count("version")) {
      cout << VERSION;
    } else {
      bool geojson = false;
      bool pbf = false;
      uint zoom = 0;

      if(vm.count("geojson")) geojson = true;
      if(vm.count("pbf")) pbf = true;
      if(vm.count("zoom")) zoom = 7;//vm["zoom"].as<uint>();
      
      cout << vm["zoom"] << endl;

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

      Handler handler(geojson, pbf, zoom);
      osmium::apply(reader, location_handler, handler);
      reader.close();

      return 0;
    }
  } catch (std::exception const& ex) {
    std::clog << "error: " << ex.what() << "\n";
    return -1;
  }
}
