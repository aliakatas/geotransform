#pragma once

#include "geotransform_operations.hpp"

#include <cmath>
#include <stdexcept>
#include <type_traits>
#include <string>

namespace geotransform
{
   const std::string ERROR_MESSAGE_GEOTRANSFORM_NOT_INITIALIZED{ "Geotransform object not initialized." };

   template <typename data_t>
   class Geotransform
   {
   public:

      Geotransform() = default;
      ~Geotransform() = default;

      /**
       * @brief Creates a geotransform object from an existing geotransform array.
       * 
       * @tparam T The data type. Supports float, double and long double.
       * @param gt_array The geotransform array with 6 elements to copy from.
       */
      template <typename T>
      Geotransform(const T gt_array[6]) : 
         p_gt{ static_cast<data_t>(gt_array[0]), static_cast<data_t>(gt_array[1]), static_cast<data_t>(gt_array[2]), static_cast<data_t>(gt_array[3]), static_cast<data_t>(gt_array[4]), static_cast<data_t>(gt_array[5]) },
         m_initialized(true) 
      {}

      /**
       * @brief Copy constructor.
       *
       * @param other The geotransform to copy from.
       */
      template <typename T>
      Geotransform(const Geotransform<T>& other) :
         p_gt{ static_cast<data_t>(other.p_gt[0]), 
            static_cast<data_t>(other.p_gt[1]), 
            static_cast<data_t>(other.p_gt[2]), 
            static_cast<data_t>(other.p_gt[3]), 
            static_cast<data_t>(other.p_gt[4]), 
            static_cast<data_t>(other.p_gt[5]) },
         m_initialized(other.m_initialized)
      {}

      /**
       * @brief Sets the geotransform array by copying from an existing array.
       * 
       * @tparam T The data type. Supports float, double and long double.
       * @param gt_array The geotransform array with 6 elements to copy from.
       */
      template <typename T>
      typename std::enable_if<std::is_floating_point<T>::value, void>::type
      set(const T gt_array[6])
      {
         for (size_t i = 0; i < 6; ++i)
            p_gt[i] = static_cast<data_t>(gt_array[i]);
            
         m_initialized = true;
      }

      /**
       * @brief Sets the geotransform array according to the input parameters 
       * considering the top left corner of the grid/image.
       * 
       * @tparam T Supports float, double and long double.
       * @param x_top_left [in] x-coordinate of the top left corner of the image.
       * @param y_top_left [in] y-coordinate of the top left corner of the image.
       * @param dx [in] x-coordinate of the image resolution. For rotated images, it corresponds to the "horizontal" analysis if the image was north-up. Must be positive.
       * @param dy [in] y-coordinate of the image resolution. For rotated images, it corresponds to the "vertical" analysis if the image was north-up. Must be positive.
       * @param angle_rad [in] Angle of rotation of the image in radians.
       */
      template <typename T>
      typename std::enable_if<std::is_floating_point<T>::value, void>::type
      set(
         const T x_top_left, const T y_top_left, 
         const T dx, const T dy, const T angle_rad)
      {
         _set_geotransform(x_top_left, y_top_left, dx, dy, angle_rad);
         m_initialized = true;
      }

