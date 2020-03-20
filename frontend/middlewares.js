export const middleware = (req, res, next) => {
  res.locals.user = {
    isAuthenticated: false,
    id: 1
  };
  next();
};

