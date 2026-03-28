# Geotransform
Utility to create and update a geotransform array of coefficients.

The geotransform in the context of 2D GIS systems is an affine transformation from an image-based coordinate system (rows,columns or pixels,lines) to real-world coordinates (geographic projections etc).

The motivation for this library is that most common tutorials online (see [GDAL](https://gdal.org/en/latest/tutorials/geotransforms_tut.html) or [Stack Overflow](https://stackoverflow.com/questions/27166739/description-of-parameters-of-gdal-setgeotransform)) do not explain the equations adequately for rotated grids (3rd and 5th elements of the array - usually set to 0).
Additionally, discussion threads ([here](https://gis.stackexchange.com/questions/69715/rotating-rasters-using-gdal-geotransform-and-arcgis-desktop) and [here](https://gis.stackexchange.com/questions/275125/expressing-raster-rotation-with-gdal-geotransform-python)) usually mix the x and y resolution in the array led by the fact that resolution in x- and y-axis is the same for the vast majority of use cases.

This is meant to simplify the creation and use of geotransforms in GIS applications written in C++.

## What's included

- **`Geotransform`** (`include/geotransform/geotransform.hpp`): a small template class (typically `float`, `double`, or `long double`) that stores the six GDAL-style coefficients, builds them from corner metadata or an existing array, maps pixel indices to world coordinates (`apply`), performs the inverse (`invert`), and can recover rotation and pixel size (`decrypt`). The affine model matches [GDAL's geotransform tutorial](https://gdal.org/en/latest/tutorials/geotransforms_tut.html).
- **Utilities** (`include/geotransform/geotransform_operations.hpp`): helpers such as degree/radian conversion, angle wrapping, compass/meteorological angle conversions, 2D rotation matrices, and point rotation/translation—used by `Geotransform` and usable on their own.

The library is **header-only**: add the `include` directory to your compiler's include path and `#include` the headers you need; no separate linking step is required beyond the C++ standard library and `<cmath>`.

## Requirements

- **CMake** 3.14 or newer (for the bundled build and tests).
- A **C++17** compiler.
- **Tests** use [doctest](https://github.com/doctest/doctest) from `extern/doctest` (vendored in this repository).

## Layout

| Path | Role |
|------|------|
| `include/geotransform/` | Public headers (`geotransform.hpp`, `geotransform_operations.hpp`) |
| `examples/` | Sample programs (`example_geotransform_simple`, `example_geotransform_advanced`) |
| `tests/` | Unit tests (doctest) |
| `extern/doctest/` | Test framework (not needed for embedding the library in your app) |

## Using the library in another project

Point your target's include directories at this repository's `include` folder (or install/copy the headers), then:

```cpp
#include "geotransform/geotransform.hpp"
```

On **MSVC**, defining `_USE_MATH_DEFINES` before including system headers (or via `target_compile_definitions`) ensures `M_PI` and related constants are available where `<cmath>` is used.

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

On Windows, the executables are typically under `build\examples\Release\` or `build\examples\Debug\` depending on the generator and configuration.

## License

This project is released under the MIT License; see [LICENSE](LICENSE).
