#!/usr/bin/env python3
import datetime
import argparse
import pymongo
import json


def getHeatmapValues(results, type):
    box_readings = {}
    for reading in results:
        lon = reading["Longitude"]
        lat = reading["Latitude"]

        box = None
        if lon in range(0,1):
            if lat in range(0,1):
                box = 1
            elif lat in range(1,2):
                box = 2
            elif lat in range(2,3):
                box = 3
        elif lon in range(1,2):
            if lat in range(0,1):
                box = 4
            elif lat in range(1,2):
                box = 5
            elif lat in range(2,3):
                box = 6

        if box is not None:
            box_readings[box].append(reading[type])

    return box_readings

def getAverageValues(results, type):
    values = []
    for reading in results:
        values.append(reading[type])

    average = sum(values)/len(values)
    return average


   




# TODO - add subprocess?

# if __name__ == "__main__":
#     parser = argparse.ArgumentParser(description='Get heatmaps for given input.')
#     parser.addArgument("--moisture", help="Collect Soil Moisture Data", dest="Soil Moisture" )
#     parser.addArgument("--CO", help="Collect Carbon Monoxide Data", dest="Carbon Monoxide" )
#     parser.addArgument("--PH", help="Collect pH Data", dest="pH" )

#     args = parser.parse_args()

#     if args.moisture:
#         return soilMoisture()
#     else:
#         soilMoisture() # TODO - add others