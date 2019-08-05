const sgMail = require("@sendgrid/mail")
const globalConfig = require("../config_api.js");


sgMail.setApiKey(globalConfig.SendGrid);

