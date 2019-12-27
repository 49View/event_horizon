#include <stb/stb_image_write.h>

#include <core/http/webclient.h>
#include <core/util.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <cmath>
#include <core/file_manager.h>
#include <core/runloop_core.h>
#include <core/tar_util.h>
#include <core/zlib_util.h>
#include <core/names.hpp>
#include <core/resources/publisher.hpp>
#include <core/resources/material.h>
#include <core/resources/resource_utils.hpp>
#include <core/descriptors/uniform_names.h>
#include <core/resources/resource_builder.hpp>
#include <core/resources/resource_pipe.hpp>
#include <core/resources/publisher.hpp>

#include <database/nosql/mongo/mongo.hpp>

#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <core/image_util.h>
#include <core/profiler.h>
#include <ostream>
#include <zip.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

// mongod --port 27017 --replSet rs0
// sudo brew services restart nginx
// /usr/loca/var/log/nginx/ tail -f aacess.log

void initDeamon() {
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }
    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

class DaemonException : public std::exception {
public:
    DaemonException(const std::string &msg) : msg(msg) {}

    virtual const char *what() const throw() {
        return msg.c_str();
    }
private:
    std::string msg{};
};

void daemonExceptionLog(const std::exception &e) {
    LOGRS(e.what());
    Socket::emit("daemonLogger", serializeLogger( LoggerLevel::Error, e.what()));
}

std::optional<MongoFileUpload> saveImageToGridFS(MongoBucket bucket, const char *filename,
                                                 int desiredWidth, int desiredHeight,
                                                 strview project,
                                                 strview uname,
                                                 strview uemail,
                                                 std::vector<std::string> &thumbs) {
    try {
        int w = 0, h = 0, n = 0;
        int thumbSize = 128;

        unsigned char *input_data = stbi_load(filename, &w, &h, &n, 0);

        auto output_data = make_uint8_p(desiredWidth * desiredHeight * n);
        auto output_data_thumb = make_uint8_p(thumbSize * thumbSize * n);
        stbir_resize_uint8(input_data, w, h, 0, output_data.first.get(), desiredWidth, desiredHeight, 0, n);
        stbir_resize_uint8(output_data.first.get(), desiredWidth, desiredHeight, 0, output_data_thumb.first.get(),
                           thumbSize, thumbSize, 0, n);

        auto bm = imageUtil::bufferToPngMemory(desiredWidth, desiredHeight, n, output_data.first.get());
        auto bm64 = imageUtil::bufferToPng64(thumbSize, thumbSize, n, output_data_thumb.first.get());
        thumbs.emplace_back(bm64);

        return Mongo::fileUpload(bucket, getFileName(filename), std::move(bm),
                                 Mongo::FSMetadata(ResourceGroup::Image, project, uname, uemail,
                                                   HttpContentType::octetStream,
                                                   MD5(bm.first.get(), bm.second).hexdigest(), bm64));
    } catch (const std::exception &e) {
        LOGRS(e.what());
        return std::nullopt;
    }
}


static int
get_data(void **datap, size_t *sizep, const char *archive) {
    /* example implementation that reads data from file */
    struct stat st;
    FILE *fp;

    if ((fp = fopen(archive, "r")) == NULL) {
        if (errno != ENOENT) {
            fprintf(stderr, "can't open %s: %s\n", archive, strerror(errno));
            return -1;
        }

        *datap = NULL;
        *sizep = 0;

        return 0;
    }

    if (fstat(fileno(fp), &st) < 0) {
        fprintf(stderr, "can't stat %s: %s\n", archive, strerror(errno));
        fclose(fp);
        return -1;
    }

    if ((*datap = malloc((size_t)st.st_size)) == NULL) {
        fprintf(stderr, "can't allocate buffer\n");
        fclose(fp);
        return -1;
    }

    if (fread(*datap, 1, (size_t)st.st_size, fp) < (size_t)st.st_size) {
        fprintf(stderr, "can't read %s: %s\n", archive, strerror(errno));
        free(*datap);
        fclose(fp);
        return -1;
    }

    fclose(fp);

    *sizep = (size_t)st.st_size;
    return 0;
}



