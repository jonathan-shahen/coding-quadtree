import argparse
import pathlib
import typing
import csv


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
    center_point: tuple[float, float]
    '''Radius, in kilometres'''
    radius: float
    extra: any = None


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

            inside_split = DataSplit(feature=MapFeature.EVEN_SPREAD, num_points=inside_points, center_point=[lat,long],radius=radius)
            outside_split = DataSplit(feature=MapFeature.EVEN_SPREAD_DONUT, num_points=outside_points, center_point=[lat,long],radius=2*radius, extra=radius)

            splits.append(inside_split)
            splits.append(outside_split)

    # Evenly split each in_points with the rest of the points requested

    return splits


def data_gen(
        out_filename: str,
        in_points: str,
        in_unit_test: str,
        num_points: int,
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
    print(splits)


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
    args = parser.parse_args()

    data_gen(args.out_filename, args.in_points,
             args.in_unit_test, args.num_points)
