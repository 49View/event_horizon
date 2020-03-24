class GlobalConfig {
	constructor(lMongoDBUser, lMongoDBPass, lMongoDBURI, lMongoDBdbName, lMongoDBReplicaSetName, lJWTSecret, lSendGrid, lCloudHost,lTokenCookie,lAntiForgeryTokenCookie,lJwtExpiresSeconds) {
		this.MongoDBUser = lMongoDBUser;
		this.MongoDBPass = lMongoDBPass;
		this.MongoDBURI = lMongoDBURI;
		this.MongoDBdbName = lMongoDBdbName;
		this.MongoDBReplicaSetName = lMongoDBReplicaSetName;
		this.mJWTSecret = lJWTSecret;
		this.mSendGrid = lSendGrid,
		this.CloudHost = lCloudHost,
		this.TokenCookie = lTokenCookie,
        this.AntiForgeryTokenCookie = lAntiForgeryTokenCookie,
        this.JwtExpiresSeconds = Number(lJwtExpiresSeconds)
	}

	get JWTSecret() {
		return this.mJWTSecret;
	}
	get SendGrid() {
		return this.mSendGrid;
	}
}

const gc = new GlobalConfig(
	  process.env.EH_MONGO_USER
	, process.env.EH_MONGO_PASSWORD
	, process.env.EH_MONGO_PATH
	, process.env.EH_MONGO_DEFAULT_DB
	, process.env.EH_MONGO_REPLICA_SET_NAME
	, process.env.EH_MASTER_TOKEN
	, process.env.EH_SEND_GRID_TOKEN
	, process.env.EH_CLOUD_HOST
	, process.env.EH_TOKEN_COOKIE
    , process.env.EH_ANTIFORGERYTOKEN_COOKIE
    , process.env.EH_JWT_EXPIRES_SECONDS
);

module.exports = gc;
