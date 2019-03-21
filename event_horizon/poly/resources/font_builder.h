#pragma once

#include <poly/resources/resource_builder.hpp>

namespace Utility::TTFCore { class Font; }

class FontBuilder : public ResourceBuilder2<FontBuilder, Utility::TTFCore::Font> {
public:
    using ResourceBuilder2::ResourceBuilder2;
protected:
    void serializeInternal( std::shared_ptr<SerializeBin> writer ) const override;
    void deserializeInternal( std::shared_ptr<DeserializeBin> reader ) override;
    void finalise( std::shared_ptr<Utility::TTFCore::Font> _elem ) override;
};
