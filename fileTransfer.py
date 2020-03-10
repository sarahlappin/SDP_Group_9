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
