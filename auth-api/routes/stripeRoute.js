const express = require("express");
const stripeController = require("../controllers/stripeController");

const router = express.Router();

router.post("/getsession/:skid", async (req, res, next) => {
  try {
    const session = await stripeController.getSession(req.params.skid);
    res.status(200).send(session);
  } catch (ex) {
    console.log("Stripe getsession errror " + ex);
    res.sendStatus(400);
  }
});

module.exports = router;
