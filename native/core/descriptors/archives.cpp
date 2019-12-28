//
// Created by Dado on 27/12/2019.
//

#include "archives.h"
#include <zip.h>
#include <sys/stat.h>
#include <core/util.h>
#include <core/file_manager.h>

static int getZipData(void **datap, size_t *sizep, const char *archive) {
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

ArchiveDirectory unzipFilesToTempFolder( const std::string& filename ) {
    ArchiveDirectory ad{};
    void *data;
    size_t size;
    zip_source_t *src;
    zip_t *za;
    zip_error_t error;
    /* get buffer with zip archive inside */
    LOGRS("[UNZIP]" << " Start unzipping" << filename);
    if (getZipData(&data, &size, filename.c_str()) < 0) {
        LOGRS("[UNZIP-ERROR]" << " " << filename << " get_data failed");
    }
    zip_error_init(&error);
    /* create source from buffer */
    if ((src = zip_source_buffer_create(data, size, 1, &error)) == NULL) {
        free(data);
        zip_error_fini(&error);
        LOGRS("[UNZIP-ERROR]" << " " << filename << " zip_source_buffer_create failed");
    }

    /* open zip archive from source */
    if ((za = zip_open_from_source(src, 0, &error)) == NULL) {
        zip_source_free(src);
        zip_error_fini(&error);
        LOGRS("[UNZIP-ERROR]" << " " << filename << " zip_open_from_source failed");
    }

    auto numFiles = zip_get_num_entries( za, ZIP_FL_UNCHANGED );
    LOGRS( "NumFiles: " <<  numFiles );
    for ( auto t = 0; t < numFiles; t++ ) {
        auto zFile = zip_fopen_index( za, t, ZIP_FL_UNCHANGED );
        zip_stat_t zInfo;
        zip_stat_index( za, t, ZIP_FL_UNCHANGED, &zInfo );
        auto buff = make_uint8_p( zInfo.size );
        auto byteRead = zip_fread( zFile, buff.first.get(), buff.second );
        ad.insert( { zInfo.name, zInfo.size} );
        LOGRS(  zip_get_name( za, t, ZIP_FL_UNCHANGED) << " Size: " << buff.second << " Uncompressed: " << zInfo.size << " Read: " << byteRead );
        auto tempFileName = getDaemonRoot() + "/" + zInfo.name;
        FM::writeLocalFile( tempFileName, SerializableContainer{buff.first.get(), buff.first.get()+buff.second} );
    }

    return ad;
}

std::vector<ArchiveDirectoryEntityElement> ArchiveDirectory::findFilesWithExtension(const std::string & _ext ) const {
    std::vector<ArchiveDirectoryEntityElement> ret{};

    for ( const auto& elem : admap ) {
        if ( getFileNameExt( elem.first ) == _ext ) {
            ret.emplace_back(elem.second);
        }
    }

    return ret;
}

void ArchiveDirectory::insert(ArchiveDirectoryEntityElement && _elem) {
    admap[_elem.name] = _elem;
}
