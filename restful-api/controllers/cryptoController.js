
exports.decodeRequest = async (req,res,next) => {
    try {
        if (req.url.startsWith("/FFFE")) {
            //Register CIPHER KEY FOR CLIENT e return CLIENT ID
        }
        if (req.url.startsWith("/FFFF")) {
            //Register DECODE URL AND BODY WITH CIPHER KEY FOR CLIENT [CLIENT ID]

            const cipherRequest = Buffer.from(req.url.substr(5), "base64").toString();

            console.log("UNCIPHER REQUEST: "+cipherRequest);
            req.url=cipherRequest;
            var temp = res.send;
            res.send = function() {
                console.log('do something..');
                console.log("Parameters for send: ",arguments);
                if (arguments[0]!==undefined) {
                    const cipherResponse = Buffer.from(JSON.stringify(arguments[0])).toString('base64');
                    // console.log("Cipher parameters for send: ", cipherResponse);
                    arguments[0]=cipherResponse;
                    console.log("Cipher parameters for send: ",arguments);
                }
                temp.apply(this,arguments);
            }
        }
    } catch (ex) {
        res.sendStatus(400);
        return;
    }
    next();
}

exports.checkRequest = async (req,res,next) => {
    console.log("CHECK REQUEST: "+req.url);
    next();
}