      /**
       * @brief Sets the geotransform array according to the input parameters 
       * considering the bottom left corner of the grid/image.
       * 
       * It assumes the grid/image is rotated about its lower left corner.
       * 
       * @tparam T Supports float, double and long double.
       * @param x_bottom_left [in] x-coordinate of the bottom left corner of the image.
       * @param y_bottom_left [in] y-coordinate of the bottom left corner of the image.
       * @param dx [in] x-coordinate of the image resolution. For rotated images, it corresponds to the "horizontal" analysis if the image was north-up. Must be positive.
       * @param dy [in] y-coordinate of the image resolution. For rotated images, it corresponds to the "vertical" analysis if the image was north-up. Must be positive.
       * @param angle_rad [in] Angle of rotation of the image in radians.
       * @param height [in] The height of the image/grid in number of pixels/rows.
       */
      template <typename T, typename index_t>
      typename std::enable_if<std::is_floating_point<T>::value && 
      std::is_integral<index_t>::value, void>::type
      set(
         const T x_bottom_left, const T y_bottom_left, 
         const T dx, const T dy, const T angle_rad, const index_t height)
      {
         data_t rotation_matrix[4];
         calculate_rotation_matrix(angle_rad, rotation_matrix);
         
         T x = x_bottom_left;
         T y = y_bottom_left - height * dy; // height is correct as we need to account for the size of the last cell. Otherwise, it would have been height - 1.
         rotate_point_about(&x, &y, rotation_matrix, x_bottom_left, y_bottom_left);

         _set_geotransform(x, y, dx, dy, angle_rad);
         m_initialized = true;
      }

      /**
       * @brief Gets the geotransform array.
       * 
       * @return Pointer to the geotransform array.
       * @throws std::runtime_error if the geotransform object is not initialized.
       */
      const data_t* get() const noexcept(false)
      { 
         if (!m_initialized)
            throw std::runtime_error(std::string(__FUNCTION__) + 
               std::string(": ") + ERROR_MESSAGE_GEOTRANSFORM_NOT_INITIALIZED);
            
         return p_gt; 
      }

      /**
       * @brief Applies the geotransform to calculate the x and y
       * coordinates of an image according to 
       * https://gdal.org/en/latest/tutorials/geotransforms_tut.html.
       * 
       * It does not perform any sanity checks on the input values.
       * 
       * @tparam T Supports float, double and long double.
       * @tparam index_t Supports any data type that is numerical or can be converted (implicitly or not) to a numerical value.
       * @param x [out] x-coordinate of the point (pixel).
       * @param y [out] y-coordinate of the point (pixel).
       * @param irow [in] The row index to use for the pixel.
       * @param icol [in] The column index to use for the pixel.
       * @throws std::runtime_error if the geotransform object is not initialized.
       */
      template <typename T, typename index_t>
      typename std::enable_if<std::is_floating_point<T>::value && 
         std::is_arithmetic<index_t>::value, void>::type
      apply(T* x, T* y, 
         const index_t irow, const index_t icol) const
      {
         if (!m_initialized)
            throw std::runtime_error(std::string(__FUNCTION__) + 
               std::string(": ") + ERROR_MESSAGE_GEOTRANSFORM_NOT_INITIALIZED);

         *x = static_cast<T>(p_gt[0] + icol * p_gt[1] + irow * p_gt[2]);
         *y = static_cast<T>(p_gt[3] + icol * p_gt[4] + irow * p_gt[5]);
      }

      /**
       * @brief Applies the inverse geotransform to retrieve the 
       * row and column indexes for a pixel given the x and y 
       * coordinates according to https://gdal.org/en/latest/tutorials/geotransforms_tut.html.
       * 
       * The result is rounded to the nearest integer value. No bound or overflow checks.
       * 
       * It does not perform any sanity checks on the input values.
       * 
       * @tparam index_t Supports int, long, long long and size_t.
       * @tparam T Supports float, double and long double.
       * @param irow [out] Row index.
       * @param icol [out] Column index.
       * @param x [in] x-coordinate of the pixel.
       * @param y [in] y-coordinate of the pixel.
       * @param geotransform [in] The geotransform array.
       * @throws std::runtime_error if the geotransform object is not initialized.
       */
      template <typename index_t, typename T>
      typename std::enable_if<
         (std::is_same<index_t, int>::value || std::is_same<index_t, long>::value || std::is_same<index_t, long long>::value || std::is_same<index_t, size_t>::value) &&
         std::is_floating_point<T>::value, void>::type
      invert(index_t* irow, index_t* icol, 
         const T x, const T y) const
      {
         if (!m_initialized)
            throw std::runtime_error(std::string(__FUNCTION__) + 
               std::string(": ") + ERROR_MESSAGE_GEOTRANSFORM_NOT_INITIALIZED);

         T Dx = static_cast<T>(x - p_gt[0]);
         T Dy = static_cast<T>(y - p_gt[3]);

         T det = static_cast<T>(p_gt[1] * p_gt[5] - p_gt[2] * p_gt[4]);
         T det_icol = static_cast<T>(Dx * p_gt[5] - p_gt[2] * Dy);
         T det_irow = static_cast<T>(p_gt[1] * Dy - Dx * p_gt[4]);

         *irow = static_cast<index_t>(std::round(det_irow / det));
         *icol = static_cast<index_t>(std::round(det_icol / det));
      }

