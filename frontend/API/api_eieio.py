# import flask_pymongo
from flask_pymongo import PyMongo
from flask import Flask, jsonify
from flask_cors import CORS              # pip install -U flask-cors
from pymongo import MongoClient
from bson.objectid import ObjectId       # install bson before pymongo or it breaks
from bson.json_util import dumps
from heatmaps import *
import logging
import datetime
import subprocess
import ssl
import dns

from heatmaps import *

app = Flask(__name__)
CORS(app)
app.config["MONGO_URI"] = "mongodb://localhost:27017/Land"
app.config['MONGO_DBNAME'] = 'Land'
mongo = PyMongo(app)

client = MongoClient("mongodb+srv://sarahlappin:SDP9@sdp-2ctih.gcp.mongodb.net/test?retryWrites=true&w=majority")
db = client["Land"]
collection = db["Land"]
surv = db["Survey"]



class JSONEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, ObjectId):
            return str(o)
        return json.JSONEncoder.default(self, o)


"""
http://127.0.0.1:5000/getLandInfo/5e68ef5199b1a6270ccd772a/CO
returns: "{\"1\": [0.03412], \"2\": [0.03032], \"3\": [0.04771], \"4\": [0.03121], \"5\": [0.05212], \"6\": [0.03991]}"
"""
@app.route("/getLandInfo/<landID>/<reading_type>", methods=['GET'])
def getLandInfo(landID, reading_type):
    print(landID)

    try:
        land_id = ObjectId(landID)
        print(land_id)
        survey = db.Survey.find({"landID": land_id}).sort("end_time", pymongo.DESCENDING).limit(1)
        print(survey[0]["end_time"])
        survey_id = survey[0]["_id"]
        print(survey_id)
    except Exception as e:
        return ("Error connecting to Survey: {}".format(e)), 404

    try:
        readings = []
        raw_readings = db["Readings"].find({"SurveyID": survey_id})
        for i in raw_readings:
            readings.append(i)

        print(readings)
        if reading_type == "all": # ALL READINGS FROM ALL QUADRANTS
            return JSONEncoder().encode(readings)
    except Exception as e:
        return "Error: {}".format(e), 404

    
    try:
        # print(dumps(readings))
        output = JSONEncoder().encode(getHeatmapValues(readings, reading_type)) 

        print("returning...")
        return dumps(output), 200
    except Exception as e:
        return "ERROR: Failed to run heatmaps.py: {}".format(e), 404
        
"""
http://127.0.0.1:5000/getLandAverage/5e68ef5199b1a6270ccd772a/CO

returns 0.04
"""
@app.route("/getLandAverage/<landID>/<reading_type>", methods=['GET'])
def getLandAverage(landID, reading_type):
    print(landID)

    try:
        land_id = ObjectId(landID)
        print(land_id)
        survey = db.Survey.find({"landID": land_id}).sort("end_time", pymongo.DESCENDING).limit(1)
        print(survey[0]["end_time"])
        survey_id = survey[0]["_id"]
        print(survey_id)
    except Exception as e:
        return ("Error connecting to Survey: {}".format(e)), 404

    try:
        readings = []
        raw_readings = db["Readings"].find({"SurveyID": survey_id})
        for i in raw_readings:
            readings.append(i)

        print(readings)
        if reading_type == "all": # ALL READINGS FROM ALL QUADRANTS
            return JSONEncoder().encode(readings)
    except Exception as e:
        return "Error: {}".format(e), 404

    
    try:
        output = JSONEncoder().encode(getAverageValues(readings, reading_type)) 

        print("returning...")
        return dumps(output), 200
    except Exception as e:
        return "ERROR: Failed to run heatmaps.py: {}".format(e), 404



# @app.route("/addLand", methods=['POST'])
# def getAverageMoisture(ne_latitude, ne_longitude, sw_latatitude, sw_longitude, land_name, user):

#     # Input validation
#     if ne_lat < sw_lat or ne_lon < sw_lon:
#         return "Invalid input: coordinates", 404

#     if name is None:
#         return "Invalid input: no name", 404

#     try:
#         # need to get user id
#         land_json = {ne_lat: ne_latitude, ne_lon: ne_longitude, sw_lat: sw_longitude, sw_lon: sw_longitude, name: land_name}
#         result = db.Land.insert_one(land_json)

#         output = result.inserted_id

#         return output, 200
#     except:
#         return "ERROR: Failed to run heatmaps.py", 500


# # @app.route("/getLand", methods=["GET"])
# # def getLand(userID):
# #     try:
#         # check userID
#         # Get all land ID with corresponding userID


# @app.route("/surveyQuadrants", methods=["POST"])
# def surveyQuadrants(qudrants, landID):
#     if len(qudrants) == 0:
#         return "Invalid input: no qudrants selected", 404
    
#     if landID is None:
#         return "Invalid input: landID required", 404

#     surveyJSON = {LandID: landID, areas: qudrants}

#     try:
#         result = db.Survey.insert_one(surveyJSON)

#         output = result.inserted_id

#         return output, 200
#     except:
#         return "ERROR: unable to connect to MongoDB", 500

    

if __name__ == "__main__":
    app.run()

