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
  createdAt: {
    type: Date,
    default: Date.now
  }
});

const model = mongoose.model("Land", LandSchema);

export default model;
