import argparse
import pathlib
import typing
import csv
import math
import numpy

from dataclasses import dataclass
from enum import Enum, auto


class MapFeature(Enum):
    '''MapFeature: Features that occur on maps (streets, downtown areas, etc) that can help make the data more interesting.'''

    '''Evenly, and randomly, spread point around a center point, up to a limited radius'''
    EVEN_SPREAD = auto()
    '''Same as EVEN_SPREAD, except the `extra` parameter is used to exclude an inner diameter. Looks like a donut.'''
    EVEN_SPREAD_DONUT = auto()
    '''Place all points on the same center point, to act like a mall/shared address.'''
    MALL = auto()
    '''Place all points evenly along a line that crosses the center point, angle is random, street length limited by radius'''
    STREET = auto()


@dataclass
class DataSplit:
    """Class for indicating how many data points are required for a split."""
    feature: MapFeature
    num_points: int
    '''tuple(lat,lon)'''
    center_point: tuple[float, float]
    '''Radius, in kilometres'''
    radius: float
    extra: any = None

# Earth's radius (km)
# NOTE: this is an approximate at the equator, but the polar radius is 6356.8 km (0.33% percent diff)
EARTH_RADIUS = 6378.1

def bounding_box(
        center_lat: float,
        center_lon: float,
        radius: float
) -> typing.Dict[str,float]:
    '''
    Returns:
        Bounding box in DEGREES
    '''
    # Converting radius to lie angular radians on the Earth
    ang_r = radius / EARTH_RADIUS

    # NOTE: longitude is more complex, see <http://janmatuschek.de/LatitudeLongitudeBoundingCoordinates>
    delta_lon = math.asin(math.sin(ang_r) / math.cos(center_lat))

    # Bounding Box
    return {
        "lat_min": math.degrees(center_lat - ang_r),
        "lat_max": math.degrees(center_lat + ang_r),
        "lon_min": math.degrees(center_lon - delta_lon),
        "lon_max": math.degrees(center_lon + delta_lon),
    }

def gen_points(
        split: DataSplit,
        id_start: int,
        split_index: int = 0,
        include_split: bool = False
) -> typing.List[str]:
    '''
    Returns:
        List of CSV formatted strings
    '''
    points = []
    center_lat = math.radians(split.center_point[0])
    center_lon = math.radians(split.center_point[1])

    box = bounding_box(center_lat, center_lon, split.radius)

    if split.feature == MapFeature.EVEN_SPREAD:
        rand_lat = numpy.random.uniform(box['lat_min'], box['lat_max'], size=split.num_points)
        rand_lon = numpy.random.uniform(box['lon_min'], box['lon_max'], size=split.num_points)
        
        for i in range(split.num_points):
            if include_split:
                s = f'{id_start +i},{rand_lat[i]},{rand_lon[i]},{split_index}\n'
            else:
                s = f'{id_start +i},{rand_lat[i]},{rand_lon[i]}\n'
            points.append(s)
    elif split.feature == MapFeature.EVEN_SPREAD_DONUT:
        donut_hole_box = bounding_box(center_lat, center_lon, split.extra)
        print(box)
        print(donut_hole_box)

        # used to ensure exact num_points generated
        a_top = int(round(1/3 * split.num_points))
        a_bottom = a_top
        b_left = int(round(1/6 * split.num_points))
        b_right = split.num_points - a_top - a_bottom - b_left

        rand_lat = [
            # a_top
            *numpy.random.uniform(donut_hole_box['lat_min'], box['lat_min'], size=a_top),
            # a_bottom
            *numpy.random.uniform(box['lat_max'], donut_hole_box['lat_max'], size=a_bottom),
            # b_left
            *numpy.random.uniform(box['lat_min'], box['lat_max'], size=b_left),
            # b_right
            *numpy.random.uniform(box['lat_min'], box['lat_max'], size=b_right),
        ]
        rand_lon = [
            # a_top
            *numpy.random.uniform(donut_hole_box['lon_min'], donut_hole_box['lon_max'], size=a_top),
            # a_bottom
            *numpy.random.uniform(donut_hole_box['lon_min'], donut_hole_box['lon_max'], size=a_bottom),
            # b_left
            *numpy.random.uniform(donut_hole_box['lon_min'], box['lon_min'], size=b_left),
            # b_right
            *numpy.random.uniform(box['lon_max'], donut_hole_box['lon_max'], size=b_right),
        ]
    
        for i in range(split.num_points):
            if include_split:
                s = f'{id_start +i},{rand_lat[i]},{rand_lon[i]},{split_index}\n'
            else:
                s = f'{id_start +i},{rand_lat[i]},{rand_lon[i]}\n'
            points.append(s)
    else:
        print(f'FAILURE(unimplemented): unknown map feature; {split.feature}')

    return points



