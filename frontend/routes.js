const HOME = "/";
const JOIN = "/join";
const LOGIN = "/login";
const LOGOUT = "/logout";

const USERS = "/users";
const USER_DETAIL = "/:id";

const LANDS = "/lands";
const ADD_LAND = "/addLand";
const LAND_DETAIL = "/:id";
const START_SURVEY = "/:id/startSurvey";
const DELETE_LAND = "/:id/delete";

const routes = {
  home: HOME,
  join: JOIN,
  login: LOGIN,
  logout: LOGOUT,
  users: USERS,
  userDetail: id => {
    if (id) return `/users/${id}`;
    else return USER_DETAIL;
  },
  lands: LANDS,
  startSurvey: id => {
    if (id) {
      return `/lands/${id}/startSurvey`;
    } else {
      return START_SURVEY;
    }
  },
  addLand: ADD_LAND,
  landDetail: id => {
    if (id) return `/lands/${id}`;
    else return LAND_DETAIL;
  },
  deleteLand: id => {
    if (id) {
      return `/lands/${id}/delete`;
    } else {
      return DELETE_LAND;
    }
  }
};

export default routes;
