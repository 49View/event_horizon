const fsController = require("../controllers/fsController");
const express = require("express");
const router = express.Router();
const entityController = require("../controllers/entityController");
const metaAssistant = require("../assistants/metadataAssistant");
const tar = require("tar-stream");
const streams = require("memory-streams");
const zlib = require("zlib");
const Base64 = require("js-base64").Base64;
const JSZip = require("jszip");

router.get("/content/byId/:id", async (req, res, next) => {
  try {
    const entityId = req.params.id;
    const project = req.user.project;
    // console.log("User:", req.user);
    const fileData = await entityController.getEntityContent(entityId, project);
    fsController.writeFile(res, fileData);
  } catch (ex) {
    console.log("ERROR GETTING ENTITY CONTENT BYID: ", ex);
    res.sendStatus(400);
  }
});

router.get("/content/byHash/:hashId", async (req, res, next) => {
  try {
    const project = req.user.project;
    const entity = await entityController.getEntityByHash(
      req.params.hashId,
      project
    );
    const fileData = await entityController.getEntityContent(
      entity._id,
      project
    );
    fsController.writeFile(res, fileData);
  } catch (ex) {
    console.log("ERROR GET CONTENT ENTITY BY HASH: ", ex);
    res.sendStatus(400);
  }
});

router.get("/:group/:tags", async (req, res, next) => {
  try {
    const group = req.params.group;
    const tags = metaAssistant.splitTags(req.params.tags);
    const project = req.user.project;
    //Check existing entity for use project (or public)
    const foundEntities = await entityController.getEntitiesByProjectGroupTags(
      project,
      group,
      tags,
      true,
      1
    );
    if (foundEntities !== null && foundEntities.length > 0) {
      const entity = foundEntities[0];
      const filePath = entityController.getFilePath(
        entity.project,
        entity.group,
        entity.metadata.name
      );
      const fileData = await fsController.cloudStorageEntityGet(filePath);

      // If no deps it's a base resouce, just save the file as it is
      if (entity.metadata.deps === null || entity.metadata.deps.length == 0) {
        fsController.writeFile(res, fileData);
      } else {
        let tarPack = tar.pack();
        let tarDict = [];
        // tarDict.push( { group: entity.group, filename: entity.metadata.name } );
        tarPack.entry({ name: entity.metadata.name }, fileData["Body"]);
        tarDict.push({
          group: entity.group,
          filename: entity.metadata.name,
          hash: entity.metadata.hash
        });
        for (const elementGroup of entity.metadata.deps) {
          for (const element of elementGroup.value) {
            const depArray = await entityController.getEntityDeps(
              entity.project,
              elementGroup.key,
              element
            );
            if (depArray !== null && depArray.length > 0) {
              const dep = depArray[0];
              const depFilePath = entityController.getFilePath(
                entity.project,
                elementGroup.key,
                dep.metadata.name
              );
              const depData = await fsController.cloudStorageEntityGet(
                depFilePath
              );
              tarPack.entry(
                { name: dep.metadata.name, size: depData.ContentLength },
                depData["Body"]
              );
              tarDict.push({
                group: elementGroup.key,
                filename: dep.metadata.name,
                hash: dep.metadata.hash
              });
            }
          }
        }
        tarPack.entry({ name: "catalog" }, JSON.stringify(tarDict));

        tarPack.finalize();
        var writer = new streams.WritableStream();
        tarPack.pipe(writer);
        tarPack.on("end", () => {
          let buff = writer.toBuffer();
          res
            .status(200)
            .set({ "Content-Length": Buffer.byteLength(buff) })
            .send(buff);
        });
      }
    } else {
      res.sendStatus(204);
    }
  } catch (ex) {
    console.log("ERROR GETTING ENTITY CONTENT BYGROUPTAGS: ", ex);
    res.status(400).send(ex);
  }
});

