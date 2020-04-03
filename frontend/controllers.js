import Land from "./models/Land";
import routes from "./routes";

export const home = async (req, res) => {
  try {
    const lands = await Land.find({}).sort({ _id: -1 });
    res.render("home", { pageTitle: "Home", lands });
  } catch (error) {
    console.log(error);
    res.render("home", { pageTitle: "Home", lands: [] });
  }
};

export const getAbout = (req, res) =>
  res.render("about", { pageTitle: "About" });
export const getContact = (req, res) =>
  res.render("contact", { pageTitle: "Contact" });

export const getJoin = (req, res) => res.render("join", { pageTitle: "Join" });
export const getLogin = (req, res) =>
  res.render("login", { pageTitle: "Login" });

export const users = (req, res) => res.render("users", { pageTitle: "Users" });
export const userDetail = (req, res) =>
  res.render("userDetail", { pageTitle: "User Detail" });

export const getAddLand = (req, res) =>
  res.render("addLand", { pageTitle: "Add Land" });

export const postAddLand = async (req, res) => {
  const {
    body: {
      name,
      pointALong,
      pointALat,
      pointBLong,
      pointBLat,
      userCO,
      userMoist,
      userPH
    }
  } = req;
  const newLand = await Land.create({
    name,
    pointALong,
    pointALat,
    pointBLong,
    pointBLat,
    userCO,
    userMoist,
    userPH,
    surveyCO: "0",
    surveyMoist: "0",
    surveyPH: "0",
    accumulatedCO: 0,
    accumulatedMoist: 0,
    accumulatedPH: 0,
    numberOfSurveys: 0,
    surveyDate: "Surveying hasn't been done yet"
  });
  res.redirect(routes.landDetail(newLand.id));
};

export const startSurvey = async (req, res) => {
  const {
    params: { id }
  } = req;
  try {
    const coValues = ["3.5", "4.5", "5.5", "6.0", "7.0", "8.5", "9.0"];
    const moistValues = ["65", "70", "75", "80", "85", "90", "95"];
    const phValues = ["5.5", "6.0", "6.5", "7.0", "7.5", "8.0", "8.5", "9.0"];
    const coValue = coValues[(coValues.length * Math.random()) | 0];
    const moistValue = moistValues[(moistValues.length * Math.random()) | 0];
    const phValue = phValues[(phValues.length * Math.random()) | 0];
    const prevLand = await Land.findById(id);
    const land = await Land.findOneAndUpdate(
      { _id: id },
      {
        surveyCO: coValue,
        surveyMoist: moistValue,
        surveyPH: phValue,
        accumulatedCO: prevLand.accumulatedCO + Number(coValue),
        accumulatedMoist: prevLand.accumulatedMoist + Number(moistValue),
        accumulatedPH: prevLand.accumulatedPH + Number(phValue),
        numberOfSurveys: prevLand.numberOfSurveys + 1,
        surveyDate: new Date().toLocaleString()
      },
      { new: true }
    );
    res.render("landDetail", {
      pageTitle: land.name,
      name: land.name,
      pointALong: land.pointALong,
      pointALat: land.pointALat,
      pointBLong: land.pointBLong,
      pointBLat: land.pointBLat,
      userCO: land.userCO,
      userMoist: land.userMoist,
      userPH: land.userPH,
      surveyCO: land.surveyCO,
      surveyMoist: land.surveyMoist,
      surveyPH: land.surveyPH,
      accumulatedCO: land.accumulatedCO,
      accumulatedMoist: land.accumulatedMoist,
      accumulatedPH: land.accumulatedPH,
      surveyDate: land.surveyDate,
      numberOfSurveys: land.numberOfSurveys,
      land
    });
  } catch (error) {
    console.log(error);
    res.redirect(routes.home);
  }
};

export const landDetail = async (req, res) => {
  const {
    params: { id }
  } = req;
  try {
    const land = await Land.findById(id);
    res.render("landDetail", {
      pageTitle: land.name,
      name: land.name,
      pointALong: land.pointALong,
      pointALat: land.pointALat,
      pointBLong: land.pointBLong,
      pointBLat: land.pointBLat,
      userCO: land.userCO,
      userMoist: land.userMoist,
      userPH: land.userPH,
      surveyCO: land.surveyCO,
      surveyMoist: land.surveyMoist,
      surveyPH: land.surveyPH,
      accumulatedCO: land.accumulatedCO,
      accumulatedMoist: land.accumulatedMoist,
      accumulatedPH: land.accumulatedPH,
      surveyDate: land.surveyDate,
      numberOfSurveys: land.numberOfSurveys,
      land
    });
  } catch (error) {
    console.log(error);
    res.redirect(routes.home);
  }
};

export const deleteLand = async (req, res) => {
  const {
    params: { id }
  } = req;

  try {
    await Land.findOneAndRemove({ _id: id });
    res.redirect(routes.home);
  } catch (error) {
    res.redirect(routes.home);
  }
};
