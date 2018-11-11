const passport = require('passport');
const JwtStrategy = require('passport-jwt').Strategy;
const ExtractJwt = require('passport-jwt').ExtractJwt;
const ClientCertificateStrategy = require('passport-client-cert').Strategy;
const globalConfig = require('../config_api.js')
const jsonWebToken = require('jsonwebtoken');
const userController = require('./userController');


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

    //
    //Configure jwt strategy
    //
    const jwtOptions = {
        jwtFromRequest: ExtractJwt.fromAuthHeaderAsBearerToken(),
        secretOrKey: globalConfig.JWTSecret,
        issuer: "eventhorizon.pw",
        audience: "eventhorizon.pw",
        algorithm: "HS384"
    }

    passport.use(new JwtStrategy(jwtOptions, async (jwtPayload, done) => {

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
                user.project=project;
            }
        } catch (ex) {
            error = "Invalid user";
        }

        done(error, user);
    }));
}

exports.getToken = async (user,project) => {

    const jwtOptions = {
        expiresIn: '6h',
        issuer: "eventhorizon.pw",
        audience: "eventhorizon.pw",
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
    const d = new Date(0);
    d.setUTCSeconds(jwtPayload.exp);
    return {
        token: jwt,
        expires: d
    };
}
