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
	, "1sg2&db9(4h23bd?dhs3+1t6-36jh/4fgmm+++xc29hgjq2&www4346sd234sdf23"
	, "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"
);

module.exports = gc;
