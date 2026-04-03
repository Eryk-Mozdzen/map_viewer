# Map Viewer for OSM and BHMW

- [Open Street Map](https://www.openstreetmap.org/#map=7/52.018/19.137)
- [Biuro Hydrograficzne Marynarki Wojennej](https://bhmw-wms.wp.mil.pl/)

### Dependencies

```bash
sudo apt install -y \
    make \
    cmake \
    libglfw3-dev
```

### Build

```bash
mkdir -p build
cd build
cmake ..
make
./map_viewer
```
