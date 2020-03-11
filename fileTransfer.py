import ssl
import pymongo
import pandas as pd
import csv
import json

def sendCSV(csvFileP):

    try:
        # setup and connect to cloud mongoDB
        client = pymongo.MongoClient('mongodb+srv://admin:SDP9@sdp-2ctih.gcp.mongodb.net/test?retryWrites=true&w=majority', ssl=True, ssl_cert_reqs=ssl.CERT_NONE)

        # access to collection and cluster
        db = client["test"]
        collection = db["test"]

        # insert sample data from csv using pandas
        csvFilePath = csvFileP

        df = pd.read_csv(csvFilePath)
        records_ = df.to_dict(orient='records')

        result = collection.insert_many(records_)
        print("Uploaded to MongoDB.")
        return True # lets the user know that the data has been successfully transmitted
    except Exception as e:
        print("Failed to upload the data: " + str(e))
        return False

def markSurveyComplete(id, timeStamp):
    try:
        # setup and connect to cloud mongoDB
        client = pymongo.MongoClient('mongodb+srv://admin:SDP9@sdp-2ctih.gcp.mongodb.net/test?retryWrites=true&w=majority', ssl=True, ssl_cert_reqs=ssl.CERT_NONE)

        # access to collection and cluster
        db = client["Land"]
        collection = db["Survey"]


        surveyDetails = collection.find_one({"surveyID" : id}) # find the given survey
        surveyDetails["endDate"] = timeStamp # update the timestamp
        collection.save({"surveyID" : id}, surveyDetails)
        
    except Exception as e:
        print("Failed to mark the survey as having been completed: " + str(e))

def getNextSurvey():
    # gets the next available survey that does not have a closing date
    try:
        # setup and connect to cloud mongoDB
        client = pymongo.MongoClient('mongodb+srv://admin:SDP9@sdp-2ctih.gcp.mongodb.net/test?retryWrites=true&w=majority', ssl=True, ssl_cert_reqs=ssl.CERT_NONE)

        # access to collection and cluster
        db = client["Land"]
        collection = db["Survey"]


        nextSurveyDetails = collection.find_one({"endDate" : 0}) # find a survey where the end date has not been set (so need to perform survey)

        if nextSurveyDetails == None:
            print("Tried to find a survey but no surveys are still to be completed")
            return False, False, False, False, False, False
        else:
            return nextSurveyDetails["surveyID"], nextSurveyDetails["startLatitude"], nextSurveyDetails["startLongitude"], nextSurveyDetails["endLatitude"], nextSurveyDetails["endLongitude"], nextSurveyDetails["samplingFrequency"]
        
    except Exception as e:
        print("Failed to get the next survey: " + str(e))
        return False, False, False, False, False, False

"""
##Test for single line data 
# json_data = [json.dumps(d) for d in csv.DictReader(open('SDPTestData.csv', encoding='utf-8'))]
# with open(jsonFilePath, "w") as jsonFile:
#    jsonFile.write(json.dumps(json_data, indent=4))
# post = {"TimeStamp": "5e45e3000000000000000000", "latitude": "−3.190260653365977",
#        "longitude": "55.94587364601307", "Moisture": "0.00", "pH": "0.00"}
#collection.insert_one(d)
"""
sendCSV("SDPTestData.csv")
