CXX := $(CXX)

osm-split:
	g++ osm-split.cpp -o osm-split -I include -std=c++11 -lz -lpthread;

chs.osm.pbf:
	curl https://s3.amazonaws.com/metro-extracts.mapzen.com/charleston_south-carolina.osm.pbf -o chs.osm.pbf

test:
	 ./osm-split chs.osm.pbf;

clean:
	rm -f osm-split;