router.get("/metadata/byHash/:hashId", async (req, res, next) => {
  try {
    const project = req.user.project;
    const entity = await entityController.getEntityByHash(
      req.params.hashId,
      project
    );
    res.status(200).send(entity);
  } catch (ex) {
    console.log("ERROR GET METADATA ENTITY BY HASH: ", ex);
    res.sendStatus(400);
  }
});

router.get("/metadata/byGroupTags/:group/:tags", async (req, res, next) => {
  try {
    const group = req.params.group;
    const tags = req.params.tags.split(/[\s,._]+/);
    const project = req.user.project;
    //Check existing entity for use project (or public)
    const foundEntities = await entityController.getEntitiesByProjectGroupTags(
      project,
      group,
      tags,
      false,
      null
    );
    if (foundEntities !== null && foundEntities.length > 0) {
      res.status(200).send(foundEntities);
    } else {
      res.sendStatus(204);
    }
  } catch (ex) {
    console.log("ERROR GETTING ENTITY METADATA BYGROUPTAGS: ", ex);
    res.sendStatus(400);
  }
});

router.get("/metadata/list/:group/:project", async (req, res, next) => {
  try {
    const group = req.params.group;
    const project = req.params.project;
    //Check existing entity for use project (or public)
    const foundEntities = await entityController.getEntitiesOfProjectWithGroup(
      project,
      group
    );
    // const foundEntities = await entityController.getEntitiesByProjectGroupTags(project, group, false, null);
    if (foundEntities !== null && foundEntities.length > 0) {
      res.status(200).send(foundEntities);
    } else {
      res.status(200).json([]);
    }
  } catch (ex) {
    console.log("ERROR GETTING ENTITY METADATA BYGROUP: ", ex);
    res.sendStatus(400);
  }
});

router.put("/metadata/upserthumb/:id", async (req, res, next) => {
  try {
    const entityId = req.params.id;
    res.send(await entityController.upsertThumb(entityId));
  } catch (ex) {
    console.log("ERROR upserthumb: ", ex);
    res.sendStatus(400);
  }
});

router.put("/metadata/upserthumbs/:group/:project", async (req, res, next) => {
  try {
    const group = req.params.group;

    // Check the entity group has a valid rule to how to generate thumbnails
    const gtr = entityController.groupThumbnailCalcRule(group);
    if (gtr === null) {
      res.sendStatus(204);
      return;
    }

    const entitiesId = await entityController.getEntitiesIdOfProjectWithGroup(
      req.params.project,
      group
    );

    for (e of entitiesId) {
      await entityController.upsertThumb(e.id, gtr);
    }
    res.sendStatus(200);
  } catch (ex) {
    console.log("ERROR upserthumbs: ", ex);
    res.sendStatus(400);
  }
});

router.put("/metadata/addtags/:id", async (req, res, next) => {
  try {
    const entityId = req.params.id;
    const tags = req.body.tags;
    res.sendStatus(await entityController.upsertTags(entityId, tags));
  } catch (ex) {
    console.log("ERROR addtags: ", ex);
    res.sendStatus(400);
  }
});

router.post("/", async (req, res, next) => {
  try {
    const newEntity = entityController.addEntity(req);
    const status = newEntity === null ? 204 : 200;
    res.status(status).send(newEntity);
  } catch (ex) {
    console.log("ERROR CREATING ENTITY: ", ex);
    res.sendStatus(400);
  }
});

// this post have a binary body and will automatically create metadata itself
// it will probably come from a daemon so it won't have req.user information
// hence we need to pass them down
router.post(
  "/:filename/:project/:group/:username/:useremail",
  async (req, res, next) => {
    const filename = req.params.filename;
    const group = req.params.group;
    const project = req.params.project;
    const username = req.params.username;
    const useremail = req.params.useremail;

    try {
      const metadata = entityController.createMetadataStartup(
        filename,
        username,
        useremail
      );

      const entity = await entityController.createEntityFromMetadata(
        req.body,
        project,
        group,
        false,
        false,
        metadata,
        true,
        null
      );

      if (entity !== null) {
        res.status(201).json(entity);
        // res.status(201).json({ ETag: data.ETag });
        res.end();
      } else {
        throw "[post.entity] Entity created is null";
      }
    } catch (ex) {
      console.log("ERROR ADDING FILE TO FS: ", ex);
      res.sendStatus(400);
    }
  }
);

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

