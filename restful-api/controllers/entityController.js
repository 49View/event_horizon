const mongoose = require("mongoose");
const zlib = require("zlib");
const entityModel = require("../models/entity");
const remapModel = require("../models/remap");
const asyncModelOperations = require("../assistants/asyncModelOperations");
const fsController = require("../controllers/fsController");
const socketController = require("../controllers/socketController");
const metadataAssistant = require("../assistants/metadataAssistant");
const tar = require("tar-stream");
const streams = require("memory-streams");
const sharp = require("sharp");
const JSZip = require("jszip");
const { uniqueNamesGenerator } = require("unique-names-generator");
const md5 = require("md5");

const getMetadataFromBody = (checkGroup, checkRaw, req) => {
  if (req.body === null || !(req.body instanceof Object)) {
    throw "Invalid metadata for entity";
  }
  const metadata = req.body;
  const metadataMissingMessage = "Required attributes missing in metadata:";
  //Check that body contains special attributes used for define entity
  if (checkGroup && typeof metadata.group === "undefined") {
    throw metadataMissingMessage + " 'Group'";
  }
  if (checkRaw && typeof metadata.raw === "undefined") {
    throw metadataMissingMessage + " 'Raw'";
  }
  if (
    typeof metadata.tags === "undefined" ||
    !(metadata.tags instanceof Array) ||
    metadata.tags.length === 0
  ) {
    throw metadataMissingMessage + " 'tags'";
  }
  return metadata;
};

const createMetadataStartup = (filename, username, useremail) => {
  const tags = metadataAssistant.splitTags(filename);
  return (metadata = {
    creator: {
      name: username,
      email: useremail
    },
    name: filename,
    thumb: "",
    tags: tags,
    deps: []
  });
};

const createEntityFromMetadata = async (
  content,
  project,
  group,
  isPublic,
  isRestricted,
  cleanMetadata,
  sendBroadcast,
  presetThumb = null
) => {
  try {
    let filePath = getFilePath(project, group, cleanMetadata.name);
    //Check content exists in project and group
    const copyEntity = await checkFileExists(
      project,
      group,
      cleanMetadata.hash
    );
    if (copyEntity === null) {
      //Upload file to S3
      // console.log( "Adding: ", filePath );
      let savedFilename = { changed: false, name: filePath };
      await fsController.cloudStorageGetFilenameAndDuplicateIfExists(
        filePath,
        "eventhorizonentities",
        savedFilename
      );
      if (savedFilename["changed"] == true) {
        const entityToDelete = await getEntityByName(
          project,
          group,
          cleanMetadata.name
        );
        await deleteEntityComplete(project, entityToDelete);
      }

      cleanMetadata.thumb = await thumbFromContent(
        content,
        presetThumb,
        groupThumbnailCalcRule(group)
      );

      // Hashing of content
      metadataAssistant.udpateMetadata(cleanMetadata, content);

      // filePath = savedFilename["name"];
      await fsController.cloudStorageFileUpload(
        content,
        filePath,
        "eventhorizonentities"
      );

      //Create entity
      let entity = await createEntity(
        project,
        group,
        isPublic,
        isRestricted,
        cleanMetadata
      );

      if (entity !== null) {
        let json = {
          msg: "entityAdded",
          data: entity
        };
        if (sendBroadcast) {
          socketController.sendMessageToAllClients(JSON.stringify(json));
        }
      }

      return entity;
    }
  } catch (error) {
    console.log("Cannot create entity: ", error);
    return null;
  }
};

const createPlaceHolderEntity = async (project, group, username, useremail) => {
  try {
    // Hashing of content
    const filename = uniqueNamesGenerator();
    let metadata = createMetadataStartup(filename, username, useremail);
    metadata.hash = md5(metadata.name);
    const idate = new Date();
    metadata.lastUpdatedDate = idate;
    metadata.creationDate = idate;

    return createEntityModel(project, group, false, false, metadata).toObject();
  } catch (error) {
    console.log("Cannot create entity: ", error);
    return null;
  }
};

