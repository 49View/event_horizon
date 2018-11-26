const express = require('express');
const userController = require('../controllers/userController');
const authController = require('../controllers/authController');

const router = express.Router();

router.post('/getToken', async (req, res, next) => {

    console.log('getToken');

    const project = req.body.project;
    const email = req.body.email;
    const password = req.body.password;
    let error = false;
    let tokenInfo = null;

    try {
        dbUser = await userController.getUserByEmailPasswordProject(email, project, password);
        if (dbUser===null) {
            error=true;
        } else {
            tokenInfo = await authController.getToken(dbUser, project, res);
        }
    } catch (ex) {
        console.log('Error getting user by email, password and project', ex);
        error = true;
    }

    if ( error===null ) {
        res.sendStatus(400);
    } else if ( error ) {
        res.sendStatus(401);
    } else {
        const d = new Date(0);
        d.setUTCSeconds(tokenInfo.expires);
    
        res.cookie('eh_jwt', tokenInfo.token, {
            httpOnly: true,
            sameSite: false,
            signed: true,
            secure: true,
            expires: d
        }).send(tokenInfo);
        // next();
    }

});

module.exports = router;