void elaborateMatSBSAR(MongoBucket entity_bucket,
                       const std::string &mainFileName,
                       const std::string &layerName,
                       int size, strview project,
                       strview uname,
                       strview uemail) {

    if (mainFileName.empty()) return;
    auto fileRoot = getDaemonRoot();

    std::string fn = getFileNameOnly(mainFileName);
    std::string fext = ".png";
    int nominalSize = 512;

    std::string sizeString = std::to_string(log2(size));
    std::string sbRender = "/opt/Allegorithmic/Substance_Automation_Toolkit/sbsrender render --inputs "
                           + mainFileName +
                           " --set-value '$outputsize@" + sizeString + "," + sizeString +
                           "' --output-bit-depth \"8\" --png-format-compression best_compression "
                           "--output-name {inputName}_{outputNodeName}";
    for (const auto &output : MPBRTextures::SBSARTextureOutputs()) {
        sbRender.append(" --input-graph-output " + output);
    }
    sbRender.append(" --output-path " + fileRoot);;

    std::system(sbRender.c_str());

    // Gather texture outputs
    ResourceDependencyDict deps;
    std::vector<std::string> thumbs;
    Material pbrmat{S::SH, fn};
    for (const auto &output : MPBRTextures::SBSARTextureOutputs()) {
        auto bfilename = fileRoot + fn;
        bfilename.append("_").append(output).append(".png");
        if (FM::fileExist(bfilename)) {
            auto to = saveImageToGridFS(entity_bucket, bfilename.c_str(), nominalSize, nominalSize, project, uname,
                                        uemail, thumbs);
            if (to) {
                auto tid = (*to).getStringId();
                deps[ResourceGroup::Image].emplace_back(tid);
                pbrmat.Values()->assign(MPBRTextures::mapToTextureUniform(output), tid);
            }
        }
    }

    // Create PBR material
    auto matSer = pbrmat.serialize();
    Mongo::fileUpload(entity_bucket, fn, matSer,
                      Mongo::FSMetadata(ResourceGroup::Material, project, uname, uemail,
                                        HttpContentType::json, Hashable<>::hashOf(matSer), thumbs[0], deps));

    // Clean up
    std::string cleanup = "cd " + fileRoot + " && rm " + fn + "*";
    std::system(cleanup.c_str());
}

void elaborateGeomFBX(MongoBucket entity_bucket, const std::string &_filename, strview project,
                      strview uname,
                      strview uemail) {
    try {
        auto dRoot = getDaemonRoot();
        auto filenameEscaped = _filename;
        replaceAllStrings(filenameEscaped, "(", "\\(");
        replaceAllStrings(filenameEscaped, ")", "\\)");

        auto filenameSanitized = _filename;
        replaceAllStrings(filenameSanitized, "(", "_");
        replaceAllStrings(filenameSanitized, ")", "_");

        auto ret = std::system(std::string{"mv " + filenameEscaped + " " + filenameSanitized}.c_str());
        LOGRS("FBX sanity renamed return code: " << ret);
        auto fn = getFileNameOnly(filenameSanitized);
        std::string filenameglb = fn + ".glb";

        std::string cmd =
                "cd " + dRoot + " && FBX2glTF -b --compute-normals always --pbr-metallic-roughness -o " + filenameglb + " " +
                getFileName(filenameSanitized);
        ret = std::system(cmd.c_str());
        if (ret != 0) throw DaemonException{std::string{"FBX elaboration return code: " + std::to_string(ret)}};

        auto fileData = FM::readLocalFile(dRoot + filenameglb);
        auto fileHash = Hashable<>::hashOf(fileData);
        Mongo::fileUpload(entity_bucket, filenameglb, std::move(fileData),
                          Mongo::FSMetadata(ResourceGroup::Geom, project, uname, uemail,
                                            HttpContentType::json, fileHash, "", ResourceDependencyDict{}));
    } catch (const std::exception &e) {
        daemonExceptionLog(e);
    }
}

