# osm-tiler

osm-tiler is an [.osm.pbf](http://wiki.openstreetmap.org/wiki/PBF_Format) tiler for efficiently breaking [OpenStreetMap planet files](http://planet.openstreetmap.org/) into smaller chunks. It is ideal for distributed processing of OpenStreetMap data, since it groups spatially related data into separate files that do not need to be loaded into memory at once.

### Use

```sh
osm-tiler planet.osm.pbf -z 7
```

### Build

osm-tiler uses https://github.com/mason to fetch dependencies to ensure a uniform development and build environment. 

```sh
make osm-tiler
```

### Test

The following command will download necessary sample files, rebuild the binary, and run osm-tiler against an OSM extract.

```sh
make test
```

### Download

Downloads a small metro extract for testing osm-tiler.

```sh
make chs.osm.pbf
```

### Clean

Deletes the osm-tiler binary.

```sh
make clean
```
