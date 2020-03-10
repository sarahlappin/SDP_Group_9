import flask_pymongo

from flask_pymongo import PyMongo
from flask import Flask
from pymongo import MongoClient

# app = Flask(__name__)
# app.config["MONGO_URI"] = "mongodb://localhost:27017/myDatabase"
# mongo = PyMongo(app)

client = pymongo.MongoClient("mongodb+srv://sarahlappin:jUjNO4JcikSUKXBa@sdp-2ctih.gcp.mongodb.net/test?retryWrites=true&w=majority")
db = client.test