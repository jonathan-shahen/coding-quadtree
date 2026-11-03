package main

import "math"

// Earth's radius in km
const EARTH_RADIUS = 6378.1

type GeoBoundary struct {
	// Minimum Latitude (in degrees)
	lat_min float32
	// Maximum Latitude (in degrees)
	lat_max float32
	// Minimum Longitude (in degrees)
	lon_min float32
	// Maximum Longitude (in degrees)
	lon_max float32
}

func NewGeoBoundary(lat float32, lon float32, radius float32) GeoBoundary {
	// Converting radius to lie angular radians on the Earth
	angR := float64(radius / EARTH_RADIUS)

	// NOTE: longitude is more complex, see < http: // janmatuschek.de/LatitudeLongitudeBoundingCoordinates>
	deltaLon := math.Asin(math.Sin(angR) / math.Cos(toRadians(float64(lat))))

	return GeoBoundary{
		float32(toDegrees((toRadians(float64(lat)) - angR))),
		float32(toDegrees((toRadians(float64(lat)) + angR))),
		float32(toDegrees((toRadians(float64(lon)) - deltaLon))),
		float32(toDegrees((toRadians(float64(lon)) + deltaLon))),
	}
}

func toRadians(degrees float64) float64 {
	return math.Pi / 180.0 * degrees
}
func toDegrees(radians float64) float64 {
	return 180.0 / math.Pi * radians
}

func (boundary *GeoBoundary) area() float64 {
	var lat_delta = math.Abs(float64(boundary.lat_max - boundary.lat_min))
	var lon_delta = math.Abs(float64(boundary.lon_max - boundary.lon_min))

	// Use the centre latitude for longitude scaling.
	var mid_lat = (boundary.lat_min + boundary.lat_max) / 2.0

	var height_m float64 = toRadians(lat_delta) * EARTH_RADIUS * 1000.0

	var width_m float64 = toRadians(lon_delta) * EARTH_RADIUS * 1000.0 * math.Cos(toRadians(float64(mid_lat)))

	return width_m * height_m
}

// @return TRUE if the lat/lon is within the boundary, FALSE otherwise.
func (boundary *GeoBoundary) inside(lat float32, lon float32) bool {
	return (lat >= boundary.lat_min && lat <= boundary.lat_max) &&
		(lon >= boundary.lon_min && lon <= boundary.lon_max)
}

// @return TRUE if the boundaries intersect, FALSE if only touching or otherwise.
func (boundary *GeoBoundary) intersect(boundary2 *GeoBoundary) bool {
	// Approximation: using Cartesian intersection on a circular body
	return boundary.lat_min < boundary2.lat_max &&
		boundary.lat_max > boundary2.lat_min &&
		boundary.lon_max > boundary2.lon_min &&
		boundary.lon_min < boundary2.lon_max
}

func (boundary *GeoBoundary) nw() GeoBoundary {
	return GeoBoundary{
		lat_min: boundary.lat_min,
		lat_max: boundary.lat_min + (boundary.lat_max-boundary.lat_min)/2.0,
		lon_min: boundary.lon_min,
		lon_max: boundary.lon_min + (boundary.lon_max-boundary.lon_min)/2.0,
	}
}
func (boundary *GeoBoundary) ne() GeoBoundary {
	return GeoBoundary{
		lat_min: boundary.lat_min,
		lat_max: boundary.lat_min + (boundary.lat_max-boundary.lat_min)/2.0,
		lon_min: boundary.lon_min + (boundary.lon_max-boundary.lon_min)/2.0,
		lon_max: boundary.lon_max,
	}
}
func (boundary *GeoBoundary) sw() GeoBoundary {
	return GeoBoundary{
		lat_min: boundary.lat_min + (boundary.lat_max-boundary.lat_min)/2.0,
		lat_max: boundary.lat_max,
		lon_min: boundary.lon_min,
		lon_max: boundary.lon_min + (boundary.lon_max-boundary.lon_min)/2.0,
	}
}
func (boundary *GeoBoundary) se() GeoBoundary {
	return GeoBoundary{
		lat_min: boundary.lat_min + (boundary.lat_max-boundary.lat_min)/2.0,
		lat_max: boundary.lat_max,
		lon_min: boundary.lon_min + (boundary.lon_max-boundary.lon_min)/2.0,
		lon_max: boundary.lon_max,
	}
}
