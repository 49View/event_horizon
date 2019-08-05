class GlobalConfig {
	constructor(lS3Bucket, lMongoDBUser, lMongoDBPass, lMongoDBURI, lMongoDBdbName, lAWSAccessId, lAWSSecretKeyId, lJWTSecret, lSendGrid) {
		this.mS3Bucket = lS3Bucket;
		this.mMongoDBUser = lMongoDBUser;
		this.mMongoDBPass = lMongoDBPass;
		this.mMongoDBURI = lMongoDBURI;
		this.mMongoDBdbName = lMongoDBdbName;			
		this.mAWSConfigAccessKeyId = lAWSAccessId;
		this.mAWSConfigSecretKeyId = lAWSSecretKeyId;
		this.mJWTSecret = lJWTSecret;
		this.mSendGrid = lSendGrid
	}

	get S3Bucket() {
		return this.mS3Bucket;
	}
	get MongoDBUser() {
		return this.mMongoDBUser;
	}
	get MongoDBPass() {
		return this.mMongoDBPass;
	}
	get MongoDBURI() {
		return this.mMongoDBURI;
	}
	get MongoDBdbName() {
		return this.mMongoDBdbName;
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
	, "dado"
	, "luckycarrot1803"
	, "sixthviewmdb001-a0hba.mongodb.net"
	, "sixthviewdb"
	, "AKIAJWSLLXTAUPNWMBOA"
	, "9lP99+jK8Ki1XD/MoDv/Ci2/Bo6pzurZjntltd0r"
	, "1sg2&db9(4h23bd?dhs3+1t6-36jh/4fgmm+++xc29hgjq2&www4346sd234sdf23"
	, "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"
);

module.exports = gc;