#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "geotransform/geotransform_operations.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <cmath>

using geotransform::calculate_rotation_matrix;
using geotransform::convert_to_meteorological_dir_deg;
using geotransform::convert_to_meteorological_dir_rad;
using geotransform::deg_to_rad;
using geotransform::get_0_2pi_rad;
using geotransform::get_0_360_deg;
using geotransform::rad_to_deg;
using geotransform::rotate_point;
using geotransform::rotate_point_about;
using geotransform::to_compass_angle_deg;
using geotransform::to_compass_angle_rad;
using geotransform::translate_point;

namespace
{
   constexpr double eps = 1e-12;
}

TEST_CASE("deg_to_rad and rad_to_deg")
{
   CHECK(deg_to_rad(0.0) == doctest::Approx(0.0).epsilon(eps));
   CHECK(deg_to_rad(90.0) == doctest::Approx(M_PI / 2.0).epsilon(eps));
   CHECK(deg_to_rad(-45.0) == doctest::Approx(-M_PI / 4.0).epsilon(eps));

   CHECK(rad_to_deg(0.0L) == doctest::Approx(0.0).epsilon(eps));
   CHECK(rad_to_deg(static_cast<long double>(M_PI)) == doctest::Approx(180.0L).epsilon(1e-9));

   const double d = 37.5;
   CHECK(rad_to_deg(deg_to_rad(d)) == doctest::Approx(d).epsilon(1e-14));

   const float f = 12.25f;
   CHECK(rad_to_deg(deg_to_rad(f)) == doctest::Approx(f).epsilon(1e-6f));
}

TEST_CASE("get_0_360_deg")
{
   CHECK(get_0_360_deg(0.0) == doctest::Approx(0.0));
   CHECK(get_0_360_deg(360.0) == doctest::Approx(360.0));
   CHECK(get_0_360_deg(361.0) == doctest::Approx(1.0));
   CHECK(get_0_360_deg(-1.0) == doctest::Approx(359.0));
   CHECK(get_0_360_deg(-361.0) == doctest::Approx(359.0));
}

TEST_CASE("get_0_2pi_rad")
{
   CHECK(get_0_2pi_rad(0.0) == doctest::Approx(0.0).epsilon(eps));
   CHECK(get_0_2pi_rad(2.0 * M_PI) == doctest::Approx(0.0).epsilon(1e-14));
   CHECK(get_0_2pi_rad(2.0 * M_PI + 0.5) == doctest::Approx(0.5).epsilon(1e-14));
   CHECK(get_0_2pi_rad(-M_PI) == doctest::Approx(M_PI).epsilon(1e-14));
}

TEST_CASE("to_compass_angle_deg")
{
   CHECK(to_compass_angle_deg(0.0) == doctest::Approx(90.0));
   CHECK(to_compass_angle_deg(90.0) == doctest::Approx(0.0));
   CHECK(to_compass_angle_deg(-90.0) == doctest::Approx(180.0));
}

TEST_CASE("to_compass_angle_rad")
{
   CHECK(to_compass_angle_rad(0.0) == doctest::Approx(M_PI / 2.0).epsilon(eps));
   CHECK(to_compass_angle_rad(M_PI / 2.0) == doctest::Approx(0.0).epsilon(eps));
}

TEST_CASE("convert_to_meteorological_dir_deg")
{
   CHECK(convert_to_meteorological_dir_deg(0.0) == doctest::Approx(180.0));
   // 180 + 180 == 360; get_0_360_deg keeps 360 (does not normalize to 0).
   CHECK(convert_to_meteorological_dir_deg(180.0) == doctest::Approx(360.0));
   CHECK(convert_to_meteorological_dir_deg(-90.0) == doctest::Approx(90.0));
}

TEST_CASE("convert_to_meteorological_dir_rad")
{
   CHECK(convert_to_meteorological_dir_rad(0.0) == doctest::Approx(M_PI).epsilon(eps));
   CHECK(convert_to_meteorological_dir_rad(M_PI) == doctest::Approx(0.0).epsilon(eps));
}

TEST_CASE("calculate_rotation_matrix")
{
   double rm[4];
   calculate_rotation_matrix(0.0, rm);
   CHECK(rm[0] == doctest::Approx(1.0).epsilon(eps));
   CHECK(rm[1] == doctest::Approx(0.0).epsilon(eps));
   CHECK(rm[2] == doctest::Approx(0.0).epsilon(eps));
   CHECK(rm[3] == doctest::Approx(1.0).epsilon(eps));

   calculate_rotation_matrix(M_PI / 2.0, rm);
   CHECK(rm[0] == doctest::Approx(0.0).epsilon(1e-14));
   CHECK(rm[1] == doctest::Approx(-1.0).epsilon(1e-14));
   CHECK(rm[2] == doctest::Approx(1.0).epsilon(1e-14));
   CHECK(rm[3] == doctest::Approx(0.0).epsilon(1e-14));
}

TEST_CASE("translate_point")
{
   double x = 1.5;
   double y = -2.25;
   translate_point(&x, &y, 3.0, 0.5);
   CHECK(x == doctest::Approx(4.5));
   CHECK(y == doctest::Approx(-1.75));
}

TEST_CASE("rotate_point")
{
   double rm[4];
   calculate_rotation_matrix(M_PI / 2.0, rm);

   double x = 1.0;
   double y = 0.0;
   rotate_point(&x, &y, rm);
   CHECK(x == doctest::Approx(0.0).epsilon(1e-14));
   CHECK(y == doctest::Approx(1.0).epsilon(1e-14));
}

TEST_CASE("rotate_point_about")
{
   double rm[4];
   calculate_rotation_matrix(M_PI, rm);

   double x = 2.0;
   double y = 0.0;
   rotate_point_about(&x, &y, rm, 1.0, 0.0);
   CHECK(x == doctest::Approx(0.0).epsilon(1e-14));
   CHECK(y == doctest::Approx(0.0).epsilon(1e-14));
}
