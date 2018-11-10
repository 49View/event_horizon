const passport = require('passport');
const JwtStrategy = require('passport-jwt').Strategy;
const ExtractJwt = require('passport-jwt').ExtractJwt;
const ClientCertificateStrategy = require('passport-client-cert').Strategy;
const globalConfig = require('../config_api.js')
const jsonWebToken = require('jsonwebtoken');


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
        algorithm: "HS384",
        ignoreExpiration: "false"
    }

    passport.use(new JwtStrategy(jwtOptions, (jwtPayload, done) => {
        //Check user in payload
        user = jwtPayload.user;

        done(null, user);
    }));
}

exports.getToken = async (user) => {

    const jwtOptions = {
        expiresIn: '12h',
        issuer: "eventhorizon.pw",
        audience: "eventhorizon.pw",
        algorithm: "HS384",
    };

    const jwt = await jsonWebToken.sign({ user: user}, globalConfig.JWTSecret, jwtOptions);
    const payload = await jsonWebToken.verify(jwt, globalConfig.JWTSecret, jwtOptions);
    const d = new Date(0);
    d.setUTCSeconds(payload.exp);
    return {
        token: jwt,
        expires: d
    };
}
