
exports.decodeRequest = (req,res,next) => {
    console.log("DECODE REQUEST", req.url);
    try {
        if (req.url.startsWith("/FFFE")) {
            //Register CIPHER KEY FOR CLIENT e return CLIENT ID
        }
        if (req.url.startsWith("/FFFF")) {
            // //Register DECODE URL AND BODY WITH CIPHER KEY FOR CLIENT [CLIENT ID]

            const uncipherRequest = Buffer.from(req.url.substr(5), "base64").toString();
            //console.log("UNCIPHER REQUEST: ",uncipherRequest);

            req.url=uncipherRequest;

            if (req.body.d!==undefined) {
                //console.log(req.body.d);
                const uncipherBody=Buffer.from(req.body.d,"base64").toString();
                req.body=JSON.parse(uncipherBody);
                //console.log("BODY: ",req.body);
            }

            const temp = res.send;
            res.send = function() {
                //console.log('Send encoded response ...');
                //console.log("Parameters for send: ",arguments);
                if (arguments[0]!==undefined) {
                    let cipherResponse;
                    //console.log("Response: ", typeof arguments[0], arguments[0]);
                    if (arguments[0] instanceof Buffer) {
                        cipherResponse=arguments[0].toString('base64');
                    } else {
                        cipherResponse = Buffer.from(JSON.stringify(arguments[0])).toString('base64');
                    }
                    // console.log("Cipher parameters for send: ", cipherResponse);
                    arguments[0]=cipherResponse;
                    //console.log("Cipher parameters for send: ",arguments);
                }
                temp.apply(this,arguments);
            }
        }
    } catch (ex) {
        console.log("ERROR:",ex);
        res.sendStatus(400);
        return;
    }
    next();
}

exports.checkRequest = async (req,res,next) => {
    console.log("CHECK REQUEST: "+req.url);
    next();
}