const decompress = async (zip, project, username, useremail) => {
  let fileList = [];
  let deps = [];
  zip.forEach((relativePath, zipEntry) => {
    if (zipEntry.dir === false) {
      fileList.push(zipEntry);
    }
  });

  for (let zipEntry of fileList) {
    const content = await zipEntry.async("uint8array");
    if (checkFileExtensionsOnEntityGroup("image", zipEntry.name)) {
      const depGruop = "image";

      const metadatadep = entityController.createMetadataStartup(
        getFileName(zipEntry.name),
        username,
        useremail
      );

      const entity = await entityController.createEntityFromMetadata(
        Buffer.from(content),
        project,
        depGruop,
        false,
        false,
        metadatadep,
        false,
        null
      );

      deps.push(entity);
    }
  }

  return deps;
};

router.post("/multizip/:filename/:group", async (req, res, next) => {
  try {
    const project = req.user.project;
    const filename = req.params.filename;
    const group = req.params.group;
    const username = req.user.name;
    const useremail = req.user.email;

    let metadata = entityController.createMetadataStartup(
      filename,
      username,
      useremail
    );

    const zip = await new JSZip().loadAsync(req.body);
    let deps = await decompress(zip, project, username, useremail);

    const material = {
      mKey: filename,
      values: {
        mType: "PN_SH",
        mStrings: [
          {
            key: "diffuseTexture",
            value: deps[0].metadata.hash
          },
          {
            key: "normalTexture",
            value: "normal"
          }
        ]
      }
    };
    metadata.deps = [
      {
        key: "image",
        value: [deps[0].metadata.hash]
      }
    ];

    const presetThumbEntity = await entityController.getEntityByHash(
      deps[0].metadata.hash,
      project
    );
    const presetThumb = presetThumbEntity.metadata.thumb;

    const entity = await entityController.createEntityFromMetadata(
      JSON.stringify(material),
      project,
      group,
      false,
      false,
      metadata,
      true,
      presetThumb
    );

    if (entity !== null) {
      res.status(201).json(entity);
      res.end();
    } else {
      throw "[post.entity] Entity created is null";
    }
  } catch (ex) {
    console.log(ex);
  }
});

router.post("/multi", async (req, res, next) => {
  try {
    const project = req.user.project;
    // const entities = await entityController.createEntitiesFromContainer(project, req.body);
    const containerBody = req.body;
    let container = tar.extract();
    const metadatas = [];
    const entities = [];

    container.on("entry", function(header, stream, next) {
      // console.log( "Header :", header );
      var writer = new streams.WritableStream();
      stream.on("end", function() {
        metadatas.push(writer.toString());
        next();
        container.end();
      });
      stream.pipe(writer);
      stream.resume();
    });

    container.on("error", error => {
      console.log("Stream ERROR");
    });

    container.on("finish", async () => {
      // all entries read
      for (metadata of metadatas) {
        metadata = JSON.parse(metadata);
        const newEntity = await entityController.createEntityFromMetadataToBeCleaned(
          project,
          metadata
        );
        entities.push(newEntity);
      }
      res.status(200).send(JSON.toString(entities));
    });

    const deflatedBody = zlib.inflateSync(new Buffer.from(containerBody));
    var reader = new streams.ReadableStream(deflatedBody);
    reader.pipe(container);
  } catch (ex) {
    console.log("ERROR CREATING ENTITY: ", ex);
    res.status(400).send(ex);
  }
});

