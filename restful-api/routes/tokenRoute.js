const express = require('express');
const userController = require('../controllers/userController');
const authController = require('../controllers/authController');

const router = express.Router();

router.get('/cleanToken', authController.authenticate, async (req, res, next) => {
    if (req.user===undefined || req.user===null) {
        res.status(401).send();
    } else {
        res.clearCookie('eh_jwt', {
            httpOnly: true,
            sameSite: false,
            signed: true,
            secure: false
        }).status(200).send();
    }
});

router.post('/refreshToken', authController.authenticate, async (req, res, next) => {
    if (req.user===undefined || req.user===null) {
        res.status(401).send();
    } else {
        let error = false;
        let tokenInfo = null;
        try {
            if (req.user.guest===true) {
                throw new Error("Can't refresh guest user");
            }
            tokenInfo = await authController.getToken(req.user._id, req.user.project);
            tokenInfo.user = { "name": req.user.name, "email": req.user.email, "guest": req.user.guest }
        } catch (ex) {
            console.log('Error refreshing token', ex);
            error = true;
        }
        if (error) {
            res.status(400).send();
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
        }         
    }
});

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
            tokenInfo = await authController.getToken(dbUser._id, project);
            tokenInfo.user = { "name": dbUser.name, "email": dbUser.email, "guest": dbUser.guest }
            tokenInfo.project = project;
        }
    } catch (ex) {
        console.log('Error getting user by email, password and project', ex);
        error = true;
    }

    if ( error===null ) {
        res.status(400).send();
    } else if ( error ) {
        res.status(401).send();
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
