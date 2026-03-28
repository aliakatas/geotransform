# Geotransform
Utility to create and update a geotransform array of coefficients.

The geotransform in the context of 2D GIS systems is an affine transformation from an image-based coordinate system (rows,columns or pixels,lines) to real-world coordinates (geographic projections etc).

The motivation for this library is that most common tutorials online (see [GDAL](https://gdal.org/en/latest/tutorials/geotransforms_tut.html) or [Stack Overflow](https://stackoverflow.com/questions/27166739/description-of-parameters-of-gdal-setgeotransform)) do not explain the equations adequately for rotated grids (3rd and 5th elements of the array - usually set to 0).
Additionally, discussion threads ([here](https://gis.stackexchange.com/questions/69715/rotating-rasters-using-gdal-geotransform-and-arcgis-desktop) and [here](https://gis.stackexchange.com/questions/275125/expressing-raster-rotation-with-gdal-geotransform-python)) usually mix the x and y resolution in the array led by the fact that resolution in x- and y-axis is the same for the vast majority of use cases.

This is meant to simplify the creation and use of geotransforms in GIS applications written in C++.

## How to build and test

As usual, build with the following:

```bash
cmake -S . -B build
cmake --build build
```

To run the tests, minimal verbosity with:
```bash
ctest --test-dir build
```

or more verbose with:
```bash
ctest --test-dir build --rerun-failed --output-on-failure
```

To run the examples, execute the following:
```bash
./build/examples/example_geotransform_simple
./build/examples/example_geotransform_advanced
```

