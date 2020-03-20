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
    body: { name, pointALong, pointALat, pointBLong, pointBLat }
  } = req;
  const newLand = await Land.create({
    name,
    pointALong,
    pointALat,
    pointBLong,
    pointBLat
  });
  res.redirect(routes.landDetail(newLand.id));
};

export const landDetail = async (req, res) => {
  const {
    params: { id }
  } = req;
  try {
    const land = await Land.findById(id);
    res.render("landDetail", { pageTitle: land.title, land });
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