const createEntityFromMetadataToBeCleaned = async (project, metadata) => {
  const {
    content,
    group,
    isPublic,
    isRestricted,
    cleanMetadata
  } = cleanupMetadata(metadata);

  return await createEntityFromMetadata();
};

const createEntitiesFromContainer = async (project, containerBody) => {
  let container = tar.extract();
  const metadatas = [];
  const entities = [];

  container.on("entry", function(header, stream, next) {
    console.log(header.name);
    var writer = new streams.WritableStream();
    stream.pipe(writer);
    stream.on("end", function() {
      console.log(writer.toString());
      metadatas.push(writer.toString());
      next(); // ready for next entry
    });
    stream.resume(); // just auto drain the stream
  });

  const deflatedBody = zlib.inflateSync(new Buffer.from(containerBody));

  var reader = new streams.ReadableStream(deflatedBody);
  await reader.pipe(container);
  console.log("**************");
  reader.on("finish", () => {
    console.log("############### ");
    // all entries read
    // res.status(200).send(null);
  });

  metadatas.forEach(element => {
    console.log("############### ");
    // const newEntity = await createEntityFromMetadata( project, element );
    // if ( newEntity !== null ) entities.push(newEntity);
  });

  return entities;
  // console.log(deflatedBody);
};

const cleanupMetadata = metadata => {
  const result = {};

  if (typeof metadata.raw !== "undefined") {
    try {
      result.content = zlib.inflateSync(
        new Buffer.from(metadata.raw, "base64")
      );
    } catch (error) {
      result.content = new Buffer.from(metadata.raw, "base64");
    }
  } else {
    result.content = null;
  }
  result.group = metadata.group;
  result.keys = metadata.tags;
  result.isPublic = metadata.isPublic || false;
  result.isRestricted = metadata.isRestricted || false;
  //Remove service attributes
  delete metadata.raw;
  delete metadata.group;
  delete metadata.isPublic;
  delete metadata.isRestricted;
  //Add hash attribute
  result.cleanMetadata = metadata;

  return result;
};

const getFilePath = (project, group, name) => {
  return project + "/" + group + "/" + name;
};

const checkFileExists = async (project, group, hash) => {
  const query = {
    $and: [{ "metadata.hash": hash }, { group: group }, { project: project }]
  };
  const result = await entityModel.findOne(query);
  return result !== null ? result.toObject() : null;
};

const createEntityModel = (
  project,
  group,
  isPublic,
  isRestricted,
  metadata
) => {
  return new entityModel({
    project: project,
    group: group,
    isPublic: isPublic,
    isRestricted: isRestricted,
    metadata: metadata
  });
};

const createEntity = async (
  project,
  group,
  isPublic,
  isRestricted,
  metadata
) => {
  const newEntityDB = createEntityModel(
    project,
    group,
    isPublic,
    isRestricted,
    metadata
  );
  await newEntityDB.save();
  return newEntityDB.toObject();
};

const updateEntity = async (entityId, metadata) => {
  const query = { _id: mongoose.Types.ObjectId(entityId) };

  await entityModel.updateOne(query, {
    metadata: metadata
  });
};

const deleteEntity = async entityId => {
  await entityModel.deleteOne({ _id: mongoose.Types.ObjectId(entityId) });
};

const deleteEntityComplete = async (project, entity) => {
  if (entity) {
    currentEntity = entity;
    console.log("[INFO] deleting entity " + currentEntity.metadata.name);
    const group = currentEntity.group;
    //Remove current file from S3
    await fsController.cloudStorageDelete(
      module.exports.getFilePath(project, group, currentEntity.metadata.name),
      "eventhorizonentities"
    );
    //Delete existing entity
    await module.exports.deleteEntity(currentEntity._id);
  }
};