void parseElaborateStream(mongocxx::change_stream &stream, MongoBucket sourceAssetBucket, MongoBucket entityBucket) {
    for (auto change : stream) {
        StreamChangeMetadata meta{change};
        Profiler p1{"elaborate time:"};
        auto fileDownloaded = Mongo::fileDownload(sourceAssetBucket,
                                                  meta.id,
                                                  getDaemonRoot() + std::string{meta.filename});
        if (meta.group == ResourceGroup::Material) {
            if (getFileNameExt(std::string(meta.filename)) == ".sbsar") {
                elaborateMatSBSAR(entityBucket, fileDownloaded, {}, 512, meta.project,
                                  meta.username,
                                  meta.useremail);
            }
        } else if (meta.group == ResourceGroup::Geom) {
            if (getFileNameExt(std::string(meta.filename)) == ".zip") {
                void *data;
                size_t size;
                zip_source_t *src;
                zip_t *za;
                zip_error_t error;
                /* get buffer with zip archive inside */
                if (get_data(&data, &size, fileDownloaded.c_str()) < 0) {
                    LOGRS("Bollox");
                }
                zip_error_init(&error);
                /* create source from buffer */
                if ((src = zip_source_buffer_create(data, size, 1, &error)) == NULL) {
                    fprintf(stderr, "can't create source: %s\n", zip_error_strerror(&error));
                    free(data);
                    zip_error_fini(&error);
                    LOGRS("Bollox2");
                }

                /* open zip archive from source */
                if ((za = zip_open_from_source(src, 0, &error)) == NULL) {
                    fprintf(stderr, "can't open zip from source: %s\n", zip_error_strerror(&error));
                    zip_source_free(src);
                    zip_error_fini(&error);
                    LOGRS("Bollox3");
                }

                auto numFiles = zip_get_num_entries( za, ZIP_FL_UNCHANGED );
                LOGRS( "NumFiles: " <<  numFiles );
                for ( auto t = 0; t < numFiles; t++ ) {
                    auto zFile = zip_fopen_index( za, t, ZIP_FL_UNCHANGED );
                    zip_stat_t zInfo;
                    zip_stat_index( za, t, ZIP_FL_UNCHANGED, &zInfo );
                    auto buff = make_uint8_p( zInfo.size );
                    auto byteRead = zip_fread( zFile, buff.first.get(), buff.second );
                    LOGRS(  zip_get_name( za, t, ZIP_FL_UNCHANGED) << " Size: " << buff.second << " Uncompressed: " << zInfo.size << " Read: " << byteRead );
                    auto tempFileName = getDaemonRoot() + "/" + zInfo.name;
                    FM::writeLocalFile( tempFileName, SerializableContainer{buff.first.get(), buff.first.get()+buff.second} );
                }

//                auto container = FM::readLocalFileC(fileDownloaded);
//                LOGRS(std::string(meta.filename));
            }
            if (getFileNameExt(std::string(meta.filename)) == ".fbx") {
                elaborateGeomFBX(entityBucket, fileDownloaded, meta.project, meta.username, meta.useremail);
            }
        }
    }
}

void parseAssetStream(Mongo &mdb, mongocxx::change_stream &stream) {
    for (auto change : stream) {
        StreamChangeMetadata meta{change};
        mdb.insertEntityFromAsset(meta);
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {

    LOGRS("Daemon version 3.0.2");

    if (!Http::useClientCertificate(true,
                                    "EH_DEAMON_CERT_KEY_PATH", "EH_DEAMON_CERT_CRT_PATH")) {
        LOGRS("Daemon certificate and key environment variables needs to be present as"
              "\n$EH_DEAMON_CERT_KEY_PATH\n$EH_DEAMON_CERT_CRT_PATH");
        return 1;
    }

    Socket::createConnection();

//    initDeamon();

    Mongo mdb{"event_horizon"};
    auto sourceAssetBucket = mdb.useBucket("fs_assets_to_elaborate");
    auto entityBucket = mdb.useBucket("fs_entity_assets");

    sourceAssetBucket.deleteAll();
    entityBucket.deleteAll();
    mdb["entities"]().delete_many({});

    auto streamToElaborate = mdb["fs_assets_to_elaborate.files"].watch();
    auto streamAsset = mdb["fs_entity_assets.files"].watch();

    while (true) {
        parseElaborateStream(streamToElaborate, sourceAssetBucket, entityBucket);
        parseAssetStream(mdb, streamAsset);
    }

    return 0;
}

#pragma clang diagnostic pop