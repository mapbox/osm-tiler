CXX := $(CXX)
CXXFLAGS := -fvisibility=hidden -Wall -Wextra -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wlong-long -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual

osm-tiler: clean
	. ./bootstrap.sh && $(CXX) osm-tiler.cpp -o osm-tiler -std=c++11 $(CXXFLAGS) -lz -lpthread -lboost_program_options -lboost_filesystem -lboost_system;

chs.osm.pbf:
	curl https://s3.amazonaws.com/metro-extracts.mapzen.com/charleston_south-carolina.osm.pbf -o chs.osm.pbf

test: chs.osm.pbf clean osm-tiler
	./osm-tiler chs.osm.pbf -z 9;

clean:
	rm -f osm-tiler; rm -rf output; mkdir output;

clean-mason:
	rm -rf ./mason_packages;