const getEntityByIdProject = async (project, entityId, returnPublic) => {
  let query;
  if (returnPublic) {
    query = {
      $and: [
        { _id: mongoose.Types.ObjectId(entityId) },
        { $or: [{ project: project }, { isPublic: true }] }
      ]
    };
  } else {
    query = {
      $and: [{ _id: mongoose.Types.ObjectId(entityId) }, { project: project }]
    };
  }
  const result = await entityModel.findOne(query);

  return result !== null ? result.toObject() : null;
};

const makePublicAll = async () => {
  let query;
  query = {};
  const result = await entityModel.updateMany(query, {
    $set: { isPublic: true }
  });

  return result;
};

const updateById = async (entityId, updatedEntity) => {
  let query;
  query = { _id: entityId };
  const result = await entityModel.findOneAndUpdate(query, updatedEntity);

  return result !== null ? result.toObject() : null;
};

const getFileName = pathname => {
  return pathname
    .split("\\")
    .pop()
    .split("/")
    .pop();
};

const checkCommonFileExtension = (group, ext) => {
  if (group === "geom") {
    if (ext === "glb" || ext === "fbx") return true;
  } else if (group === "material") {
    if (ext === "zip") return true;
  } else if (group === "image") {
    if (
      ext === "jpeg" ||
      ext === "png" ||
      ext === "jpg" ||
      ext === "exr" ||
      ext === "tga" ||
      ext === "tiff" ||
      ext === "gif"
    ) {
      return true;
    }
  }

  return false;
};

const checkFileExtensionsOnEntityGroup = (group, filename) => {
  const ext = filename
    .split(".")
    .pop()
    .toLowerCase();

  return checkCommonFileExtension(group, ext);
};

const array_intersection = (tags, against) => {
  const res = tags.filter(value => against.includes(value));
  return res.length > 0;
};

const arrangePBRness = tags => {
  const textureCheckMap = {
    hasNormal: ["normal"],
    hasMetallic: ["metallic", "metalness"],
    hasRoughness: ["roughness"],
    hasOpacity: ["opacity"],
    hasAO: ["ambientocclusion", "ao", "ambient", "occlusion"],
    hasHeight: ["height"],
    hasTranslucency: ["translucency"]
  };

  if (array_intersection(tags, textureCheckMap.hasNormal))
    return "normalTexture";
  if (array_intersection(tags, textureCheckMap.hasMetallic))
    return "metallicTexture";
  if (array_intersection(tags, textureCheckMap.hasRoughness))
    return "roughnessTexture";
  if (array_intersection(tags, textureCheckMap.hasOpacity))
    return "opacityTexture";
  if (array_intersection(tags, textureCheckMap.hasAO)) return "aoTexture";
  if (array_intersection(tags, textureCheckMap.hasHeight))
    return "heightTexture";
  if (array_intersection(tags, textureCheckMap.hasTranslucency))
    return "translucencyTexture";
  return "diffuseTexture";
};

const decompressZipppedEntityDeps = async (
  zipFile,
  project,
  username,
  useremail
) => {
  const zip = await new JSZip().loadAsync(zipFile);

  let fileList = [];
  zip.forEach((relativePath, zipEntry) => {
    if (zipEntry.dir === false) {
      if (checkFileExtensionsOnEntityGroup("image", relativePath)) {
        const tags = metadataAssistant.splitTags(getFileName(relativePath));
        fileList.push({ type: arrangePBRness(tags), entry: zipEntry });
      }
    }
  });

  let deps = {};
  deps["mStrings"] = [];
  deps["deps"] = [];
  for (let zipEntry of fileList) {
    const content = await zipEntry.entry.async("uint8array");
    const depGruop = "image";

    const metadatadep = createMetadataStartup(
      getFileName(zipEntry.entry.name),
      username,
      useremail
    );

    // Resize image to a decente size
    const decentSize = 512;
    const fullSizeImage = Buffer.from(content);
    const scaledDown = await sharp(fullSizeImage)
      .resize(decentSize, decentSize, {
        fit: "inside",
        withoutEnlargement: true
      })
      .toFormat("png")
      .toBuffer();
    const entity = await createEntityFromMetadata(
      scaledDown,
      project,
      depGruop,
      false,
      false,
      metadatadep,
      false,
      null
    );

    deps[zipEntry.type] = entity;
    deps["mStrings"].push({
      key: zipEntry.type,
      value: entity.metadata.hash
    });
    deps["deps"].push(entity.metadata.hash);
  }

  return deps;
};

