#pragma once

#define EARTH_RADIUS 6378.1

#include <numbers>
#include <iostream>
#include <cmath>

namespace QuadTree
{
    class GeoBoundary
    {
    public:
        /** Min/Max values, stored in degrees */
        float lat_min, lat_max, lon_min, lon_max;
        GeoBoundary(float lat_min, float lat_max, float lon_min, float lon_max)
        {
            this->lat_min = lat_min;
            this->lat_max = lat_max;
            this->lon_min = lon_min;
            this->lon_max = lon_max;
        }

        /** @return TRUE if the lat/lon is within the boundary, FALSE otherwise. */
        inline bool inside(float lat, float lon) const
        {
            return (
                (lat >= lat_min && lat <= lat_max) &&
                (lon >= lon_min && lon <= lon_max));
        }

        /** Better approximate, longitude needed scaling */
        inline double area() const
        {
            constexpr double DEG_TO_RAD = std::numbers::pi / 180.0;

            const double lat_delta = std::abs(lat_max - lat_min);
            const double lon_delta = std::abs(lon_max - lon_min);

            // Use the centre latitude for longitude scaling.
            const double mid_lat = (lat_min + lat_max) / 2.0;

            const double height_m =
                lat_delta * DEG_TO_RAD * EARTH_RADIUS * 1000.0;

            const double width_m =
                lon_delta * DEG_TO_RAD * EARTH_RADIUS * 1000.0 *
                std::cos(mid_lat * DEG_TO_RAD);

            return width_m * height_m;
        }

        inline GeoBoundary nw() const
        {
            return GeoBoundary(lat_min, lat_min + (lat_max - lat_min) / 2, lon_min, lon_min + (lon_max - lon_min) / 2);
        }
        inline GeoBoundary ne() const
        {
            return GeoBoundary(lat_min, lat_min + (lat_max - lat_min) / 2, lon_min + (lon_max - lon_min) / 2, lon_max);
        }
        inline GeoBoundary sw() const
        {
            return GeoBoundary(lat_min + (lat_max - lat_min) / 2, lat_max, lon_min, lon_min + (lon_max - lon_min) / 2);
        }
        inline GeoBoundary se() const
        {
            return GeoBoundary(lat_min + (lat_max - lat_min) / 2, lat_max, lon_min + (lon_max - lon_min) / 2, lon_max);
        }

        inline std::string to_string() const
        {
            return std::format("<GeoBoundary: lat:[{},{}], lon:[{},{}]>", lat_min, lat_max, lon_min, lon_max);
        }
    };

    inline std::ostream &operator<<(std::ostream &os, const GeoBoundary &boundary)
    {
        return os << boundary.to_string();
    }
}