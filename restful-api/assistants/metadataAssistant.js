const md5 = require("md5");

module.exports = {
  udpateMetadata: (cleanMetadata, content) => {
    cleanMetadata.hash = md5(content);
    const idate = new Date();
    cleanMetadata.lastUpdatedDate = idate;
    if (!cleanMetadata.creationDate) {
      cleanMetadata.creationDate = idate;
    }
    if (!cleanMetadata.accessCount) cleanMetadata.accessCount = 0;
    if (!cleanMetadata.thumb) cleanMetadata.thumb = "";
    if (!cleanMetadata.tags) cleanMetadata.tags = [];
    if (!cleanMetadata.deps) cleanMetadata.deps = [];
  },

  aggregate: async (model, aggregationQuery) => {
    return new Promise((resolve, reject) => {
      //console.log(aggregationQuery);
      model.aggregate(aggregationQuery, (err, data) => {
        //console.log("RESULT:",data,err);
        if (err) {
          console.log("ERROR in asyncModelOperations.aggregate", err);
          reject(err);
          return;
        }
        //console.log("RESULT:",data);
        resolve(data);
      });
    });
  },
  find: async (model, query) => {
    return new Promise((resolve, reject) => {
      model.find(query, (err, data) => {
        if (err) {
          console.log("ERROR in asyncModelOperations.find", err);
          reject(err);
          return;
        }
        //console.log("RESULT:",data);
        resolve(data);
      });
    });
  }
};
