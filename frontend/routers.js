import express from "express";
import {
  home,
  getJoin,
  getLogin,
  getAddLand,
  getLandDetail
} from "./controllers";

const router = express.Router();

router.get("/", home);
router.get("/join", getJoin);
router.get("/login", getLogin);
router.get("/addLand", getAddLand);
router.get("/landDetail", getLandDetail);

export default router;
