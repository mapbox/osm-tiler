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
#include <boost/program_options.hpp>
#include "handler.hpp"

#define VERSION "v1.0.0\n"

using namespace boost::program_options;

int main(int argc, char** argv) {
  try {
    options_description desc("\nusage:  osm-tiler [options] <file>");

    desc.add_options()
    ("help,h", "show help")
    ("version,v", "show version number")
    ("zoom,z", value<uint>(), "zoom level of tiles");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if(vm.count("help")) {
      cout << desc;
    } else if(vm.count("version")) {
      cout << VERSION;
    } else {
      uint zoom = 0;

      if(vm.count("zoom")) zoom = vm["zoom"].as<uint>();

      std::string filename = argv[1];

      osmium::io::Reader reader(
        filename,
        osmium::osm_entity_bits::relation | osmium::osm_entity_bits::node | osmium::osm_entity_bits::way
      );

      Handler handler(zoom);
      osmium::apply(reader, handler);
      reader.close();

      return 0;
    }
  } catch (std::exception const& ex) {
    std::clog << "error: " << ex.what() << "\n";
    return -1;
  }
}
