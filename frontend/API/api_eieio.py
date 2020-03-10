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
def getSoilMoisture():

    # Get current time
    now = datetime.now()

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
def getSoilMoisture():

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
def getAverageMoisture():
    
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
        return "ERROR: Failed to run heatmaps.py", 404

    
if __name__ == "__main__":
    app.run()