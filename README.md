#DP GROUP 9 - eEIEIO

1.  Ensure you have MongoDB installed: [https://www.mongodb.com/download-center/community]
2.  Install Node.js: [https://nodejs.org/en/download/]
3.  `cd <SDP directory>/frontend`
4.  `npm install`
5.  `mongod`
6.  If you are a windows user: 
    1.  Add "set" to the commands on lines 32 and 33 so they are in the form: `"set WEBPACK_ENV=development webpack -w"`
    2.  Change line 31 to: ` "dev:server": "nodemon --exec ./node_modules/.bin/babel-node.cmd init.js --ignore 'scss'",`
7. In a new terminal: `npm run dev:assets`
8. On the same terminal as 4: `npm run dev:server`


