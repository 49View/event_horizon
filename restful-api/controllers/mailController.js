const sgMail = require("@sendgrid/mail")
const globalConfig = require("../config_api.js");


sgMail.setApiKey(globalConfig.SendGrid);

exports.sendMail = async (to, from, subject, text, html) => {

    const msg = {
        to: to,
        from: from,
        subject: subject,
        text: text,
        html: html
    }

    console.log("Start sending mail");
    let error=false;
    try {
        await sgMail.send(msg);
        console.log("Mail sent");
    } catch (ex) {
        error=true;
        console.log("Mail don't sent");
    }

    return error;
}