const gtr_dep0_image = "dep0";
const gtr_content_image = "content";
const gtr_content_vector = "content_vector";
const gtr_content_color = "content_color";
const gtr_content_default = "content_default";

const groupThumbnailCalcRule = group => {
  let contentType = null;
  if (group === "material") {
    contentType = gtr_dep0_image;
  } else if (group === "image") {
    contentType = gtr_content_image;
  } else if (group === "color") {
    contentType = gtr_content_color;
  } else if (group === "profile") {
    contentType = gtr_content_vector;
  } else if (group === "geom") {
    contentType = gtr_content_default;
  }
  return contentType;
};

const groupThumbnailSourceContent = async (entity, gtr) => {
  try {
    if (gtr === gtr_content_image || gtr === gtr_content_vector) {
      return await getEntityContent(entity._id, entity.project);
    } else if (gtr === gtr_dep0_image) {
      for (element of entity.metadata.deps) {
        if (element.key === "image") {
          const dep0Entity = await getEntityByHash(
            element.value[0],
            entity.project
          );
          return await getEntityContent(dep0Entity._id, dep0Entity.project);
        }
      }
    } else if (gtr === gtr_content_default) {
      return "";
    }
  } catch (error) {
    console.log("groupThumbnailSourceContent failed. Reason: " + error);
    return null;
  }
};

const thumbFromContent = async (content, presetThumb, gtr) => {
  let thumbBuff = null;
  if (gtr === gtr_content_vector) {
    thumbBuff = content;
  } else if (gtr === gtr_content_image) {
    try {
      thumbBuff = await sharp(content)
          .resize(64, 64, { fit: "inside", withoutEnlargement: true })
          .toFormat("jpg")
          .toBuffer();
    } catch (e) {
      console.log( e );
    }
  } else if (gtr === gtr_content_color) {
    const cp = JSON.parse(content);
    thumbBuff = await sharp({
      create: {
        width: 1,
        height: 1,
        channels: 4,
        background: {
          r: cp.color[0] * 255.0,
          g: cp.color[1] * 255.0,
          b: cp.color[2] * 255.0,
          alpha: cp.color[3]
        }
      }
    })
      .png()
      .toBuffer();
  } else if (gtr === gtr_dep0_image) {
    thumbBuff = presetThumb;
  }
  return thumbBuff === null ? "" : thumbBuff.toString("base64");
};

const upsertThumb = async entityId => {
  const entity = await getEntityById(entityId);
  if (entity.metadata.thumb.length == 0) {
    try {
      const gtr = groupThumbnailCalcRule(entity.group);
      const content = await groupThumbnailSourceContent(entity, gtr);
      entity.metadata.thumb = await thumbFromContent(content.Body, null, gtr);
      return await updateById(entityId, entity);
    } catch (error) {
      console.log("Upsert thumb on id " + entityId + " failed. Cause:" + error);
      return null;
    }
  }
  return {};
};

const upsertTags = async (entityId, tags) => {
  try {
    const entity = await getEntityById(entityId);
    entity.metadata.tags = tags;
    await updateById(entityId, entity);
    return 201;
  } catch (error) {
    console.log("Upsert tags on id " + entityId + " failed. Cause:" + error);
    return 204;
  }
  return 204;
};

const getEntityContent = async (entityId, project) => {
  //Check existing entity for use project (or public)
  const currentEntity = await getEntityByIdProject(project, entityId, true);
  if (currentEntity === null) {
    throw "Invalid entity for user project";
  }
  const filePath = getFilePath(
    currentEntity.project,
    currentEntity.group,
    currentEntity.metadata.name
  );
  return await fsController.cloudStorageEntityGet(filePath);
};

