class GlobalConfig {
	constructor(lS3Bucket, lMongoDBUser, lMongoDBPass, lMongoDBURI, lMongoDBdbName, lMongoDBReplicaSetName, lAWSAccessId, lAWSSecretKeyId, lJWTSecret, lSendGrid) {
		this.mS3Bucket = lS3Bucket;
		this.MongoDBUser = lMongoDBUser;
		this.MongoDBPass = lMongoDBPass;
		this.MongoDBURI = lMongoDBURI;
		this.MongoDBdbName = lMongoDBdbName;
		this.MongoDBReplicaSetName = lMongoDBReplicaSetName;
		this.mAWSConfigAccessKeyId = lAWSAccessId;
		this.mAWSConfigSecretKeyId = lAWSSecretKeyId;
		this.mJWTSecret = lJWTSecret;
		this.mSendGrid = lSendGrid
	}

	get S3Bucket() {
		return this.mS3Bucket;
	}
	get AWSConfigAccessKeyId() {
		return this.mAWSConfigAccessKeyId;
	}
	get AWSConfigSecretKeyId() {
		return this.mAWSConfigSecretKeyId;
	}
	get JWTSecret() {
		return this.mJWTSecret;
	}
	get SendGrid() {
		return this.mSendGrid;
	}
}

const gc = new GlobalConfig(
	"eventhorizonfs"
	, process.env.EH_MONGO_USER
	, process.env.EH_MONGO_PASSWORD
	, process.env.EH_MONGO_PATH
	, process.env.EH_MONGO_DEFAULT_DB
	, process.env.EH_MONGO_REPLICA_SET_NAME
	, "AKIAJWSLLXTAUPNWMBOA"
	, "9lP99+jK8Ki1XD/MoDv/Ci2/Bo6pzurZjntltd0r"
	, "1sg2&db9(4h23bd?dhs3+1t6-36jh/4fgmm+++xc29hgjq2&www4346sd234sdf23"
	, "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"
);

module.exports = gc;
