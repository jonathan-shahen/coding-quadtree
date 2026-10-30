const EARTH_RADIUS: f32 = 6378.1;

#[derive(Debug)]
pub struct GeoBoundary {
    /// Minimum Latitude (in degrees)
    pub lat_min: f32,
    /// Maximum Latitude (in degrees)
    pub lat_max: f32,
    /// Minimum Longitude (in degrees)
    pub lon_min: f32,
    /// Maximum Longitude (in degrees)
    pub lon_max: f32,
}

impl GeoBoundary {
    pub fn from_center_point(lat: f32, lon: f32, radius: f32) -> Self {
        // Converting radius to lie angular radians on the Earth
        let ang_r = radius / EARTH_RADIUS;

        // NOTE: longitude is more complex, see < http: // janmatuschek.de/LatitudeLongitudeBoundingCoordinates>
        let delta_lon = (ang_r.sin() / lat.to_radians().cos()).asin();

        Self {
            lat_min: (lat.to_radians() - ang_r).to_degrees(),
            lat_max: (lat.to_radians() + ang_r).to_degrees(),
            lon_min: (lon.to_radians() - delta_lon).to_degrees(),
            lon_max: (lon.to_radians() + delta_lon).to_degrees(),
        }
    }

    /// @return TRUE if the lat/lon is within the boundary, FALSE otherwise.
    pub fn inside(&self, lat: f32, lon: f32) -> bool {
        (lat >= self.lat_min && lat <= self.lat_max) && (lon >= self.lon_min && lon <= self.lon_max)
    }

    /// @return TRUE if the boundaries intersect, FALSE if only touching or otherwise.
    pub fn intersect(&self, boundary: &Self) -> bool {
        // Approximation: using Cartesian intersection on a circular body
        self.lat_min < boundary.lat_max
            && self.lat_max > boundary.lat_min
            && self.lon_max > boundary.lon_min
            && self.lon_min < boundary.lon_max
    }

    /// Better approximate, longitude needed scaling
    pub fn area(&self) -> f64 {
        let lat_delta = (self.lat_max - self.lat_min).abs();
        let lon_delta = (self.lon_max - self.lon_min).abs();

        // Use the centre latitude for longitude scaling.
        let mid_lat = (self.lat_min + self.lat_max) / 2.0;

        let height_m = lat_delta.to_radians() * EARTH_RADIUS * 1000.0;

        let width_m = lon_delta.to_radians() * EARTH_RADIUS * 1000.0 * mid_lat.to_radians().cos();

        return (width_m as f64) * (height_m as f64);
    }

    pub fn nw(&self) -> Self {
        Self {
            lat_min: self.lat_min,
            lat_max: self.lat_min + (self.lat_max - self.lat_min) / 2.0,
            lon_min: self.lon_min,
            lon_max: self.lon_min + (self.lon_max - self.lon_min) / 2.0,
        }
    }
    pub fn ne(&self) -> Self {
        Self {
            lat_min: self.lat_min,
            lat_max: self.lat_min + (self.lat_max - self.lat_min) / 2.0,
            lon_min: self.lon_min + (self.lon_max - self.lon_min) / 2.0,
            lon_max: self.lon_max,
        }
    }
    pub fn sw(&self) -> Self {
        Self {
            lat_min: self.lat_min + (self.lat_max - self.lat_min) / 2.0,
            lat_max: self.lat_max,
            lon_min: self.lon_min,
            lon_max: self.lon_min + (self.lon_max - self.lon_min) / 2.0,
        }
    }
    pub fn se(&self) -> Self {
        Self {
            lat_min: self.lat_min + (self.lat_max - self.lat_min) / 2.0,
            lat_max: self.lat_max,
            lon_min: self.lon_min + (self.lon_max - self.lon_min) / 2.0,
            lon_max: self.lon_max,
        }
    }
}
