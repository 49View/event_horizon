class GlobalConfig {
	constructor(lS3Bucket, lMongoDBUser, lMongoDBPass, lMongoDBURI, lMongoDBdbName) {
		this.mS3Bucket = lS3Bucket;
		this.mMongoDBUser = lMongoDBUser;
		this.mMongoDBPass = lMongoDBPass;
		this.mMongoDBURI = lMongoDBURI;
		this.mMongoDBdbName = lMongoDBdbName;			
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
}

const gc = new GlobalConfig("sixthviewfs", "dado", "luckycarrot1803", "sixthviewmdb001-a0hba.mongodb.net", "sixthviewdb");

module.exports = gc;