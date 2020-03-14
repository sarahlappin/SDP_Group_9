const express = require("express");
const app = express();

app.set('views', __dirname + '/views');
app.engine('html', require('ejs').renderFile);
app.set('view engine', 'ejs');
app.use(express.static(__dirname + '/public'));
app.get('/', (req, res) => res.render("home.html"));
app.get('/join', (req, res) => res.render("join.html"));
app.get('/login', (req, res) => res.render("login.html"));
app.get('/addLand', (req, res) => res.render("addLand.html"));
app.get('/landDetail', (req, res) => res.render("landDetail.html"));

const handleListening = () =>
  console.log("Listening on: http://localhost:3000");

app.listen(3000, handleListening);
