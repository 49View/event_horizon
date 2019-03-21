
const cloudApi = require('../third_party_api/s3');


exports.cloudStorageFileUpload = (body, key, bucket) => {
	return cloudApi.upload( body, key, bucket);
}

exports.cloudStorageFileGet = (key, bucket) => {
	return cloudApi.get( key, bucket );
}

exports.cloudStorageRename = (source, dest, bucket) => {
	return cloudApi.rename( source, dest, bucket );
}

exports.cloudStorageDelete = (key, bucket) => {
	return cloudApi.delete( key, bucket );
}

exports.cloudStorageCheckExists = async ( key, bucket ) => {
    return await cloudApi.checkObjectExists(key,bucket);
}

exports.cloudStorageGetFilenameAndDuplicateIfExists = async ( key, bucket, filename ) => {    
    let finalName = key;
    const result = await cloudApi.checkObjectExists(key, bucket);
    console.log( "Result of duplicate check " + result );
    if ( result == true ) {
        // Now the file has been found so add the classic ugly orrible _N appendix
        const str = key;
        const n = str.lastIndexOf(".");
        if ( n == -1 ) n = str.length;
        const d = new Date(); 
        finalName = str.substring(0, n) + "_" + d.getTime() + str.substring(n, str.length);
        filename['changed'] = true;
    } else {
        filename['changed'] = false;
    }
    filename['name'] = finalName;
}

// exports.cloudStorageDeleteMulti = (key) => {
// 	return cloudApi.deleteMulti( key );
// }

// exports.cloudStorageFileList = ( key, completed ) => {
//     return cloudApi.list( key, completed );
// }

exports.writeFile = ( res, data ) => {
    res.status(200).set(
        {
            'Content-Type': data.ContentType,
            'Content-Last-Modified': data.LastModified,
            'ETag': data.ETag,
            'Content-Length': data.ContentLength
        }
    ).send(data["Body"]);
}

exports.writeError = ( res, number, err, message ) => {
    console.log("[ERROR-CAUGHT]");
    console.log(err, err.stack);
    res.status(number).send(message);
}

