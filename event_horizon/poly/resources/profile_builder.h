#pragma once

#include <memory>
#include <string>
#include <core/util.h>
#include <core/math/vector2f.h>
#include <core/image_constants.h>
#include <poly/resources/resource_builder.hpp>
#include <core/math/poly_utils.hpp>


class Profile;

using profileDirectMakeFunc = std::function<std::shared_ptr<Profile>(const std::string&, const std::vector<Vector2f>&, const std::vector<float>&)>;

class ProfileBuilder : public ResourceBuilder2<ProfileBuilder, Profile> {
public:
    using ResourceBuilder2::ResourceBuilder2;
    ~ProfileBuilder() override = default;
    // This will create a circular profile
//    explicit ProfileBuilder( ProfileManager& _pm, float _radius, float _subDivs = 3 );
//    explicit ProfileBuilder( ProfileManager& _pm, const Vector2f& _length );
//             ProfileBuilder( ProfileManager& _pm, const Vector2f& _v1, const Vector2f& _v2 );

    ProfileBuilder& func( profileDirectMakeFunc _cf ) {
        cfunc = _cf;
        return *this;
    }

    ProfileBuilder& cv2( const Vector2f& _v ) {
        vv2fs.emplace_back( _v );
        Name( Name() + _v.toString() );
        return *this;
    }

    ProfileBuilder& cf( const float _v ) {
        Name( Name() + std::to_string(_v) );
        vfs.emplace_back( _v );
        return *this;
    }

protected:
    void finalise( std::shared_ptr<Profile> _elem ) override;

private:
    profileDirectMakeFunc cfunc = nullptr;
    std::vector<Vector2f> vv2fs;
    std::vector<float> vfs;
};
