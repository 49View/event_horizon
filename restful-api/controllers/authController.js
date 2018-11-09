const passport = require('passport');
const JwtStrategy = require('passport-jwt').Strategy;
const ExtractJwt = require('passport-jwt').ExtractJwt;
const ClientCertificateStrategy = require('passport-client-cert').Strategy;


exports.InitializeAuthentication = (globalConfig) => {

    //
    //Configure client certificate strategy
    //
    passport.use(new ClientCertificateStrategy((clientCert, done) => {

        

        if (clientCert===undefined) {
            console.log("Undefined client cert");
            console.log(clientCert);
        }

        console.log("CLient Certificate:", clientCert);

        var cn = clientCert.subject.CN,
        user = null;
    
        console.log('CLIENT CERTIFICATE AUTH: '+cn);

        // The CN will typically be checked against a database
        if(cn === 'client1') {
            user = { name: 'Test User' };
        }
    
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
        algorithms: "HS384",
        ignoreExpiration: "false"
    }

    passport.use(new JwtStrategy(jwtOptions, (jwtPayload, done) => {

        //Check user in payload
        console.log('JWT AUTH: ',jwtPayload);
    }));
}
