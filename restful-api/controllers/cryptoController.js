
exports.decodeRequest = async (req,res,next) => {
    try {
        if (req.url.startsWith("/FFFE")) {
            //Register CIPHER KEY FOR CLIENT e return CLIENT ID
        }
        if (req.url.startsWith("/FFFF")) {
            //Register DECODE URL AND BODY WITH CIPHER KEY FOR CLIENT [CLIENT ID]

            // const cipherRequest = Buffer.from(req.url.substr(5), "base64").toString();

            // console.log(cipherRequest);
            // req.url=cipherRequest;
        }
    } catch (ex) {
        res.sendStatus(400);
        return;
    }
    next();
}

exports.checkRequest = async (req,res,next) => {
    console.log(req.url);
    next();
}