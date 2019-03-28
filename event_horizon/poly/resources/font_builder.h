#pragma once

#include <poly/resources/resource_builder.hpp>

namespace Utility::TTFCore { class Font; }

class FontBuilder : public ResourceBuilder3<FontBuilder, Utility::TTFCore::Font> {
public:
    using ResourceBuilder3::ResourceBuilder3;
};
