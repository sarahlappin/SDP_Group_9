import express from "express";
import routes from "../routes";
import { home, getJoin, getLogin, getAbout, getContact } from "../controllers";

const globalRouter = express.Router();

globalRouter.get("/", home);
globalRouter.get(routes.join, getJoin);
globalRouter.get(routes.login, getLogin);
globalRouter.get(routes.about, getAbout);
globalRouter.get(routes.contact, getContact);

export default globalRouter;
