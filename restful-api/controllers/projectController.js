const userController = require('./userController');
const authController = require('./authController');
const projectModel = require('../models/project');
const jsonWebToken = require('jsonwebtoken');
const axios = require('axios');


const getProjectInfo = async (projectName) => {
    
    let projectInfo = null;

    try {
        const dbProject=await projectModel.findOne({ "name": { $regex: projectName, $options: "i"}});
        if (dbProject!==null) {
            projectInfo=dbProject.toObject();
            projectInfo.apiPrefix=projectInfo.apiPrefix.toLowerCase();
            if (!projectInfo.apiPrefix.endsWith("/")) {
                projectInfo.apiPrefix+="/";
            }
            projectInfo.apiServer=projectInfo.apiServer.toLowerCase();
            if (!projectInfo.apiServer.endsWith("/")) {
                projectInfo.apiServer+="/";
            }        
        }
    } catch (ex) {
        console.log("Error initializing projects routes", ex);
        projectInfo = null
    }

    return projectInfo;
}

exports.checkProjectRoutes = async (req,res,next) => {

    try {

        const currentUser = req.user;
        let projectToRoute = null;
        const projectInfo = await getProjectInfo(req.user.project);
        if (projectInfo!==null) {
            const requestUrl=req.url.toLowerCase();
            let apiPrefix = projectInfo.apiPrefix;    
            if (requestUrl.startsWith(apiPrefix)) {
                projectToRoute=projectInfo;
            }
        }

        if (projectToRoute!==null) {

            const projectApiUrl = projectToRoute.apiServer+req.url.substr(projectToRoute.apiPrefix.length);
            console.log("ROUTE REQUEST TO PROJECT API: ", projectApiUrl);

            const jwtOptions = {
                issuer: "ateventhorizon.com",
                audience: projectToRoute.apiServer,
                algorithm: "HS384",
            };
        
            const payload = {
                user: {
                    id: currentUser._id.toString(),
                    name: currentUser.name,
                    email: currentUser.email,
                    roles: currentUser.roles
                },
                exp: currentUser.expires
            }
            const projectApiJwt = await jsonWebToken.sign(payload, projectToRoute.apiSecret, jwtOptions);
        
            const projectApiRequest = {
                method: req.method,
                url: projectApiUrl,
                data: req.body,
                headers: {
                    "authorization": "Bearer "+projectApiJwt,
                    "accept": "*/*"
                },
                "responseType": "arraybuffer"
            }

            try {
                const projectApiResponse = await axios(projectApiRequest);

                let responseHeaders = {};
                if (projectApiResponse.headers && projectApiResponse.headers["content-type"]) {
                    responseHeaders["Content-Type"]=projectApiResponse.headers["content-type"];
                }
                if (projectApiResponse.headers && projectApiResponse.headers["content-length"]) {
                    responseHeaders["Content-Length"]=projectApiResponse.headers["content-length"];
                }
                if (projectApiResponse.headers && projectApiResponse.headers["etag"]) {
                    responseHeaders["Etag"]=projectApiResponse.headers["etag"];
                }

                res.status(projectApiResponse.status).set(responseHeaders).send(projectApiResponse.data);
            } catch (ex) {
                console.log("ERROR IN REDIRECT TO PROJECT");
                if (ex && ex.response && ex.response.status && ex.response.statusText) {
                    res.status(ex.response.status).send(ex.response.statusText);
                } else {
                    console.log(ex);
                    res.status(404).send("Bad request");
                }
            }
        } else {
            next();
        }
    } catch (ex) {
        console.log("Error checking projects routes", ex);
        next();
    }
}