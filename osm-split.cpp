#define RAPIDJSON_HAS_STDSTRING 1
#include <exception>
#include <vector>
#include <iostream>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/item_type.hpp>
#include <osmium/io/pbf_input.hpp>
#include "handler.hpp"

int main(int argc, char** argv) {
  std::vector<std::string> args;

  for (int i=1;i<argc;++i) {
    args.push_back(argv[i]);
  }

  if (args.empty()) {
    std::clog << "please pass the path to an osm pbf file\n";
    return -1;
  }

  try {
    std::string filename = args[0];

    osmium::io::Reader reader(
      filename,
      osmium::osm_entity_bits::relation | osmium::osm_entity_bits::node | osmium::osm_entity_bits::way
    );
    Handler handler;
    osmium::apply(reader, handler);
    reader.close();

    std::cout << "Complete" << endl;

  } catch (std::exception const& ex) {
    std::clog << "error: " << ex.what() << "\n";
    return -1;
  }
}
