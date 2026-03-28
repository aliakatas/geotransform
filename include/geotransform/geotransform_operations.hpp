#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <cmath>

#define ONE_EIGHTY_DEG           180.0

namespace geotransform
{
   /**
    * @brief Translates an angle in degrees to radians.
    * 
    * Does not apply any limitations to the input or output values
    * 
    * @tparam T supports float, double and long double
    * @param d [in] Angle in degrees.
    * @return Angle in radians.
    */
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, T>::type
   deg_to_rad(const T d)
   {
      return M_PI * d / ONE_EIGHTY_DEG;
   }

   /**
    * @brief Translates and angle in radians to degrees.
    * 
    * Does not apply any limitations to the input or output values
    * 
    * @tparam T Supports float, double and long double.
    * @param r [in] Angle in radians.
    * @return Angle in degrees.
    */
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, T>::type
   rad_to_deg(const T r)
   {
      return ONE_EIGHTY_DEG * r / M_PI;
   }

   /**
    * @brief Limits an angle in degrees between the values of 0 and 360.
    * 
    * @tparam T supports float, double and long double
    * @param d [in] Angle in degrees.
    * @return Angle adjusted in range [0, 360]
    */
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, T>::type
   get_0_360_deg(T d)
   {
      while (d < 0) {
         d += 360;
      }
      while (d > 360) {
         d -= 360;
      }
      return d;
   }

   /**
    * @brief Limits an angle in radians between the values of 0 and 2*PI.
    * 
    * @tparam T supports float, double and long double
    * @param r [in] Angle in radians.
    * @return Angle adjusted in range [0, 2*PI]
    */
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, T>::type
   get_0_2pi_rad(T r)
   {
      while (r < 0) {
         r += 2 * M_PI;
      }
      while (r >= 2 * M_PI) {
         r -= 2 * M_PI;
      }
      return r;
   }

   /**
    * @brief Converts an angle in degrees to follow the compass convention.
    * That is 0 deg = North - up. A positive angle rotates clockwise.
    * 
    * @tparam T Supports float, double and long double.
    * @param d [in] Angle in degrees.
    * @return Angle converted to the 0-up system.
    */
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, T>::type
   to_compass_angle_deg(T d)
   {
      d = 90 - d;
      return get_0_360_deg(d);
   }

   /**
    * @brief Converts an angle in radians to follow the compass convention.
    * That is 0 rad = North - up. A positive angle rotates clockwise.
    * 
    * @tparam T Supports float, double and long double.
    * @param r [in] Angle in radians.
    * @return Angle converted to the 0-up system.
    */
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, T>::type
   to_compass_angle_rad(T r)
   {
      r = (M_PI / 2) - r;
      return get_0_2pi_rad(r);
   }

   /**
    * @brief Converts an angle of a vector in degrees to follow the meteorological convention.
    * That is 0 deg = North - up. A positive angle rotates clockwise. 
    * Angle indicates the point of origin
    * 
    * @tparam T Supports float, double and long double.
    * @param r [in] Angle in degrees.
    * @return Angle converted to the meteorological convention.
    */
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, T>::type
   convert_to_meteorological_dir_deg(T d)
   {
      d += 180;
      return get_0_360_deg(d);
   }

   /**
    * @brief Converts an angle of a vector in radians to follow the meteorological convention.
    * That is 0 rad = North - up. A positive angle rotates clockwise. 
    * Angle indicates the point of origin
    * 
    * @tparam T Supports float, double and long double.
    * @param r [in] Angle in radians.
    * @return Angle converted to the meteorological convention.
    */
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, T>::type
   convert_to_meteorological_dir_rad(T r)
   {
      r += M_PI;
      return get_0_2pi_rad(r);
   }

   /**
    * @brief Calculates the 2D rotation matrix.
    * 
    * @tparam T Supports float, double and long double.
    * @param angle_rad [in] The angle in radians.
    * @param rotation_matrix [out] The rotation matrix. Must have at least 4 elements. Memory allocation must happen before calling this
    */
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, void>::type
   calculate_rotation_matrix(T angle_rad, T* rotation_matrix)
   {
      rotation_matrix[0] = std::cos(angle_rad);
      rotation_matrix[1] = -std::sin(angle_rad);
      rotation_matrix[2] = std::sin(angle_rad);
      rotation_matrix[3] = std::cos(angle_rad);
   }

   /**
    * @brief Translates a 2D point in 2D.
    * 
    * It does not perform any sanity checks on the input values.
    * 
    * @tparam T Supports float, double and long double.
    * @param x [inout] The x-coordinate of the point.
    * @param y [inout] They y-coordinate of the point.
    * @param dx [in] The displacement in x-direction.
    * @param dy [in] The displacement in y-direction.
    */
   template <typename T>
   typename std::enable_if<std::is_floating_point<T>::value, void>::type
   translate_point(T* x, T* y, const T dx, const T dy)
   {
      *x += dx;
      *y += dy;
   }

   /**
    * @brief Rotates a 2D point given the rotation matrix.
    * The point is rotated about the axes origin.
    * 
    * It does not perform any sanity checks on the input values.
    * 
    * @tparam T Supports float, double and long double.
    * @tparam RM Supports float, double and long double.
    * @param x [inout] The x-coordinate of the point.
    * @param y [inout] The y-coordinate of the point.
    * @param rotation_matrix [in] The rotation matrix to be applied.
    */
   template <typename T, typename RM>
   typename std::enable_if<std::is_floating_point<T>::value && std::is_floating_point<RM>::value, void>::type
   rotate_point(T* x, T* y, const RM* rotation_matrix)
   {
      T xtemp = *x;
      T ytemp = *y;
      *x = static_cast<T>(rotation_matrix[0] * xtemp + rotation_matrix[1] * ytemp);
      *y = static_cast<T>(rotation_matrix[2] * xtemp + rotation_matrix[3] * ytemp);
   }

   /**
    * @brief Rotates a 2D point about another point in 2D space.
    * 
    * It does not perform any sanity checks on the input values.
    * 
    * @tparam T Supports float, double and long double.
    * @tparam RM Supports float, double and long double.
    * @tparam Tref Supports float, double and long double.
    * @param x [inout] The x-coordinate of the point.
    * @param y [inout] The y-coordinate of the point.
    * @param rot_mat [in] The rotation matrix to be applied.
    * @param xref [in] The x-coordinate of the point to rotate about.
    * @param yref [in] The y-coordinate of the point to rotate about.
    */
   template <typename T, typename RM, typename Tref>
   typename std::enable_if<std::is_floating_point<T>::value && 
      std::is_floating_point<RM>::value &&
      std::is_floating_point<Tref>::value, void>::type
   rotate_point_about(T* x, T* y, const RM* rot_mat, const Tref xref, const Tref yref)
   {
      translate_point(x, y, -xref, -yref);
      rotate_point(x, y, rot_mat);
      translate_point(x, y, xref, yref);
   }

}
