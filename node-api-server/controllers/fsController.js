
var cloudApi = require('../third_party_api/s3');

exports.cloudStorageFileUpdate = (body, key) => {
	return cloudApi.upload( body, key );
}

exports.cloudStorageFileGet = (key) => {
	return cloudApi.get( key );
}

exports.cloudStorageRename = (source, dest) => {
	return cloudApi.rename( source, dest );
}

exports.cloudStorageDelete = (key) => {
	return cloudApi.delete( key );
}

exports.cloudStorageDeleteMulti = (key) => {
	return cloudApi.deleteMulti( key );
}

exports.cloudStorageFileList = ( key, completed ) => {
    return cloudApi.list( key, completed );
}
