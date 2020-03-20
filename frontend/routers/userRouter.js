import express from "express";
import routes from "../routes";
import {
    users,
    userDetail
  } from "../controllers";
  
  const userRouter = express.Router();
  
  userRouter.get("/", users);
  userRouter.get(routes.userDetail(), userDetail);
  
  export default userRouter;
  