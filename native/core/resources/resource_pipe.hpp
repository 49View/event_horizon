//
// Created by Dado on 2019-04-02.
//

#pragma once

#include <core/resources/resource_utils.hpp>
#include <core/resources/publisher.hpp>
#include <core/tar_util.h>

class ResourcePipe {
public:
    ResourcePipe() {
        tar = std::make_shared<tarUtil::TarWrite>(tagStream);
    }

    template <typename R>
    auto pipeFile( const std::string& _filename ) {
        auto fileContent = FM::readLocalFileC(_filename);
        if ( !fileContent.empty()) {
            return pipe<R>( toLower(_filename), fileContent);
        }
        return ResourceTarDict{};
    }

    template <typename R>
    auto pipe( const std::string& _filename, const SerializableContainer& _fileContent ) {
        auto _elem = Publisher<R>{}.pipe( getFileName(_filename),
                                          _fileContent,
                                          ResourceVersioning<R>::HasDeps() ? refs : ResourceDependencyDict{} );
        tar->put( _elem.name.c_str(), _elem.metadata );
        auto prefix = ResourceVersioning<R>::Prefix();
        ResourceTarDict ret = { prefix, _elem.name, _elem.hash };
        catalog.emplace_back( ret );
        refs[ResourceVersioning<R>::Prefix()].emplace_back( _elem.hash );
        return ret;
    }

    void publish() {
        tar->finish();
        Http::post( Url{ HttpFilePrefix::entities + "multi" }, zlibUtil::deflateMemory(tagStream.str()), nullptr );
    }

    const std::vector<ResourceTarDict>& getCatalog() const {
        return catalog;
    }

private:
    ResourceDependencyDict refs;
    std::vector<ResourceTarDict> catalog{};
    std::stringstream tagStream;
    std::shared_ptr<tarUtil::TarWrite> tar;
};

