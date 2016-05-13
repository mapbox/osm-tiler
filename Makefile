BUILDTYPE ?= Release

CXX := $(CXX)
CXXFLAGS := $(CXXFLAGS) -std=c++11 -fvisibility=hidden
LDFLAGS := $(LDFLAGS) -lz -lpthread -lboost_program_options -lboost_filesystem -lboost_system
WARNING_FLAGS := -Wall -Wextra -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wlong-long -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual

MASON_HOME := ./mason_packages/.link

RELEASE_FLAGS := -O3 -DNDEBUG
DEBUG_FLAGS := -O0 -DDEBUG -fno-inline-functions -fno-omit-frame-pointer

ifeq ($(BUILDTYPE),Release)
	FINAL_FLAGS := -g $(WARNING_FLAGS) $(RELEASE_FLAGS)
else
	FINAL_FLAGS := -g $(WARNING_FLAGS) $(DEBUG_FLAGS)
endif

all: osm-tiler

mason_packages:
	./bootstrap.sh

osm-tiler: clean mason_packages
	$(CXX) osm-tiler.cpp -o osm-tiler -I$(MASON_HOME)/include -L$(MASON_HOME)/lib $(CXXFLAGS) $(FINAL_FLAGS) $(LDFLAGS);

chs.osm.pbf:
	curl https://s3.amazonaws.com/metro-extracts.mapzen.com/charleston_south-carolina.osm.pbf -o chs.osm.pbf

test: chs.osm.pbf clean osm-tiler
	./osm-tiler chs.osm.pbf -z 9 -o ./output;

clean:
	rm -f osm-tiler; rm -rf output; mkdir output;

clean-mason:
	rm -rf ./mason_packages;
