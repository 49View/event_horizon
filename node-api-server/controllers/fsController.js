
var cloudApi = require('../third_party_api/s3');

exports.cloudStorageFileUpdate = (body, key) => {
	return cloudApi.upload( body, decodeURIComponent(key) );
}

exports.cloudStorageFileGet = (key) => {
	return cloudApi.get( decodeURIComponent(key) );
}

exports.cloudStorageRename = (source, dest) => {
	return cloudApi.rename( decodeURIComponent(source), decodeURIComponent(dest) );
}

exports.cloudStorageDelete = (key) => {
	return cloudApi.delete( decodeURIComponent(key) );
}

exports.cloudStorageDeleteMulti = (key) => {
	return cloudApi.deleteMulti( decodeURIComponent(key) );
}

exports.cloudStorageFileList = ( key, completed ) => {
    return cloudApi.list( decodeURIComponent(key), completed );
}
