# Geotransform
Utility to create and update a geotransform array of coefficients.

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

