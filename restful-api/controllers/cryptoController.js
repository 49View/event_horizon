
const crypto = require('crypto')
const rsa = require('js-crypto-rsa');

let keyPair = null;
let privateKey, publicKey;
const passphrase = "m(73f=fn2wxccv325rs1%$%&Szz2sdjnkl549huuq324u9g1q23";


exports.generateKey = async () => {
    try {
        //crypto.getCiphers().forEach(e => { console.log(e)});
        const keyPair = crypto.generateKeyPairSync('rsa', {
            modulusLength: 4096,
            publicKeyEncoding: {
              type: 'spki',
              format: 'pem'
            },
            privateKeyEncoding: {
              type: 'pkcs8',
              format: 'pem',
              cipher: 'aes-256-cbc',
              passphrase: passphrase
            }
          });
          privateKey=keyPair.privateKey;
          publicKey=keyPair.publicKey;
          console.log("Private: ", privateKey);
          console.log("Public: ", publicKey);
          const baseText="Questo è un testo di prova criptato";
          console.log("Text ["+baseText.length+"]: *"+baseText+"*");
          const cipherText = crypto.publicEncrypt({ key:publicKey, padding: crypto.constants.RSA_PKCS1_OAEP_PADDING} , Buffer.from(baseText));
          console.log("Ciphered ["+cipherText.length+"]: ", cipherText);

          const cleanText = crypto.privateDecrypt({ key:privateKey, padding: crypto.constants.RSA_PKCS1_OAEP_PADDING, passphrase: passphrase}, cipherText);
          console.log("Deciphered ["+cleanText.length+"]: "+ cleanText + " *"+ cleanText+"*");
    } catch (ex) {
        console.log("ERROR", ex);
    }
}

exports.decodeRequest = (req,res,next) => {
    console.log("DECODE REQUEST", req.url);
    try {
        if (req.url.startsWith("/FFFA")) {
            //Return server public key
        }
        if (req.url.startsWith("/FFFE")) {
            //Save client key, assign id to key (and client), encrypt id with client key and send to client
        }
        if (req.url.startsWith("/FFFF")) {
            //Get client key from client id, decrypt message with key, route messane then encrypt response and send to client

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