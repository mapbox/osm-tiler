CXX := $(CXX)
CXXFLAGS := -fvisibility=hidden -Wall -Wextra -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wlong-long -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual

osm-split: clean
	. ./bootstrap.sh && $(CXX) osm-split.cpp -o osm-split -std=c++11 $(CXXFLAGS) -lz -lpthread -lboost_program_options -lboost_filesystem -lboost_system;

chs.osm.pbf:
	curl https://s3.amazonaws.com/metro-extracts.mapzen.com/charleston_south-carolina.osm.pbf -o chs.osm.pbf

test: chs.osm.pbf clean osm-split node-cache
	./osm-split chs.osm.pbf -z 7;

node-cache:
	touch node-cache;

clean:
	rm -f osm-split node-cache; rm -rf output; mkdir output;

clean-mason:
	rm -rf ./mason_packages;
