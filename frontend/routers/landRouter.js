import express from "express";
import routes from "../routes";

import { getAddLand, postAddLand, landDetail, deleteLand } from "../controllers";

const landRouter = express.Router();

landRouter.get(routes.addLand, getAddLand);
landRouter.post(routes.addLand, postAddLand);
landRouter.get(routes.landDetail(), landDetail);
landRouter.get(routes.deleteLand(), deleteLand);

export default landRouter;


