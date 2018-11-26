const passport = require('passport');
const JwtStrategy = require('passport-jwt').Strategy;
const ExtractJwt = require('passport-jwt').ExtractJwt;
const ClientCertificateStrategy = require('passport-client-cert').Strategy;
const globalConfig = require('../config_api.js')
const jsonWebToken = require('jsonwebtoken');
const userController = require('./userController');
const routeAuthorizationModel = require('../models/route_authorization');



exports.InitializeAuthentication = () => {

    //
    //Configure client certificate strategy
    //
    passport.use(new ClientCertificateStrategy((clientCert, done) => {

        var cn = clientCert.subject.CN,
        user = null;
    
        console.log('CLIENT CERTIFICATE AUTH: '+cn);

        // The CN will typically be checked against a database
        if (cn === 'client1') {
            user = { name: 'Test User' };
        }
        console.log(user);

        done(null, user);
    }));

    
    const cookieExtractor = function(req) {
        console.log("COOKIE EXTRACTOR");
        var token = null;
        if (req && req.signedCookies && req.signedCookies['eh_jwt'])
        {
            token = req.signedCookies['eh_jwt'];
        }
        return token;
    };

    const authHeaderExtractor = function(req) {
        console.log("AUTH HEADER EXTRACTOR");
        var token = null;
        if (req && req.headers && req.headers['authorization'] && req.headers['authorization'].startsWith('Bearer '))
        {
            token = req.headers['authorization'].substr(7);
        }
        return token;
    }

    //
    //Configure jwt strategy
    //
    const jwtOptions = {
        jwtFromRequest: ExtractJwt.fromExtractors([cookieExtractor, authHeaderExtractor]),
        secretOrKey: globalConfig.JWTSecret,
        issuer: "ateventhorizon.com",
        algorithm: "HS384"
    }

    passport.use(new JwtStrategy(jwtOptions, async (jwtPayload, done) => {

        console.log("JWT PAYLOAD", jwtPayload);
        let error = null;
        let user = false;
        //Check user in payload

        const userId = jwtPayload.u.i;
        const project = jwtPayload.u.p;

        try {
            user = await userController.getUserByIdProject(userId, project);
            if (user===null) {
                error = "Invalid user";
            } else {
                user.roles=user.roles.map(v => v.toLowerCase());
                user.project=project.toLowerCase();
            }
        } catch (ex) {
            error = "Invalid user";
        }

        done(error, user);
    }));
}

exports.getToken = async (user, project, res) => {

    const jwtOptions = {
        expiresIn: '6h',
        issuer: "ateventhorizon.com",
        algorithm: "HS384",
    };

    const payload = {
        u: {
            i: user._id,
            p: project
        }
    }
    const jwt = await jsonWebToken.sign(payload, globalConfig.JWTSecret, jwtOptions);
    const jwtPayload = await jsonWebToken.verify(jwt, globalConfig.JWTSecret, jwtOptions);

    return {
        token: jwt,
        expires: jwtPayload.exp
    };
}

exports.authorize = async (req,res,next) => {

    const url = req.originalUrl;
    const urlParts = url.split("/");
    let authorized = false;

    if (urlParts.length>0 && urlParts[0].length===0) {
        urlParts.shift();       
    }
    const urlPartials = [];
    for (let i=urlParts.length;i>0;i--) {
        let urlPartial = "";
        for (let j=0;j<i;j++) {
            urlPartial=urlPartial+"/"+urlParts[j].toLowerCase();
        }
        urlPartials.push(urlPartial);
    }

    const query = { "$and": [ { "verb": req.method.toLowerCase()}, { "route": { "$in": urlPartials}}]};
    try {
        const routeAuthorizations = await routeAuthorizationModel.find(query).sort([['route','descending']]);
        let routeAuthorization = [];
        for (let i=0;i<urlPartials.length;i++) {
            for (let j=0;j<routeAuthorizations.length;j++) {
                let currentAuthorization=routeAuthorizations[j].toObject();
                if (urlPartials[i]===currentAuthorization.route) {
                    routeAuthorization.push(currentAuthorization);
                }
            }
            if (routeAuthorization.length>0) {
                break;
            }
        }
        for (let i=0;i<routeAuthorization.length;i++) {
            let currentAuthorization=routeAuthorization[i];
            currentAuthorization.project=currentAuthorization.project.toLowerCase();
            currentAuthorization.role=currentAuthorization.role.toLowerCase();
            if ((currentAuthorization.project==="*" || currentAuthorization.project===req.user.project) &&
                (currentAuthorization.role==="*" || req.user.roles.indexOf(currentAuthorization.role)>=0)  &&
                (currentAuthorization.user==="*" || currentAuthorization.user===req.user._id.toString())) {      
                    console.log("Valid authorization: ",currentAuthorization);
                    authorized=true;
                    break;
            }
        }
    } catch (ex) {
        console.log("ERROR IN AUTHORIZATION: "+ex);
        authorized=false;
    }

    // console.log("PARTIAL: "+urlPartials);
    // console.log("METHOD: ",req.method);

    if (!authorized) {
        res.sendStatus(401);
    } else {
        next(); 
    }   
}
