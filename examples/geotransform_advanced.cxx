#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "geotransform/geotransform.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

int main()
{
   using geotransform::deg_to_rad;
   using geotransform::Geotransform;

   const double angle_deg = 15.0;
   const double angle_rad = deg_to_rad(angle_deg);

   // --- Rotated raster from top-left metadata (same entry point as the simple example) ---
   Geotransform<double> rotated;
   rotated.set(1000.0, 2000.0, 2.0, 2.0, angle_rad);

   double x = 0.0;
   double y = 0.0;
   rotated.apply(&x, &y, 10, 25);
   std::cout << std::fixed << std::setprecision(6);
   std::cout << "Rotated grid: pixel (row=10, col=25) maps to (" << x << ", " << y << ")\n";

   int irow = 0;
   int icol = 0;
   rotated.invert(&irow, &icol, x, y);
   std::cout << "invert() recovers pixel (" << irow << ", " << icol << ")\n";

   double ox = 0.0;
   double oy = 0.0;
   double dx = 0.0;
   double dy = 0.0;
   double ang = 0.0;
   rotated.decrypt(&ox, &oy, &dx, &dy, &ang);
   std::cout << "decrypt: origin (" << ox << ", " << oy << "), "
             << "dx=" << dx << ", dy=" << dy << ", angle_rad=" << ang
             << " (~" << (ang * 180.0 / M_PI) << " deg)\n";

   // --- Bottom-left corner + image height: rotation about the lower-left corner ---
   Geotransform<double> from_bottom_left;
   const double x_bl = 500.0;
   const double y_bl = 100.0;
   const int nrows = 240;
   from_bottom_left.set(x_bl, y_bl, 1.0, 1.0, angle_rad, nrows);

   from_bottom_left.apply(&x, &y, 0, 0);
   std::cout << "\nFrom bottom-left corner: top-left pixel (0,0) -> (" << x << ", " << y << ")\n";

   // --- Mixed precision: build in float, promote to double for downstream math ---
   const float gdal_style[6] = {
      440720.0f, 10.0f, 0.0f,
      3'751'320.0f, 0.0f, -10.0f
   };
   Geotransform<float> gt_f(gdal_style);
   Geotransform<double> gt_d(gt_f);
   const double* c = gt_d.get();
   std::cout << "\nPromoted float geotransform to double; gt[0]=" << c[0] << ", gt[5]=" << c[5] << '\n';

   return 0;
}
