const express = require('express');
const userController = require('../controllers/userController');
const authController = require('../controllers/authController');
const sessionController = require('../controllers/sessionController');

const router = express.Router();

router.get('/cleanToken', authController.authenticate, async (req, res, next) => {
    if (req.user===undefined || req.user===null) {
        res.status(401).send();
    } else {
        if (req.user.hasToken===true) {
            await sessionController.invalidateSessionById(req.user.sessionId);
            res.clearCookie('eh_jwt', {
                httpOnly: true,
                sameSite: false,
                signed: true,
                secure: false
            }).status(200).send();
        } else {
            res.status(401).send();
        }
    }
});

router.post('/refreshToken', authController.authenticate, async (req, res, next) => {
    console.log(req.user);
    if (req.user===undefined || req.user===null) {
        res.status(401).send();
    } else {
        let error = false;
        let tokenInfo = null;
        const ipAddress = req.ip;
        const userAgent = req.headers['user-agent'] || null;
    
        try {
            if (req.user.hasToken===true) {
                await sessionController.invalidateSessionById(req.user.sessionId);
                tokenInfo = await authController.getToken(req.user._id, req.user.project, ipAddress, userAgent);
                tokenInfo.user = { "name": req.user.name, "email": req.user.email, "guest": req.user.guest }
            } else {
                throw new Error("Can't refresh token");
            }
        } catch (ex) {
            console.log('Error refreshing token', ex);
            error = true;
        }
        if (error) {
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
    const ipAddress = req.ip;
    const userAgent = req.headers['user-agent'] || null;

    try {
        dbUser = await userController.getUserByEmailPasswordProject(email, project, password);
        if (dbUser===null) {
            error=true;
        } else {
            tokenInfo = await authController.getToken(dbUser._id, project, ipAddress, userAgent);
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
