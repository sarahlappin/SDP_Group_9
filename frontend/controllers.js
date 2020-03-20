export const home = (req, res) => res.render("home", { pageTitle: "Home" });
export const getJoin = (req, res) => res.render("join", { pageTitle: "Join" });
export const getLogin = (req, res) => res.render("login", { pageTitle: "Login" });
export const getAddLand = (req, res) => res.render("addLand", { pageTitle: "Add Land" });
export const getLandDetail = (req, res) => res.render("landDetail", { pageTitle: "Land Detail" });
