import express from "express";
import { middleware } from "./middlewares";
import router from "./routers";

const app = express();
const PORT = 4000;

app.set("view engine", "pug");
app.use("/static", express.static("static"))
app.use(middleware);
app.use("/", router);

const handleListening = () => {
  console.log(`Listening on http://localhost:${PORT}`);
};

app.listen(PORT, handleListening);
