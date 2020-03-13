#!/usr/bin/env python3
import datetime
import argparse
import pymongo
import json


def getHeatmapValues(results, reading_type):
    box_readings = {1:[],2:[],3:[],4:[],5:[],6:[]}
    for reading in results:
        box = reading["Quadrant"]
        moisture = reading[reading_type]
        print("Box: {}, Reading: {}".format(box, moisture))
        # box = None
        # if lon in range(0,1):
        #     if lat in range(0,1):
        #         box = 1
        #     elif lat in range(1,2):
        #         box = 2
        #     elif lat in range(2,3):
        #         box = 3
        # elif lon in range(1,2):
        #     if lat in range(0,1):
        #         box = 4
        #     elif lat in range(1,2):
        #         box = 5
        #     elif lat in range(2,3):
        #         box = 6

        if box is not None:
            box_readings[box].append(moisture)

    return box_readings

def getAverageValues(results, reading_type):
    values = []
    for reading in results:
        print(reading[reading_type])
        values.append(reading[reading_type])

    if len(values) != 0:
        total = sum(values)
        print("out of for")
        average = total/len(values)
        return round(average, 2)
    else:
        return 0


# def defineQuadrants(numQuadrants, ne_lat, ne_lon, ):
#     if numQuadrants == 6

# def getSoilTrend(result):
    


   




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