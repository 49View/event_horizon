var AWS = require('aws-sdk');
const globalConfig = require('../config_api.js')

// AWS.config = new AWS.Config();
// AWS.config.accessKeyId = globalConfig.AWSConfigAccessKeyId;
// AWS.config.secretAccessKey = globalConfig.AWSConfigSecretAccessKey;
AWS.config = new AWS.Config();
AWS.config.accessKeyId = "AKIAJWSLLXTAUPNWMBOA";
AWS.config.secretAccessKey = "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";

var s3 = new AWS.S3();

function listInternal ( key, complete ) {
	return new Promise( (resolve, reject) => {
		const key_name = "Key";
		const params = {
			Bucket: globalConfig.S3Bucket,
			Prefix: key
		};

		const p1 = s3.listObjectsV2(params).promise();
		p1.then( (data) => {
			let keys = [];
			const kl = key.length + 1; // +1 here includes slash /
			for ( const k in data["Contents"]) {
				if ( complete ) {
					keys.push(data["Contents"][k][key_name]);
				} else {
					const kd =  data["Contents"][k][key_name];
					const kname = kd.substring(kl, kd.length);
					if ( kname.length > 0 ) keys.push(kname);	
				}
			}
			resolve( keys );
		}).catch( (err) => {
			const err_string = `Cannot list ${key}`;
			console.log(err_string);
			reject( err_string );
		});
	});
}

exports.list = ( key, complete ) => {
	return listInternal( key, complete );
}

exports.upload = ( filedata, key, bucket ) => {
	var params = {
	  Body: filedata,	
	  Bucket: bucket, 
	  Key: key
	 };

	 return s3.putObject(params).promise();
}

exports.get = ( key, bucket ) => {

    var params = {
        Bucket: bucket,
        Key: key
    };
	
    return s3.getObject(params).promise();
}

exports.delete = ( key, bucket ) => {

    var params = {
        Bucket: bucket,
        Key: key
    };
	
    return s3.deleteObject(params).promise();
}

exports.deleteMulti = ( key ) => {
	
	return new Promise( (resolve, reject) => {
		let params = {
			Bucket: "sixthviewfs",
			Delete: { Objects: [] }
		};
	
		const p = listInternal( key, false );
		p.then( data => {
			if ( data ) {
				data.forEach(element => {
					params.Delete.Objects.push( { Key : key + "/" + element } );
				});
				s3.deleteObjects(params).promise();
			}
			// console.log(params);
			resolve( data );			
		}).catch( (err) => {
			const err_string = `Cannot list2 ${key}`;
			console.log(err_string);
			reject( err_string );
		});
	});	
}

exports.rename = ( source, dest, bucket ) => {

	var paramsCopy = {
		Bucket: bucket,
		CopySource: `${bucket}/${source}`,
        Key: dest
    };

	var paramsDelete = {
		Bucket: bucket,
        Key: source
    };

	return s3.copyObject(paramsCopy).promise().then(() => 
		   s3.deleteObject(paramsDelete).promise());
}