def split_data(
        in_points: str,
        in_unit_test: str,
        num_points: int,
) -> typing.List[DataSplit]:
    '''
    split_data: takes the input files and creates the required chunks for points to create

    Parameters:
        in_points (str) - filename of CSV file holding places to surround with generated data
        in_unit_test (str) - filename of CSV file holding unit test data to embed into generated data
        num_points (int) - Number of points (lat,long,id) to create

    Return:
        list[DataSplit] - ordered list that contains a total of num_points
    '''

    splits = []
    # Unit test should be first, this ensures easily testable incrementing ids
    with open(in_unit_test) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            lat = float(row['Latitude'])
            long = float(row['Longitude'])
            radius = float(row['R_km'])
            inside_points = int(row['num businesses'])
            outside_points = int(row['num extra businesses'])

            inside_split = DataSplit(feature=MapFeature.EVEN_SPREAD, num_points=inside_points, center_point=[
                                     lat, long], radius=radius)
            outside_split = DataSplit(feature=MapFeature.EVEN_SPREAD_DONUT, num_points=outside_points, center_point=[
                                      lat, long], radius=2*radius, extra=radius)

            splits.append(inside_split)
            splits.append(outside_split)

    # Evenly split each in_points with the rest of the points requested

    return splits


def data_gen(
        out_filename: str,
        in_points: str,
        in_unit_test: str,
        num_points: int,
        include_split: bool
):
    '''
    data_gen: generates data and writes it to a file

    Parameters:
        out_filename (str) - filename to write the generated CSV data
        in_points (str) - filename of CSV file holding places to surround with generated data
        in_unit_test (str) - filename of CSV file holding unit test data to embed into generated data
        num_points (int) - Number of points (lat,long,id) to create
    '''
    splits = split_data(in_points, in_unit_test, num_points)

    print(f'Created {len(splits)} splits')

    with open(out_filename, "w", encoding="utf-8", buffering=1024 * 1024) as f:
        # CSV Header
        if include_split:
            f.write("id,lat,lon,split\n")
        else:
            f.write("id,lat,lon\n")


        id_start = 0
        for i in range(len(splits)):
            points = []
            split = splits[i]
            print(f'Working on Split: {split}')

            if split.feature == MapFeature.EVEN_SPREAD:
                points = gen_points(split, id_start, i, include_split)
            elif split.feature == MapFeature.EVEN_SPREAD_DONUT:
                points = gen_points(split, id_start, i, include_split)
            else:
                print(f'FAILURE(unimplemented): unable to generate points for this split; {split}')
            
            id_start += len(points)
            f.writelines(points)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-o", "--out_filename", type=pathlib.Path,
                        default="./data.csv", help="filename to write the generated CSV data")
    parser.add_argument("-p", "--in_points", type=pathlib.Path, default="./most_popular_cities.csv",
                        help="filename of CSV file holding places to surround with generated data")
    parser.add_argument("-u", "--in_unit_test", type=pathlib.Path, default="./unit_test.csv",
                        help="filename of CSV file holding unit test data to embed into generated data")
    parser.add_argument("-n", "--num_points", type=int, default=1_000_000_000,
                        help="Number of points (lat,long,id) to create")
    parser.add_argument("--include_split", action='store_true', default=False,
                        help="Include an extra column, storing the `split`. Useful for debugging.")
    args = parser.parse_args()

    data_gen(args.out_filename, args.in_points,
             args.in_unit_test, args.num_points, args.include_split)
