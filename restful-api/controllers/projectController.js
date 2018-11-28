const userController = require('./userController');
const authController = require('./authController');
const projectModel = require('../models/project');
const axios = require('axios');


const InitializeProjectsRoutes = async () => {
    
    let projectsInfo = [];

    try {
        let dbProjects = [];
        dbProjects=await projectModel.find({});

        projectsInfo=dbProjects.map(dbp => {
            const p=dbp.toObject();
            p.apiPrefix=p.apiPrefix.toLowerCase();
            if (!p.apiPrefix.endsWith("/")) {
                p.apiPrefix+="/";
            }
            p.apiServer=p.apiServer.toLowerCase();
            if (!p.apiServer.endsWith("/")) {
                p.apiServer+="/";
            }
            return p;
        });
    } catch (ex) {
        console.log("Error initializing projects routes");
        projectsInfo = [];
    }

    return projectsInfo;
}

exports.CheckProjectsRoutes = async (req,res,next) => {

    try {

        const projectsInfo = await InitializeProjectsRoutes();

        const requestUrl=req.url.toLowerCase();
        let projectToRoute = null;
        projectsInfo.every( projectInfo => {
            let apiPrefix = projectInfo.apiPrefix;    
            if (requestUrl.startsWith(apiPrefix)) {
                projectToRoute=projectInfo;
                return false;
            }
            return true;
        });

        if (projectToRoute!==null) {

            const projectApiUrl = projectToRoute.apiServer+req.url.substr(projectToRoute.apiPrefix.length);
            console.log("ROUTE REQUEST TO PROJECT API: ", projectApiUrl)

            const currentUser = req.user;
            const projectApiRequest = {
                method: req.method,
                url: projectApiUrl,
                data: req.body,
                headers: {
                    "x-eventhorizon-username": currentUser.name,
                    "x-eventhorizon-useremail": currentUser.email,
                    "x-eventhorizon-userid": currentUser._id.toString(),
                    "x-eventhorizon-roles": currentUser.roles.join(","),
                    "x-eventhorizon-tokenexpires": currentUser.expires
                }
            }

            try {
                console.log(projectApiRequest);
                const projectApiResponse = await axios(projectApiRequest);
                res.status(projectApiResponse.status).send(projectApiResponse.data);
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