router.put("/:id", async (req, res, next) => {
  try {
    const entityId = req.params.id;
    const project = req.user.project;
    //Check existing entity for use project
    const currentEntity = await entityController.getEntityByIdProject(
      project,
      entityId,
      false
    );
    if (currentEntity === null) {
      throw "Invalid entity for user project";
    }
    const group = currentEntity.group;
    const metadata = entityController.getMetadataFromBody(false, false, req);
    const {
      content,
      isPublic,
      isRestricted,
      cleanMetadata
    } = entityController.cleanupMetadata(metadata);
    //If content defined
    if (content !== null) {
      //Check content exists in project and group
      const copyEntity = await entityController.checkFileExists(
        project,
        group,
        cleanMetadata.name
      );
      if (copyEntity !== null && !copyEntity._id.equals(currentEntity._id)) {
        throw "Content already defined";
      } else if (copyEntity === null) {
        //Remove current file from S3
        await fsController.cloudStorageDelete(
          entityController.getFilePath(
            project,
            group,
            currentEntity.metadata.name
          ),
          "eventhorizonentities"
        );
        //Upload file to S3
        const filePath = entityController.getFilePath(
          project,
          group,
          cleanMetadata.name
        );
        await fsController.cloudStorageFileUpload(
          content,
          filePath,
          "eventhorizonentities"
        );
      }
    } else {
      //If content don't change use current name
      cleanMetadata.name = currentEntity.metadata.name;
    }
    //Update entity
    await entityController.updateEntity(
      currentEntity._id,
      project,
      group,
      isPublic,
      isRestricted,
      cleanMetadata
    );

    res.status(204).send();
  } catch (ex) {
    console.log("ERROR UPDATING ENTITY: ", ex);
    res.sendStatus(400);
  }
});

router.delete("/:id", async (req, res, next) => {
  try {
    const entityId = req.params.id;
    const project = req.user.project;
    //Check existing entity for use project
    const currentEntity = await entityController.getEntityByIdProject(
      project,
      entityId,
      false
    );
    if (currentEntity === null) {
      console.log(
        "[INFO] Deletion of " + entityId + " not found, no operations performed"
      );
      res.status(204).send();
    } else {
      for (ktype of currentEntity["metadata"]["deps"]) {
        for (key of ktype["value"]) {
          const currDep = await entityController.getEntityByHash(
            key,
            currentEntity.project
          );
          if (currDep !== null) {
            await entityController.deleteEntityComplete(project, currDep);
          }
        }
      }
      await entityController.deleteEntityComplete(project, currentEntity);
      res.status(201).send();
    }
  } catch (ex) {
    console.log("ERROR DELETING ENTITY: ", ex);
    res.sendStatus(400);
  }
});

router.delete("/hash/:hashId", async (req, res, next) => {
  try {
    const project = req.user.project;
    const currDep = await entityController.getEntityByHash(
      req.params.hashId,
      project
    );
    if (currDep !== null) {
      await entityController.deleteEntityComplete(project, currDep);
    }
    res.status(201).send();
  } catch (ex) {
    console.log("ERROR DELETING ENTITY: ", ex);
    res.sendStatus(400);
  }
});

router.delete("/group/:groupId", async (req, res, next) => {
  try {
    const project = req.user.project;
    const entities = await entityController.getEntitiesOfProjectWithGroup(
      project,
      req.params.groupId
    );
    if (entities === null) {
      res.status(204).send();
    } else {
      for (const entity of entities) {
        await entityController.deleteEntityComplete(project, entity.toObject());
      }
      res.status(201).send();
    }
  } catch (ex) {
    console.log("ERROR DELETING ENTITIES: ", ex);
    res.sendStatus(400);
  }
});

router.delete("/", async (req, res, next) => {
  try {
    const project = req.user.project;
    const entities = await entityController.getEntitiesOfProject(project);
    if (entities === null) {
      res.status(204).send();
    } else {
      for (const entity of entities) {
        await entityController.deleteEntityComplete(project, entity);
      }
      res.status(201).send();
    }
  } catch (ex) {
    console.log("ERROR DELETING ENTITIES: ", ex);
    res.sendStatus(400);
  }
});

module.exports = router;
