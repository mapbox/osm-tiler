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
    ("zoom,z", value<uint>(), "zoom level of tiles")
    ("input,i", value<string>(), "input OpenStreetMap file")
    ("output,o", value<string>(), "output directory");

    positional_options_description p;
    p.add("input", 1);
    variables_map vm;

    auto parser = command_line_parser(argc, argv).options(desc).positional(p);
    store(parser.run(), vm);

    if(vm.count("help")) {
      cout << desc;
    } else if(vm.count("version")) {
      cout << VERSION;
    } else {
      uint zoom = 0;
      string output;
      string input;

      if(vm.count("zoom")) zoom = vm["zoom"].as<uint>();
      else {
        cout << "Error: A zoom level is required" << endl;
        return 1;
      }

      if(vm.count("output")) output = vm["output"].as<string>();
      else {
        cout << "Error: An output directory is required" << endl;
        return 1;
      }

      if(vm.count("input")) input = vm["input"].as<string>();
      else {
        cout << "Error: An OpenStreetMap input file is required" << endl;
        return 1;
      }

      osmium::io::Reader reader(
        input,
        osmium::osm_entity_bits::relation | osmium::osm_entity_bits::node | osmium::osm_entity_bits::way
      );

      Handler handler(zoom, output);
      osmium::apply(reader, handler);
      reader.close();

      return 0;
    }
  } catch (std::exception const& ex) {
    std::clog << "error: " << ex.what() << "\n";
    return -1;
  }
}
