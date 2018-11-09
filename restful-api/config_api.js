class GlobalConfig {
	constructor(lS3Bucket, lMongoDBUser, lMongoDBPass, lMongoDBURI, lMongoDBdbName, lAWSAccessId, lAWSSecretKeyId) {
		this.mS3Bucket = lS3Bucket;
		this.mMongoDBUser = lMongoDBUser;
		this.mMongoDBPass = lMongoDBPass;
		this.mMongoDBURI = lMongoDBURI;
		this.mMongoDBdbName = lMongoDBdbName;			
		this.mAWSConfigAccessKeyId = lAWSAccessId;
		this.mAWSConfigSecretKeyId = lAWSSecretKeyId;

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
}

const gc = new GlobalConfig("sixthviewfs", "dado", "luckycarrot1803", "sixthviewmdb001-a0hba.mongodb.net", "sixthviewdb", "AKIAJWSLLXTAUPNWMBOA", "9lP99+jK8Ki1XD/MoDv/Ci2/Bo6pzurZjntltd0r");

module.exports = gc;