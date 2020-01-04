#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <cmath>

#define TINYGLTF_IMPLEMENTATION
#include <tinygltf/include/tiny_gltf.h>
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include <core/http/webclient.h>
#include <core/util.h>
#include <core/file_manager.h>
#include <core/runloop_core.h>
#include <core/tar_util.h>
#include <core/zlib_util.h>
#include <core/names.hpp>
#include <core/descriptors/archives.h>
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
    Socket::emit("daemonLogger", serializeLogger(LoggerLevel::Error, e.what()));
}

void daemonWarningLog(const std::string &e) {
    LOGRS(e);
    Socket::emit("daemonLogger", serializeLogger(LoggerLevel::Warning, e));
}

std::optional<MongoFileUpload> elaborateImage(MongoBucket bucket, const char *filename,
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

std::string chooseMainArchiveFilename(const ArchiveDirectory &ad, strview group) {
    if (group == ResourceGroup::Geom) {
        auto candidates = ad.findFilesWithExtension({".fbx"});
        if (candidates.size() > 0) {
            for (const auto &elem : candidates) {
                if (elem.name.find("_upY.fbx") != std::string::npos) {
                    return elem.name;
                }
            }
            return candidates.back().name;
        }
    }
    if (group == ResourceGroup::Material) {
        auto candidates = ad.findFilesWithExtension({".png", ".jpg"});
        if ( candidates.size() > 0 ) {
            return ad.Name();
        }
    }
    return {};
}

ArchiveDirectory generateActiveDirectoryFromSBSARTempFiles(const std::string &fn) {
    ArchiveDirectory ret{fn};
    auto fileRoot = getDaemonRoot();

    for (const auto &output : MPBRTextures::SBSARTextureOutputs()) {
        std::string outFilename{};
        outFilename.append(fn).append("_").append(output).append(".png");
        if (FM::fileExist(fileRoot + outFilename)) {
            ret.insert({outFilename, 0, output});
        }
    }
    return ret;
}

ArchiveDirectory& mapActiveDirectoryFilesToPBR( ArchiveDirectory& ad ) {
    for ( auto& elem : ad ) {
        elem.second.metaString = MPBRTextures::findTextureInString( elem.second.name );
    }
    return ad;
}

ResourceEntityHelper elaborateInternalMaterial(MongoBucket entity_bucket,
                                               const ArchiveDirectory &ad,
                                               int nominalSize,
                                               strview project,
                                               strview uname,
                                               strview uemail) {
    ResourceEntityHelper mat{};
    auto fileRoot = getDaemonRoot();

    Material pbrmaterial{S::SH, ad.Name()};
    for (const auto &output : ad) {
        std::string fullFileName = fileRoot + output.second.name;
        auto to = elaborateImage(entity_bucket, fullFileName.c_str(), nominalSize, nominalSize, project, uname,
                                 uemail, mat.thumbs);
        if (to && !output.second.metaString.empty()) {
            auto tid = (*to).getStringId();
            mat.deps[ResourceGroup::Image].emplace_back(tid);
            pbrmaterial.Values()->assign(MPBRTextures::mapToTextureUniform(output.second.metaString), tid);
        }
    }

    // Create PBR material
    mat.sc = pbrmaterial.serialize();
    return mat;
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
    ResourceEntityHelper mat = elaborateInternalMaterial(entity_bucket,
                                                         generateActiveDirectoryFromSBSARTempFiles(fn), nominalSize,
                                                         project, uname, uemail);
    Mongo::fileUpload(entity_bucket, fn, mat.sc,
                      Mongo::FSMetadata(ResourceGroup::Material, project, uname, uemail,
                                        HttpContentType::json, Hashable<>::hashOf(mat.sc), mat.thumbs[0], mat.deps));

    // Clean up
    std::string cleanup = "cd " + fileRoot + " && rm " + fn + "*";
    std::system(cleanup.c_str());
}

void elaborateMatFromArchive(MongoBucket entity_bucket,
                       ArchiveDirectory &ad,
                       strview project,
                       strview uname,
                       strview uemail) {

    const int nominalSize = 512;
    // Gather texture outputs
    ResourceEntityHelper mat = elaborateInternalMaterial(entity_bucket,
                                                         mapActiveDirectoryFilesToPBR(ad), nominalSize,
                                                         project, uname, uemail);
    Mongo::fileUpload(entity_bucket, ad.Name(), mat.sc,
                      Mongo::FSMetadata(ResourceGroup::Material, project, uname, uemail,
                                        HttpContentType::json, Hashable<>::hashOf(mat.sc), mat.thumbs[0], mat.deps));
}

int resaveGLB( const std::string& filename ) {
//    using namespace tinygltf;
//
//    Model model;
//    TinyGLTF loader;
//    std::string err;
//    std::string warn;
//
//    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename.c_str());
//
//    if (!warn.empty()) {
//        printf("Warn: %s\n", warn.c_str());
//    }
//
//    if (!err.empty()) {
//        printf("Err: %s\n", err.c_str());
//    }
//
//    if (!ret) {
//        printf("Failed to parse glTF\n");
//        return -1;
//    }
    return 0;
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

        if (filenameEscaped != filenameSanitized) {
            auto ret = std::system(
                    std::string{"cd " + dRoot + " && mv " + filenameEscaped + " " + filenameSanitized}.c_str());
            LOGRS("FBX sanity renamed return code: " << ret);
        }
        auto fn = getFileNameOnly(filenameSanitized);
        std::string filenameglb = fn + ".glb";

        std::string cmd =
                "cd " + dRoot + " && FBX2glTF -b --pbr-metallic-roughness -o " + filenameglb +
                " " +
                getFileName(filenameSanitized);
        auto ret = std::system(cmd.c_str());
        if (ret != 0) throw DaemonException{std::string{"FBX elaboration return code: " + std::to_string(ret)}};

        auto fileData = FM::readLocalFile(dRoot + filenameglb);
        resaveGLB(dRoot + filenameglb);
        auto fileHash = Hashable<>::hashOf(fileData);
        Mongo::fileUpload(entity_bucket, filenameglb, std::move(fileData),
                          Mongo::FSMetadata(ResourceGroup::Geom, project, uname, uemail,
                                            HttpContentType::json, fileHash, "", ResourceDependencyDict{}));
    } catch (const std::exception &e) {
        daemonExceptionLog(e);
    }
}

