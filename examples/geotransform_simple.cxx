#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "geotransform/geotransform.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

/**
 * Minimal GDAL-style workflow: build a north-up affine transform from the top-left
 * corner and pixel size, then map a few pixel centers to projected coordinates.
 */
int main()
{
   using geotransform::Geotransform;

   Geotransform<double> gt;

   // Projected coordinates at the top-left corner of the raster; square 10 m pixels;
   // angle 0 means columns align with +x and rows with -y (north-up).
   const double x_top_left = 500000.0;
   const double y_top_left = 4'100'000.0;
   const double pixel_size_m = 10.0;

   gt.set(x_top_left, y_top_left, pixel_size_m, pixel_size_m, 0.0);

   const double* coeff = gt.get();
   std::cout << std::fixed << std::setprecision(3);
   std::cout << "GDAL geotransform coefficients:\n";
   for (int i = 0; i < 6; ++i)
      std::cout << "  gt[" << i << "] = " << coeff[i] << '\n';

   std::cout << "\nPixel center coordinates (row, col) -> (x, y):\n";

   for (int row : { 0, 100, 1000 }) {
      for (int col : { 0, 50, 200 }) {
         double x = 0.0;
         double y = 0.0;
         gt.apply(&x, &y, row, col);
         std::cout << "  (" << row << ", " << col << ") -> (" << x << ", " << y << ")\n";
      }
   }

   return 0;
}
