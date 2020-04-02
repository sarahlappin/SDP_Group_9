import mongoose from "mongoose";

const LandSchema = new mongoose.Schema({
  name: {
    type: String,
    required: "Name of land is required"
  },
  pointALong: {
    type: String,
    required: "Point A Longitude is required"
  },
  pointALat: {
    type: String,
    required: "Point A Latitude is required"
  },
  pointBLong: {
    type: String,
    required: "Point B Longitude is required"
  },
  pointBLat: {
    type: String,
    required: "Point B Latitude is required"
  },
  surveyCO: {
    type: String,
    required: "Last survey CO value is required"
  },
  surveyMoist: {
    type: String,
    required: "Last survey soil moisture value is required"
  },
  surveyPH: {
    type: String,
    required: "Last survey pH value is required"
  },
  accumulatedCO: {
    type: Number,
    required: "Accumulated survey CO value is required"
  },
  accumulatedMoist: {
    type: Number,
    required: "Accumulated survey soil moisture value is required"
  },
  accumulatedPH: {
    type: Number,
    required: "Accumulated survey pH value is required"
  },
  userCO: {
    type: String,
    required: "Desired CO value is required"
  },
  userMoist: {
    type: String,
    required: "Desired soil moisture value is required"
  },
  userPH: {
    type: String,
    required: "Desired pH value is required"
  },
  numberOfSurveys: {
    type: Number,
    required: "Number of surveys done is required"
  },
  surveyDate: {
    type: String
  }
});

const model = mongoose.model("Land", LandSchema);

export default model;
