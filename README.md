# osm-split
experimental osm pbf tiler for efficiently breaking planet files into small chunks

### Test

The following command will download necessary sample files, rebuild the binary, and run osm-split against an OSM extract.

```sh
make test
```

### Download

Downloads a small metro extract for testing osm-split

```sh
make chs.osm.pbf
```

### Build

Compiles the osm-split binary.

```sh
make osm-split
```

### Clean

Deletes the osm-split binary.

```sh
make clean
```

### Shortcuts

... [todo: document shortcuts we take in exchange for performance]

### Performance

... [todo: how fast can this tool tile the planet]