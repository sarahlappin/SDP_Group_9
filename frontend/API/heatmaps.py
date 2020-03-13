import datetime
import argparse
import pymongo
import json

"""
PARAMS

results = 
[{'_id': ObjectId('5e696194fb7d480a94906177'), 'SurveyID': ObjectId('5e68f59d99b1a6270ccd7730'), 'Quadrant': 1, 'latitude': -3.1904, 'longitude': 55.9458, 'TimeStamp': '25/2/2020 12:18:10PM +00:00', 'CO': 0.03412, 'moisture': 27.1, 'pH': 5.2}, {'_id': ObjectId('5e696194fb7d480a94906178'), 'SurveyID': ObjectId('5e68f59d99b1a6270ccd7730'), 'Quadrant': 2, 'latitude': -3.1901, 'longitude': 55.9502, 'TimeStamp': '25/2/2020 12:19:02PM +00:00', 'CO': 0.03032, 'moisture': 34.1, 'pH': 5.7}, {'_id': ObjectId('5e696194fb7d480a94906179'), 'SurveyID': ObjectId('5e68f59d99b1a6270ccd7730'), 'Quadrant': 3, 'latitude': -3.19, 'longitude': 55.949, 'TimeStamp': '25/2/2020 12:19:30PM +00:00', 'CO': 0.04771, 'moisture': 32.2, 'pH': 3.1}, {'_id': ObjectId('5e696194fb7d480a9490617a'), 'SurveyID': ObjectId('5e68f59d99b1a6270ccd7730'), 'Quadrant': 4, 'latitude': -3.19026, 'longitude': 55.9456, 'TimeStamp': '28/2/2020 12:17:55PM +00:00', 'CO': 0.03121, 'moisture': 33.7, 'pH': 5.7}, {'_id': ObjectId('5e696194fb7d480a9490617b'), 'SurveyID': ObjectId('5e68f59d99b1a6270ccd7730'), 'Quadrant': 5, 'latitude': -3.1903, 'longitude': 55.94588, 'TimeStamp': '28/2/2020 12:18:10PM +00:00', 'CO': 0.05212, 'moisture': 35.2, 'pH': 5.8}, {'_id': ObjectId('5e696194fb7d480a9490617c'), 'SurveyID': ObjectId('5e68f59d99b1a6270ccd7730'), 'Quadrant': 6, 'latitude': -3.1904, 'longitude': 55.94592, 'TimeStamp': '28/2/2020 12:18:35PM +00:00', 'CO': 0.03991, 'moisture': 32.2, 'pH': 5.0}]

reading_type = "moisture"
"""
def getHeatmapValues(results, reading_type):
    box_readings = {1:[],2:[],3:[],4:[],5:[],6:[]}
    for reading in results:
        print(reading)
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
            box_readings[int(box)].append(moisture)

    print(box_readings)

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
