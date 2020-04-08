# SDP GROUP 9 - eEIEIO

### Front-end Server Set-up:
1. Ensure MongoDB is installed. You can download MongoDB from: [https://www.mongodb.com/download-center/community]
2. Install the latest version of Node.js from: [https://nodejs.org/en/download/]
3. Open a new terminal and navigate to the front end folder: `cd  <Directory>/SDP/frontend`
4. In the same terminal, run: `npm install` 
5. In the same terminal, run: `mongod `
   1. If the following exception is shown, the data folder has not been created properly on installation:
    `exception in initAndListen: NonExistentPath: Data directory /data/db not found. `
   2. Open a new terminal and from the home directory run mkdir /data/db
   3. Run `mongod`
   4. If this gives a permission denied error on a Linux or MacOS machine, run: `sudo mongod`
   5. If you still get an error, include the MongoDB path to the db folder using:
    `sudo mongod --dbpath /usr/local/var/mongodb/data/db`
    Please note: the path to /data/db may be different on your machine
6. In a new terminal, navigate to the frontend folder: `cd <Directory>/SDP/frontend`
7. Before starting the server, you should bundle the assets. This will automatically combine your modules to create 1 file containing all assets/modules. In the same terminal:
   1. Windows users run: `npm run dev:assets-windows`
   2. Linux/MacOS users run: `npm run dev:assets-default`
8. You should now start the server. In the same terminal again:
   1. Windows users run: `npm run dev:server-windows`
   2. Linux/MacOS users run: `npm run dev:server-default`
9. If an error occurs in step 8, run: `npm i @babel/core @babel/node @babel/polyfill @babel/preset-env babel-loader` and run the command again. This will manually install modules required.
10. Open a browser and go to localhost:4000 to launch the website. This may take a minute the first time it is loaded. 


