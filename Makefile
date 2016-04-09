CXX := $(CXX)

osm-split: clean
	g++ osm-split.cpp -o osm-split -I include -std=c++11 -lz -lpthread -lboost_program_options -lboost_filesystem -lboost_system;

chs.osm.pbf:
	curl https://s3.amazonaws.com/metro-extracts.mapzen.com/charleston_south-carolina.osm.pbf -o chs.osm.pbf

test: chs.osm.pbf clean osm-split node-cache
	./osm-split chs.osm.pbf -g -z 7;

node-cache:
	touch node-cache;

clean:
	rm -f osm-split node-cache; rm -rf output; mkdir output;