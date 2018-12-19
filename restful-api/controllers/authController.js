const passport = require('passport');
const JwtStrategy = require('passport-jwt').Strategy;
const ExtractJwt = require('passport-jwt').ExtractJwt;
const ClientCertificateStrategy = require('passport-client-cert').Strategy;
const globalConfig = require('../config_api.js')
const jsonWebToken = require('jsonwebtoken');
const userController = require('./userController');
const routeAuthorizationModel = require('../models/route_authorization');
const clientCertificateModel = require('../models/client_certificate');
const util = require('util');

const jwtOptions = {
    expiresIn: '6h',
    issuer: "ateventhorizon.com",
    algorithm: "HS384",
};

exports.InitializeAuthentication = () => {

    //
    //Configure client certificate strategy
    //
    passport.use(new ClientCertificateStrategy(async (clientCert, done) => {

        const commonName = clientCert.subject.CN;
        let user = false;
        let error = null;
    
        // console.log('CLIENT CERTIFICATE AUTH: '+commonName+"");
        // console.log('Certificate expires: ', clientCert.valid_to);
        // console.log('Certificate expires: ', Date.parse(clientCert.valid_to)/1000);

        const query = {"clientCommonName": commonName};
    
        try {
            const clientCertificateInfoDB = await clientCertificateModel.findOne(query);

            const clientCertificateInfo = clientCertificateInfoDB.toObject();
            user = await userController.getUserByIdProject(clientCertificateInfo.userId, clientCertificateInfo.project);
            if (user===null) {
                error = "User not found";
            } else {
                user.roles=user.roles.map(v => v.toLowerCase());
                user.project=clientCertificateInfo.project.toLowerCase();
                user.expires=Date.parse(clientCert.valid_to)/1000;
                //console.log("Store user: ", user);
            }
        } catch (ex) {
            console.log("Unknown common name", ex)
            error="Invalid certificate";
        }

        if (error!==null) {
            done(null, false, { message: error});
        } else {
            done(null, user);
        }
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

    // const guestHeaderExtractorAsync = async (req) => {
    //     console.log("GUEST HEADER EXTRACTOR");
    //     let project = null;
    //     let token = null;
    //     if (req && req.headers && req.headers['x-eventhorizon-guest'])
    //     {
    //         project = req.headers['x-eventhorizon-guest'];
    //         console.log("P:", project);
    //         //Check if exists guest user for project
    //         let dbGuestUser = await userController.getUserByGuestProject(project);
    //         console.log("U:", dbGuestUser);
    //         if (dbGuestUser!==null) {
    //             //Create token for guest user on the fly
    //             token=await createJwtToken(dbGuestUser._id, project);

    //             console.log("T:",token);
    //         }
    //     }

    //     //const token='eyJhbGciOiJIUzM4NCIsInR5cCI6IkpXVCJ9.eyJ1Ijp7ImkiOiI1YmY5MDQ0NTZhMjMyYTJmYzhmNjJjZGMiLCJwIjoiNDl2aWV3In0sImlhdCI6MTU0NTIxODYxNiwiZXhwIjoxNTQ1MjQwMjE2LCJpc3MiOiJhdGV2ZW50aG9yaXpvbi5jb20ifQ.RIDS4YURT9PNMNqywFU034yx7GGvm8pZgTfLCwo1YRv8jvsskHObaE9TgpPLDvCx';
    //     console.log(token);
    //     return token;
    // }

    // const guestHeaderExtractor = (req) => {
    //     try {
    //         const result  = guestHeaderExtractorAsync(req);
    //         return result;
    //     } catch (ex) {
    //         return null;
    //     }
    // }

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

        //console.log("JWT PAYLOAD", jwtPayload);
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
                user.expires=jwtPayload.exp;
            }
            //console.log("Store user: ", user);
        } catch (ex) {
            error = "Invalid user";
        }

        if (error!==null) {
            done(null, false, { message: error});
        } else {
            done(null, user);
        }
    }));

}

const createJwtToken = async (userId, project) => {
    const payload = {
        u: {
            i: userId,
            p: project
        }
    }
    //console.log(globalConfig.JWTSecret);
    const jwt = await jsonWebToken.sign(payload, globalConfig.JWTSecret, jwtOptions);

    return jwt;
}

exports.getToken = async (userId, project) => {

    const jwt = await createJwtToken(userId, project);
    const jwtPayload = await jsonWebToken.verify(jwt, globalConfig.JWTSecret, jwtOptions);

    return {
        token: jwt,
        expires: jwtPayload.exp
    };
}

exports.verifyToken = async (jwtToken) => {

    return await jsonWebToken.verify(jwtToken, globalConfig.JWTSecret, jwtOptions);
}

exports.authenticate = passport.authenticate(['client-cert','jwt'], {session:false});

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
