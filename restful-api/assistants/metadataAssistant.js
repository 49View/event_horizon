const md5 = require("md5");

const gsplitTags = source => {
  return source.split(/[\s,._-]+/).map(e => {
    return e.toLowerCase();
  });
};

module.exports = {
  splitTags: source => {
    return gsplitTags(decodeURIComponent(source));
  },

  createMetadata: ( filename, username, useremail ) => {
    let cleanMetadata = {};
    cleanMetadata.name = filename;
    // if (content.mKey) cleanMetadata.name = content.mKey;
    // if (content.name) cleanMetadata.name = content.name;
    // cleanMetadata.hash = md5(content);
    const idate = new Date();
    cleanMetadata.creator = {
        name: username,
        email: useremail
    };
    cleanMetadata.lastUpdatedDate = idate;
    cleanMetadata.creationDate = idate;
    cleanMetadata.accessCount = 0;
    cleanMetadata.unitValue = 0;
    cleanMetadata.absValue = 0;
    cleanMetadata.thumb = "";
    cleanMetadata.tags = gsplitTags(cleanMetadata.name);
    cleanMetadata.deps = [];
    return cleanMetadata;
  },

  udpateMetadata: (cleanMetadata, content) => {
    if (!cleanMetadata) {
      if (content.mKey) metaname = content.mKey;
      if (content.name) metaname = content.name;
      cleanMetadata = {
        name: content.mKey
      };
    }
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
