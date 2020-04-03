# SDP GROUP 9 - eEIEIO

### Front-end Server Set-up:
1.  Ensure you have MongoDB installed: [https://www.mongodb.com/download-center/community]
2.  Install Node.js: [https://nodejs.org/en/download/]
3.  Run `cd <SDP directory>/frontend`
4.  Run `npm install`
5.  Run `mongod`
    1.  If this gives you an error, manually create the `/data/db` folder in your root directory by running`mkdir -p /data/db`. 
    2.  Try running `mongod` again
    3.  If you receieve a further error regarding permissions, run: `sudo mongod`. You may need to include the path to the db folder e.g.`--dbpath /usr/local/var/mongodb/data/db`
6.  If you are a windows user: 
    1.  Add "set" to the commands on lines 32 and 33 in package.json so they are in the form: `"set WEBPACK_ENV=development webpack -w"`
    2.  Also change line 31 to: ` "dev:server": "nodemon --exec ./node_modules/.bin/babel-node.cmd init.js --ignore 'scss'",`
7. If you are a linux/Mac user:
   1. Ensure "set" is not before the commands as in 6.1. The should look like: `"WEBPACK_ENV=development webpack -w"`
   2. Ensure line 31 of package.json is: ` "dev:server": "nodemon --exec ./node_modules/.bin/babel-node init.js --ignore 'scss'",`
8. In a new terminal run: `npm run dev:assets`
9.  On the same terminal as 4 run: `npm run dev:server`
10. If instruction 9 gives you an error, try running: `npm i @babel/core @babel/node @babel/polyfill @babel/preset-env babel-loader` and try 9 again.
11. Go to `localhost:4000`



