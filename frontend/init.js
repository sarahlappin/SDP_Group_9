import express from "express";
import "./db";
import bodyParser from "body-parser";
import { middleware } from "./middlewares";
import routes from "./routes";
import globalRouter from "./routers/globalRouter";
import userRouter from "./routers/userRouter";
import landRouter from "./routers/landRouter";
import "./models/Land";
import "./models/Comment";

const app = express();
const PORT = 4000;

app.set("view engine", "pug");
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use("/static", express.static("static"));
app.use(middleware);
app.use(routes.home, globalRouter);
app.use(routes.users, userRouter);
app.use(routes.lands, landRouter);

const handleListening = () => {
  console.log(`✅ Listening on http://localhost:${PORT}`);
};

app.listen(PORT, handleListening);
