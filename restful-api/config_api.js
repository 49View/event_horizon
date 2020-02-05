class GlobalConfig {
	constructor(lMongoDBUser, lMongoDBPass, lMongoDBURI, lMongoDBdbName, lMongoDBReplicaSetName, lJWTSecret, lSendGrid) {
		this.MongoDBUser = lMongoDBUser;
		this.MongoDBPass = lMongoDBPass;
		this.MongoDBURI = lMongoDBURI;
		this.MongoDBdbName = lMongoDBdbName;
		this.MongoDBReplicaSetName = lMongoDBReplicaSetName;
		this.mJWTSecret = lJWTSecret;
		this.mSendGrid = lSendGrid
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
);

module.exports = gc;
