const fsController = require("../controllers/fsController");
const express = require("express");
const router = express.Router();
const entityController = require("../controllers/entityController");
const metaAssistant = require("../assistants/metadataAssistant");
const tar = require("tar-stream");
const streams = require("memory-streams");
const zlib = require("zlib");
const md5 = require("md5");

router.get("/check/:id", async (req, res, next) => {
  try {
    const entityId = req.params.id;
    const project = req.user.project;
    //Check existing entity for use project
    const currentEntity = await entityController.getEntityByIdProject(
      project,
      entityId,
      false
    );
    res.status(200).send({ found: currentEntity ? true : false });
  } catch (ex) {
    console.log("ERROR CHECKING ENTITY BYID: ", ex);
    res.sendStatus(400);
  }
});

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
      req.params.tags,
      true,
      null
    );
    if (foundEntities !== null && foundEntities.length > 0) {
      let entity = foundEntities[0];
      for (const ent in foundEntities) {
        if (ent.project === project) {
          entity = ent;
          break;
        }
      }

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
                dep.project,
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
    const tags = metaAssistant.splitTags(decodeURIComponent(req.params.tags));
    const project = req.user.project;
    //Check existing entity for use project (or public)
    const foundEntities = await entityController.getEntitiesByProjectGroupTags(
      project,
      group,
      tags,
      req.params.tags,
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

router.post("/placeholder/:group", async (req, res, next) => {
  const project = req.user.project;
  const group = req.params.group;
  const username = req.user.name;
  const useremail = req.user.email;

  try {
    const entity = await entityController.createPlaceHolderEntity(
      project,
      group,
      username,
      useremail
    );

    if (entity !== null) {
      res
        .status(201)
        .json(entity)
        .end();
    } else {
      throw "[post.entity] Entity created is null";
    }
  } catch (ex) {
    console.log("[POST] Entity error: ", ex);
    res.sendStatus(400);
  }
});

router.post("/:group/:filename", async (req, res, next) => {
  const filename = req.params.filename;
  const group = req.params.group;
  const project = req.user.project;
  const username = req.user.name;
  const useremail = req.user.email;

  try {
    const entity = await entityController.createEntityFromMetadata(
      req.body,
      project,
      group,
      true,
      false,
      entityController.createMetadataStartup(filename, username, useremail),
      true,
      null
    );

    if (entity !== null) {
      res
        .status(201)
        .json(entity)
        .end();
    } else {
      throw "[post.entity] Entity created is null";
    }
  } catch (ex) {
    console.log("[POST] Entity error: ", ex);
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
    const username = decodeURIComponent(req.params.username);
    const useremail = decodeURIComponent(req.params.useremail);

    try {
      const entity = await entityController.createEntityFromMetadata(
        req.body,
        project,
        group,
        true,
        false,
        entityController.createMetadataStartup(filename, username, useremail),
        true,
        null
      );

      if (entity !== null) {
        res
          .status(201)
          .json(entity)
          .end();
      } else {
        throw "[post.entity] Entity created is null";
      }
    } catch (ex) {
      console.log("[POST] Entity error: ", ex);
      res.sendStatus(400);
    }
  }
);

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

    const deps = await entityController.decompressZipppedEntityDeps(
      req.body,
      project,
      username,
      useremail
    );

    const material = {
      mKey: filename,
      values: {
        mType: "PN_SH",
        mStrings: deps["mStrings"]
      }
    };
    metadata.deps = [
      {
        key: "image",
        value: deps["deps"]
      }
    ];

    const presetThumb = deps["diffuseTexture"].metadata.thumb;

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

router.put("/makepublic/all", async (req, res, next) => {
  try {
    res.send(await entityController.makePublicAll());
  } catch (ex) {
    console.log("[ERROR] makepublic/all: ", ex);
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
      res.status(204).send();
      return;
    }
    const group = currentEntity.group;
    let content = req.body;
    var contype = req.headers["content-type"];
    if (contype.indexOf("application/json") !== -1) {
      content = Buffer.from(JSON.stringify(content));
    }

    //If content defined
    if (content !== null) {
      const origFilePath = entityController.getFilePath(
        project,
        group,
        currentEntity.metadata.name
      );
      const tempDeleteFilePath = origFilePath + ".delete";
      // Remove current file from S3
      await fsController.cloudStorageRename(
        origFilePath,
        tempDeleteFilePath,
        "eventhorizonentities"
      );
      //Upload file to S3
      const filePath = entityController.getFilePath(
        project,
        group,
        currentEntity.metadata.name
      );
      await fsController.cloudStorageFileUpload(
        content,
        filePath,
        "eventhorizonentities"
      );
      await fsController.cloudStorageDelete(
        tempDeleteFilePath,
        "eventhorizonentities"
      );

      const metadata = {
        ...currentEntity.metadata,
        hash: md5(content),
        lastUpdatedDate: new Date()
      };

      //Update entity
      await entityController.updateEntity(currentEntity._id, metadata);
    }

    res.status(200).send();
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
      true
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