void parseElaborateStream(mongocxx::change_stream &stream, MongoBucket sourceAssetBucket, MongoBucket entityBucket) {

    try {
        for (auto change : stream) {
            StreamChangeMetadata meta{change};
            Profiler p1{"elaborate time:"};
            auto filename = std::string(meta.filename);
            auto fileDownloaded = Mongo::fileDownload(sourceAssetBucket,
                                                      meta.id,
                                                      getDaemonRoot() + std::string{filename});

            bool bIsInAnArchive = getFileNameExt(std::string(filename)) == ".zip";
            ArchiveDirectory ad{filename};
            // First unzip all the content if package arrives in a zip file
            if (bIsInAnArchive) {
                if (getFileNameExt(filename) == ".zip") {
                    unzipFilesToTempFolder(fileDownloaded, ad);
                }
                if (filename = chooseMainArchiveFilename(ad, meta.group); filename.empty()) {
                    daemonWarningLog(std::string(meta.filename) + " does not contain any appropriate asset file");
                    continue;
                }
            }

            if (meta.group == ResourceGroup::Material) {
                if (getFileNameExt(std::string(filename)) == ".zip") {
                    elaborateMatFromArchive(entityBucket, ad, meta.project,
                                      meta.username,
                                      meta.useremail);
                }
                if (getFileNameExt(std::string(filename)) == ".sbsar") {
                    elaborateMatSBSAR(entityBucket, fileDownloaded, {}, 512, meta.project,
                                      meta.username,
                                      meta.useremail);
                }
            } else if (meta.group == ResourceGroup::Geom) {
                if (getFileNameExt(filename) == ".fbx") {
                    elaborateGeomFBX(entityBucket, filename, meta.project, meta.username, meta.useremail);
                }
            }
        }
    } catch (const std::exception &e) {
        daemonExceptionLog(e);
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
    mdb["remaps"]().delete_many({});

    auto streamToElaborate = mdb["fs_assets_to_elaborate.files"].watch();
    auto streamAsset = mdb["fs_entity_assets.files"].watch();

    while (true) {
        parseElaborateStream(streamToElaborate, sourceAssetBucket, entityBucket);
        parseAssetStream(mdb, streamAsset);
    }

    return 0;
}

#pragma clang diagnostic pop