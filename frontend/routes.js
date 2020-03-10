// Global routes
const HOME = "/";
const JOIN = "/join";
const LOGIN = "/login";
const LOGOUT = "/logout";

// User routes
const USERS = "/users";
const USER_DETAIL = "/:id";
const CHANGE_PASSWORD = "/change-password";

// Land routes
const LANDS = "/lands";
const LAND_DETAIL = "/:id";
const ADD_LAND = "/add";
const DELETE_LAND = "/:id/delete";

const routes = {
  home: HOME,
  join: JOIN,
  login: LOGIN,
  logout: LOGOUT,
  users: USERS,
  changePassword: CHANGE_PASSWORD,
  lands: LANDS,
  addLand: ADD_LAND,
  deleteLand: DELETE_LAND,
  userDetail: id => {
    // not done
  },
  landDetail: id => {
    // not done
  }
};

export default routes;
