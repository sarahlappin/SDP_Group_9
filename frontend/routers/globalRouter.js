import express from "express";
import routes from "../routes";
import {
  home,
  getJoin,
  getLogin,
} from "../controllers";

const globalRouter = express.Router();

globalRouter.get("/", home);
globalRouter.get(routes.join, getJoin);
globalRouter.get(routes.login, getLogin);

export default globalRouter;
