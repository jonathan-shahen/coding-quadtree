#pragma once

#define EARTH_RADIUS 6378.1

#include <numbers>

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

        /** @return Approximate Area in m^2. */
        inline float area() const
        {
            // approx area = length * width
            return (
                // Delta degrees -> to radians -> to km -> to m
                abs(lat_max - lat_min) * (std::numbers::pi / 180) * (EARTH_RADIUS) * (1000) *
                // Delta degrees -> to radians -> to km -> to m
                abs(lon_max - lon_min) * (std::numbers::pi / 180) * (EARTH_RADIUS) * (1000));
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
    };

}