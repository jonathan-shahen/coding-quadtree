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