const getEntityById = async entityId => {
  let query;
  query = { _id: entityId };
  const result = await entityModel.findOne(query);

  return result !== null ? result.toObject() : null;
};

const getEntityByHash = async (entityId, project) => {
  let query;
  query = { project: project, "metadata.hash": entityId };
  const result = await entityModel.findOne(query);

  return result !== null ? result.toObject() : null;
};

const getEntityByName = async (project, group, name) => {
  let query;
  query = { project: project, group: group, "metadata.name": name };
  const result = await entityModel.findOne(query);

  return result !== null ? result.toObject() : null;
};

const getEntitiesOfProject = async (project, returnPublic) => {
  let query;
  if (returnPublic) {
    query = [{ project: project }, { isPublic: true }];
  } else {
    query = { project: project };
  }
  const result = await entityModel.find(query);

  return result !== null ? result : null;
};

const getEntitiesOfProjectWithGroup = async (
  project,
  groupID,
  returnPublic
) => {
  let query;
  if (returnPublic) {
    query = { project: project, group: groupID, isPublic: true };
  } else {
    query = { project: project, group: groupID };
  }
  const result = await entityModel.find(query);

  return result !== null ? result : null;
};

const getEntitiesIdOfProjectWithGroup = async (project, groupID) => {
  const query = { project: project, group: groupID };
  const result = await entityModel.find(query, "id");

  return result !== null ? result : null;
};

const getEntitiesByProjectGroupTags = async (
  project,
  group,
  tags,
  fullName,
  fullData,
  randomElements
) => {
  const aggregationQueries = [
    {
      $match: {
        $and: [
          { isRestricted: false },
          { group: group },
          {
            $or: [{ project: project }, { isPublic: true }]
          },
          {
            $or: [
              {
                "metadata.tags": {
                  $all: tags
                }
              },
              { "metadata.name": tags[0] },
              { "metadata.name": fullName },
              {
                _id:
                  tags[0].length == 12 || tags[0].length == 24
                    ? mongoose.Types.ObjectId(tags[0])
                    : "DDDDDDDDDDDD"
              }
            ]
          }
        ]
      }
    }
  ];

  if (fullData !== true) {
    aggregationQueries.push({
      $project: {
        project: 0,
        isPublic: 0,
        isRestricted: 0
      }
    });
  }

  if (randomElements !== null) {
    aggregationQueries.push({
      $sample: {
        size: randomElements
      }
    });
  }

  const result = await asyncModelOperations.aggregate(
    entityModel,
    aggregationQueries
  );

  return result;
};

const getEntityDeps = async (project, group, deps) => {
  const aggregationQueries = [
    {
      $match: {
        $and: [
          { isRestricted: false },
          { group: group },
          {
            $or: [{ project: project }, { isPublic: true }]
          },
          {
            "metadata.hash": deps
          }
        ]
      }
    }
  ];

  const result = await asyncModelOperations.aggregate(
    entityModel,
    aggregationQueries
  );

  return result;
};

exports.postMultiZip = async (
  filename,
  group,
  body,
  project,
  username,
  useremail
) => {
  try {
    let metadata = createMetadataStartup(filename, username, useremail);

    const deps = await decompressZipppedEntityDeps(
      body,
      project,
      username,
      useremail
    );

    const material = {
      mKey: filename,
      values: {
        mType: "PN_SH",
        mStrings: deps["mStrings"],
        mFloats: [ {key:"metallic", value: 0.5} ]
      }
    };
    metadata.deps = [
      {
        key: "image",
        value: deps["deps"]
      }
    ];

    const presetThumb = deps["diffuseTexture"].metadata.thumb;

    return await createEntityFromMetadata(
      JSON.stringify(material),
      project,
      group,
      true,
      false,
      metadata,
      true,
      presetThumb
    );
  } catch (ex) {
    console.log(ex);
    return null;
  }
};