      /**
       * @brief Retrieve the information used to build the geotransform
       * according to https://gdal.org/en/latest/tutorials/geotransforms_tut.html.
       * 
       * It does not perform any sanity checks on the input values.
       * 
       * @param x_top_left [out] x-coordinate of the top left corner of the image.
       * @param y_top_left [out] y-coordinate of the top left corner of the image.
       * @param dx [out] x-coordinate of the image resolution. For rotated images, it corresponds to the "horizontal" analysis if the image was north-up.
       * @param dy [out] y-coordinate of the image resolution. For rotated images, it corresponds to the "vertical" analysis if the image was north-up.
       * @param angle_rad [out] Angle of rotation of the image in radians.
       * @throws std::runtime_error if the geotransform object is not initialized.
       */
      template <typename T>
      typename std::enable_if<std::is_floating_point<T>::value, void>::type
      decrypt_geotransform(T* x_top_left, T* y_top_left,
         T* dx, T* dy, T* angle_rad) const
      {
         if (!m_initialized)
            throw std::runtime_error(std::string(__FUNCTION__) + 
               std::string(": ") + ERROR_MESSAGE_GEOTRANSFORM_NOT_INITIALIZED);

         // Get the origin
         *x_top_left = static_cast<T>(p_gt[0]);
         *y_top_left = static_cast<T>(p_gt[3]);

         // Get the angle as well as the resolution
         *angle_rad = std::atan(p_gt[4] / p_gt[1]);
         if (std::abs(*angle_rad) > 1.e-6) {
            *dx = p_gt[1] / std::cos(*angle_rad);
            *dy = -p_gt[5] / std::cos(*angle_rad);
         }
         else {
            *dx = p_gt[1];
            *dy = -p_gt[5];
         }
      }

   private:
      bool m_initialized = false;
      data_t p_gt[6];
      
      /**
       * @brief Creates the geotransform array according to
       * https://gdal.org/en/latest/tutorials/geotransforms_tut.html.
       * 
       * It does not perform any sanity checks on the input values.
       * 
       * @tparam T Supports float, double and long double.
       * @param x_top_left [in] x-coordinate of the top left corner of the image.
       * @param y_top_left [in] y-coordinate of the top left corner of the image.
       * @param dx [in] x-coordinate of the image resolution. For rotated images, it corresponds to the "horizontal" analysis if the image was north-up. Must be positive.
       * @param dy [in] y-coordinate of the image resolution. For rotated images, it corresponds to the "vertical" analysis if the image was north-up. Must be positive.
       * @param angle_rad [in] Angle of rotation of the image in radians.
       */
      template <typename T>
      typename std::enable_if<std::is_floating_point<T>::value, void>::type
      _set_geotransform(
         const T x_top_left, const T y_top_left, 
         const T dx, const T dy, const T angle_rad)
      {
         p_gt[0] = static_cast<data_t>(x_top_left);
         p_gt[1] = static_cast<data_t>(dx * std::cos(angle_rad));
         p_gt[2] = static_cast<data_t>(dy * std::sin(angle_rad));
         p_gt[3] = static_cast<data_t>(y_top_left);
         p_gt[4] = static_cast<data_t>(dx * std::sin(angle_rad));
         p_gt[5] = static_cast<data_t>(-dy * std::cos(angle_rad));
      }

   };

}