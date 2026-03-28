#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "geotransform/geotransform.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <cmath>
#include <stdexcept>

namespace
{
   constexpr double eps = 1e-12;
}

using geotransform::Geotransform;

TEST_CASE("default construction is not initialized")
{
   Geotransform<double> gt;
   CHECK_THROWS_AS(gt.get(), std::runtime_error);

   double x = 0.0;
   double y = 0.0;
   CHECK_THROWS_AS(gt.apply(&x, &y, 0, 0), std::runtime_error);

   int irow = 0;
   int icol = 0;
   CHECK_THROWS_AS(gt.invert(&irow, &icol, 0.0, 0.0), std::runtime_error);

   double xtl = 0.0;
   double ytl = 0.0;
   double dx = 0.0;
   double dy = 0.0;
   double ang = 0.0;
   CHECK_THROWS_AS(gt.decrypt_geotransform(&xtl, &ytl, &dx, &dy, &ang), std::runtime_error);
}

TEST_CASE("constructor from array")
{
   const double arr[6] = { 10.0, 2.0, 3.0, 40.0, 4.0, -5.0 };
   Geotransform<double> gt(arr);
   const double* g = gt.get();
   for (int i = 0; i < 6; ++i)
      CHECK(g[i] == doctest::Approx(arr[i]));
}

TEST_CASE("set from array")
{
   Geotransform<double> gt;
   const double arr[6] = { 1.0, 1.0, 0.0, 2.0, 0.0, -1.0 };
   gt.set(arr);
   const double* g = gt.get();
   for (int i = 0; i < 6; ++i)
      CHECK(g[i] == doctest::Approx(arr[i]));
}

TEST_CASE("set top-left north-up matches GDAL coefficients")
{
   Geotransform<double> gt;
   gt.set(100.0, 200.0, 10.0, 10.0, 0.0);
   const double* g = gt.get();
   CHECK(g[0] == doctest::Approx(100.0).epsilon(eps));
   CHECK(g[1] == doctest::Approx(10.0).epsilon(eps));
   CHECK(g[2] == doctest::Approx(0.0).epsilon(eps));
   CHECK(g[3] == doctest::Approx(200.0).epsilon(eps));
   CHECK(g[4] == doctest::Approx(0.0).epsilon(eps));
   CHECK(g[5] == doctest::Approx(-10.0).epsilon(eps));
}

TEST_CASE("apply uses GDAL row-major indexing")
{
   Geotransform<double> gt;
   gt.set(100.0, 200.0, 10.0, 10.0, 0.0);
   double x = 0.0;
   double y = 0.0;
   gt.apply(&x, &y, 3, 2);
   CHECK(x == doctest::Approx(120.0));
   CHECK(y == doctest::Approx(170.0));
}

TEST_CASE("invert round-trips apply for north-up")
{
   Geotransform<double> gt;
   gt.set(100.0, 200.0, 10.0, 10.0, 0.0);
   double x = 0.0;
   double y = 0.0;
   gt.apply(&x, &y, 3, 2);
   int irow = 0;
   int icol = 0;
   gt.invert(&irow, &icol, x, y);
   CHECK(irow == 3);
   CHECK(icol == 2);
}

TEST_CASE("invert with size_t indices")
{
   Geotransform<double> gt;
   gt.set(0.0, 0.0, 1.0, 1.0, 0.0);
   double x = 0.0;
   double y = 0.0;
   gt.apply(&x, &y, 4, 7);
   size_t irow = 0;
   size_t icol = 0;
   gt.invert(&irow, &icol, x, y);
   CHECK(irow == 4u);
   CHECK(icol == 7u);
}

TEST_CASE("invert round-trips apply for rotated geotransform")
{
   Geotransform<double> gt;
   const double angle = M_PI / 6.0;
   gt.set(0.0, 0.0, 1.0, 1.0, angle);
   double x = 0.0;
   double y = 0.0;
   gt.apply(&x, &y, 2, 5);
   int irow = 0;
   int icol = 0;
   gt.invert(&irow, &icol, x, y);
   CHECK(irow == 2);
   CHECK(icol == 5);
}

TEST_CASE("decrypt_geotransform recovers north-up parameters")
{
   Geotransform<double> gt;
   gt.set(100.0, 200.0, 10.0, 10.0, 0.0);
   double xtl = 0.0;
   double ytl = 0.0;
   double dx = 0.0;
   double dy = 0.0;
   double ang = 0.0;
   gt.decrypt_geotransform(&xtl, &ytl, &dx, &dy, &ang);
   CHECK(xtl == doctest::Approx(100.0));
   CHECK(ytl == doctest::Approx(200.0));
   CHECK(dx == doctest::Approx(10.0));
   CHECK(dy == doctest::Approx(10.0));
   CHECK(ang == doctest::Approx(0.0).epsilon(1e-9));
}

TEST_CASE("decrypt_geotransform round-trip for non-zero rotation")
{
   const double x0 = 12.5;
   const double y0 = -3.25;
   const double dx_in = 2.0;
   const double dy_in = 3.0;
   const double angle = M_PI / 7.0;

   Geotransform<double> gt;
   gt.set(x0, y0, dx_in, dy_in, angle);

   double xtl = 0.0;
   double ytl = 0.0;
   double dx = 0.0;
   double dy = 0.0;
   double ang = 0.0;
   gt.decrypt_geotransform(&xtl, &ytl, &dx, &dy, &ang);

   CHECK(xtl == doctest::Approx(x0).epsilon(1e-12));
   CHECK(ytl == doctest::Approx(y0).epsilon(1e-12));
   CHECK(dx == doctest::Approx(dx_in).epsilon(1e-12));
   CHECK(dy == doctest::Approx(dy_in).epsilon(1e-12));
   CHECK(ang == doctest::Approx(angle).epsilon(1e-12));
}

TEST_CASE("copy constructor duplicates coefficients and initialization")
{
   const double arr[6] = { 5.0, 1.0, 0.25, 10.0, -0.5, -2.0 };
   Geotransform<double> a(arr);
   Geotransform<double> b(a);
   const double* ga = a.get();
   const double* gb = b.get();
   for (int i = 0; i < 6; ++i)
      CHECK(gb[i] == doctest::Approx(ga[i]));
}

TEST_CASE("copy constructor from different coefficient type")
{
   const float arr[6] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
   Geotransform<float> f(arr);
   Geotransform<double> d(f);
   const double* g = d.get();
   for (int i = 0; i < 6; ++i)
      CHECK(g[i] == doctest::Approx(static_cast<double>(arr[i])));
}

TEST_CASE("set bottom-left corner north-up")
{
   Geotransform<double> gt;
   gt.set(100.0, 50.0, 1.0, 1.0, 0.0, 100);
   const double* g = gt.get();
   CHECK(g[0] == doctest::Approx(100.0));
   CHECK(g[3] == doctest::Approx(-50.0));
   CHECK(g[1] == doctest::Approx(1.0));
   CHECK(g[2] == doctest::Approx(0.0));
   CHECK(g[4] == doctest::Approx(0.0));
   CHECK(g[5] == doctest::Approx(-1.0));
}
