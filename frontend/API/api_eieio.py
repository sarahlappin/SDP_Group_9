# import flask_pymongo
from flask_pymongo import PyMongo
from flask import Flask
from pymongo import MongoClient
from bson.objectid import ObjectId       # install bson before pymongo or it breaks
from bson.json_util import dumps
import datetime
import subprocess
import ssl
import dns

from heatmaps import *

app = Flask(__name__)
app.config["MONGO_URI"] = "mongodb://localhost:27017/Land"
app.config['MONGO_DBNAME'] = 'Land'
mongo = PyMongo(app)

client = MongoClient("mongodb+srv://sarahlappin:SDP9@sdp-2ctih.gcp.mongodb.net/test?retryWrites=true&w=majority")
db = client["Land"]
collection = db["Land"]



@app.route("/getSoilMoisture", methods=['GET'])
def getSoilMoisture(LandID):

    # Get current time
    now = datetime.datetime.now()

    try:
        # Query database to find most recent set of readings
        result = db.Readings.find()

        if result is None:
            return "Error: No readings available for this land", 404

        output = getHeatmapValues(result, "Moisture(%)")

        return output, 200
    except:
        return "ERROR: Failed to run heatmaps.py", 404
        


@app.route("/getCO", methods=['GET'])
def getCO(LandID):

    # Get current time
    now = datetime.now()

    try:
        # Query database to find most recent set of readings
        result = db.Readings.find()

        if result is None:
            return "Error: No readings available for this land", 404

        output = getHeatmapValues(result, "CO(ppmv)")

        return output, 200
    except:
        return "ERROR: Failed to run heatmaps.py", 404


@app.route("/averageMoisture", methods=['GET'])
def getAverageMoisture(LandID):
    
    # Get current time
    now = datetime.now()

    try:
        # Query database to find most recent set of readings
        result = db.Readings.find()

        if result is None:
            return "Error: No readings available for this land", 404

        output = getAverageValues(result, "Moisture(%)")

        return output, 200
    except:
        return "ERROR: Failed to run heatmaps.py", 500


@app.route("/addLand", methods=['POST'])
def addLand(ne_latitude, ne_longitude, sw_latatitude, sw_longitude, land_name, user):

    # Input validation
    if ne_lat < sw_lat or ne_lon < sw_lon:
        return "Invalid input: coordinates", 404

    if name is None:
        return "Invalid input: no name", 404

    try:
        # need to get user id
        land_json = {ne_lat: ne_latitude, ne_lon: ne_longitude, sw_lat: sw_longitude, sw_lon: sw_longitude, name: land_name}
        result = db.Land.insert_one(land_json)

        output = result.objectid

        return output, 200
    except:
        return "ERROR: Failed to run heatmaps.py", 500


@app.route("/getLand", methods=["GET"])
def getLand(userID):
    pass
        # check userID
        # Get all land ID with corresponding userID


@app.route("/surveyQuadrants", methods=["POST"])
def surveyQuadrants(qudrants, landID):
    if len(qudrants) == 0:
        return "Invalid input: no qudrants selected", 404
    
    if landID is None:
        return "Invalid input: landID required", 404

    surveyJSON = {LandID: landID, areas: qudrants}

    try:
        result = db.Survey.insert_one(surveyJSON)

        output = result.objectid

        return output, 200
    except:
        return "ERROR: unable to connect to MongoDB", 500

    
    
if __name__ == "__main__":
    app.run()