const remapSave = async (data, project) => {
  try {
    data.project = project;
    data.hash = md5(
      project + data.sourceEntity + data.sourceRemap + data.destRemap
    );
    return await remapModel.findOneAndUpdate({ hash: data.hash }, data, {
      upsert: true,
      new: true,
      setDefaultsOnInsert: true
    });
  } catch (ex) {
    console.log("[ERROR] entityController.remapSave \n", ex);
  }
};

const remap = (project, data) => {
  try {
    if (Array.isArray(data)) {
      let rets = [];
      for (let d of data) {
        rets.push(remapSave(d, project));
      }
      return rets;
    } else {
      return remapSave(data, project);
    }
  } catch (ex) {
    console.log("[ERROR] entityController.remap \n", ex);
    return null;
  }
};

const getEntitiesRemap = async (project, entities) => {
  try {
    const entitiesArray = Array.isArray(entities.entities) ? entities.entities : [entities.entities];
    let reparsedArray = [];
    let kvMap = [];
    for ( const entry of entitiesArray ) {
      const tags = metadataAssistant.splitTags(entry);
      const ret = await getEntitiesByProjectGroupTags(project, "geom", tags, entry, false, null );
      if ( ret && ret.length > 0 ) {
        const kname = ret[0].metadata.name;
        reparsedArray.push(kname);
        kvMap.push( {
          key: kname,
          value: entry
        })
      }
    }
    let remaps = await remapModel.find({
      $and: [{ project: project }, { sourceEntity: { $in: reparsedArray } }]
    });
    const ret = {
      kv: kvMap,
      remaps: remaps
    };
    console.log('Remaps: ', ret );
    return ret;
  } catch (ex) {
    console.log("[ERROR] entityController.remap \n", ex);
    console.log( "Parameters: \n Project: ", project, " entities: ", entities );
    return null;
  }
};

function RGBAToHexA(r, g, b, a) {
  r = Math.round(r * 255).toString(16);
  g = Math.round(g * 255).toString(16);
  b = Math.round(b * 255).toString(16);
  a = Math.round(a * 255).toString(16);

  if (r.length == 1) r = "0" + r;
  if (g.length == 1) g = "0" + g;
  if (b.length == 1) b = "0" + b;
  if (a.length == 1) a = "0" + a;

  return "#" + r + g + b + a;
}

module.exports = {
  createMetadataStartup: createMetadataStartup,
  getMetadataFromBody: getMetadataFromBody,
  createEntityFromMetadata: createEntityFromMetadata,
  createPlaceHolderEntity: createPlaceHolderEntity,
  createEntitiesFromContainer: createEntitiesFromContainer,
  cleanupMetadata: cleanupMetadata,
  decompressZipppedEntityDeps: decompressZipppedEntityDeps,
  getFilePath: getFilePath,
  getEntityContent: getEntityContent,
  checkFileExists: checkFileExists,
  createEntity: createEntity,
  updateById: updateById,
  makePublicAll: makePublicAll,
  updateEntity: updateEntity,
  upsertTags: upsertTags,
  groupThumbnailCalcRule: groupThumbnailCalcRule,
  upsertThumb: upsertThumb,
  thumbFromContent: thumbFromContent,
  deleteEntity: deleteEntity,
  deleteEntityComplete: deleteEntityComplete,
  getEntityByIdProject: getEntityByIdProject,
  getEntitiesIdOfProjectWithGroup: getEntitiesIdOfProjectWithGroup,
  getEntityById: getEntityById,
  getEntityByHash: getEntityByHash,
  getEntityByName: getEntityByName,
  getEntitiesOfProject: getEntitiesOfProject,
  getEntitiesOfProjectWithGroup: getEntitiesOfProjectWithGroup,
  getEntityDeps: getEntityDeps,
  getEntitiesByProjectGroupTags: getEntitiesByProjectGroupTags,
  getEntitiesRemap: getEntitiesRemap,
  remap: remap
};
