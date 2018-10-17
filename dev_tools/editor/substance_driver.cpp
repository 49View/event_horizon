//
// Created by Dado on 19/08/2018.
//

#include "substance_driver.h"
#include <core/file_manager.h>
#include <core/descriptors/material.h>
#include <math.h>
#include <core/util.h>
#include <core/tar_util.h>
#include <core/http/basen.hpp>

#include <core/zlib_util.h>
#include <core/image_util.h>
#include <poly/material_builder.h>

namespace SubstanceDriver {

static std::string pastedString = "";
static std::string refreshMat = "";


void elaborateSbsarLayer( const std::string& mainFileName, const std::string& layerName, const uint32_t size,
                          DependencyMaker& _md ) {

    FM::copyLocalToRemote( mainFileName, MDaemonPaths::UploadDir + getFileName(mainFileName) );
    return;

    std::string fn = getFileNameOnly( mainFileName );
    std::string metadataName = fn + layerName;
    std::string metadataHttp = metadataName + ".mat";
    std::string filePath =  getFileNamePath(mainFileName) + "/";
    std::string filePathNoExt = getFileNameNoExt( fn );
    std::string fext = ".png";
    std::string cdInto = "cd " + filePath + "\n";

    std::string sizeString = std::to_string(log2(size));
    std::string sbRender = "sbsrender render --inputs "
                           + mainFileName +
                           " --set-value '$outputsize@" + sizeString + "," + sizeString + "' "
                           "--output-name {inputName}_{outputNodeName} "
                           "--input-graph-output basecolor "
                           "--input-graph-output metallic --input-graph-output ambient_occlusion "
                           "--input-graph-output roughness --input-graph-output height --input-graph-output normal "
                           "--output-path ./";

    std::string cmdS = cdInto + sbRender;

    std::system(cmdS.c_str());

    std::string tarname = filePathNoExt + layerName + ".tar";
    std::string fileb = filePathNoExt + "_" + MPBRTextures::basecolorString + fext;
    std::string fileh = filePathNoExt + "_" + MPBRTextures::heightString + fext;
    std::string filem = filePathNoExt + "_" + MPBRTextures::metallicString + fext;
    std::string filer = filePathNoExt + "_" + MPBRTextures::roughnessString + fext;
    std::string filen = filePathNoExt + "_" + MPBRTextures::normalString + fext;
    std::string filea = filePathNoExt + "_" + MPBRTextures::ambientOcclusionString + fext;

    MaterialBuilder mb{metadataName};
    mb.buffer( MPBRTextures::basecolorString,FM::readLocalFile((filePath + fileb))).
    buffer( MPBRTextures::heightString,FM::readLocalFile((filePath + fileh))).
    buffer( MPBRTextures::metallicString,FM::readLocalFile((filePath + filem))).
    buffer( MPBRTextures::roughnessString,FM::readLocalFile((filePath + filer))).
    buffer( MPBRTextures::normalString,FM::readLocalFile((filePath + filen))).
    buffer( MPBRTextures::ambientOcclusionString,FM::readLocalFile((filePath + filea))).
    makeDirect(_md);

//    LOGR( mb.toMetaData().c_str() );

    Http::post( Url{ Http::restEntityPrefix( Material::entityGroup(), metadataHttp ) }, mb.toMetaData() );
//
//    std::string rmcl = cdInto + "\n rm -f " fileb + " " + fileh + " " + filem + " " + filer
//                       + " " + filen + " " + filea;
//
//    std::system( rmcl.c_str());
}

void elaborateFromTextureSet( const std::string& mainFileName, DependencyMaker& _md, const std::string& _forcedExt ) {
    static const std::string basecolorString = "_basecolor";
    static const std::string heightString = "_height";
    static const std::string metallicString = "_metallic";
    static const std::string roughnessString = "_roughness";
    static const std::string normalString = "_normal";
    static const std::string ambientOcclusionString = "_ambient_occlusion";

    std::string fn = getFileNameOnly( mainFileName );
    std::string filePath =  getFileNamePath(mainFileName) + "/";
    std::string filePathNoExt = getFileNameNoExt( fn );
    std::string fext = _forcedExt;

    std::string cdInto = "cd " + filePath + "\n";

    std::string tarname = filePathNoExt + ".tar";
    std::string matname = filePathNoExt + ".mat";
    std::string tarStart = "\n export COPYFILE_DISABLE=true\n tar -cvf " + tarname;

    std::string fileb = filePathNoExt + basecolorString + fext;
    std::string fileh = filePathNoExt + heightString + fext;
    std::string filem = filePathNoExt + metallicString + fext;
    std::string filer = filePathNoExt + roughnessString + fext;
    std::string filen = filePathNoExt + normalString + fext;
    std::string filea = filePathNoExt + ambientOcclusionString + fext;

    std::string filebcl = " " + fileb;
    std::string filehcl = " " + fileh;
    std::string filemcl = " " + filem;
    std::string filercl = " " + filer;
    std::string filencl = " " + filen;
    std::string fileacl = " " + filea;

    std::string cmd = cdInto + tarStart + filebcl+ filehcl + filemcl + filercl + filencl+ fileacl;
    std::system( cmd.c_str());

    auto f = zlibUtil::deflateMemory( FM::readLocalFile(filePath + "/" + tarname) );
    Http::post( Url{ Http::restEntityPrefix( Material::entityGroup(), matname ) }, f );

    std::string rmcl = cdInto + " rm -f " + tarname + " " + matname + " "
                     + fileb + " " + fileh + " " + filem + " " + filer + " " + filen + " " + filea;

    std::system( rmcl.c_str());

}

void elaborateSbsar( const std::string& mainFileName, DependencyMaker& _md ) {
    if ( getFileNameExt( mainFileName ) != ".sbsar" ) return;

    // Check that the file is not a copy (mac version)
    if ( checkFileNameNotACopy(mainFileName) ) return;

//    elaborateSbsarLayer( mainFileName, MQSettings::Low,     128 ,_md );
    elaborateSbsarLayer( mainFileName, MQSettings::Medium,  512 ,_md );
//    elaborateSbsarLayer( mainFileName, MQSettings::Hi,      1024,_md );

    refreshMat = getFileNameOnly(mainFileName);
    refreshMat = string_trim_upto(refreshMat, ".mat");
    replaceAllStrings(refreshMat, "_", " " );

//    elaborateSbsarLayer( mainFileName, sbsar::UltraHi, 2048 );
}

}