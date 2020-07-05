//
// Created by Dado on 2019-01-06.
//

#pragma once

#include <variant>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <set>
#include <cstdint>

class Vector2f;
class Vector3f;
class Vector4f;
class Quaternion;

using AnimVariant = std::variant<int, float, Vector2f, Vector3f, Vector4f, Quaternion>;

using TimelineIndex = uint64_t;
using TimelineSet   = std::set<TimelineIndex>;

struct AnimTypeUID {
protected:
    static uint64_t suid; // Static Increment ID
};

template <typename T>
class AnimType : private AnimTypeUID {
public:
    explicit AnimType( const T& v, std::string _name ) : value(v), name( std::move( _name )) {
        uid = suid++;
    }

    uint64_t UID() const {
        return uid;
    }

    void set(const T& _value) {
        value = _value;
    }

    T operator()() const {
        return value;
    }

    const std::string& Name() const { return name; }

    uint64_t uid = 0;
    T value;
    bool isAnimating = false;
private:
    std::string name;
};

class Animable {
public:
    virtual TimelineSet addKeyFrame( const std::string& name, float _time ) = 0;
};

template <typename T>
using AnimValue = std::shared_ptr<AnimType<T>>;

using inta 		    = AnimValue<int>;
using floata 		= AnimValue<float>;
using V2fa 			= AnimValue<Vector2f>;
using V3fa 			= AnimValue<Vector3f>;
using V4fa 			= AnimValue<Vector4f>;
using Quaterniona 	= AnimValue<Quaternion>;

enum class AnimLoopType {
    Linear,
    Reverse,
    Bounce,
    Loop,
    Toggle
};

enum class AnimVelocityType {
    Linear,
    Cosine,
    Exp,
    Hermite
};

using KeyFrameTimes_t = std::vector<float>;
using AnimVisitCallback = std::function<void(const std::string&, const std::vector<float>&, TimelineIndex, TimelineIndex, int)>;
using TimelineLinks = std::unordered_map< TimelineIndex, TimelineSet >;
using TimelineGroupCCF = std::function<void()>;
using TimelineUpdateGroupCCF = std::function<void(float)>;

const static TimelineIndex   tiNorm  = 1000000000;

constexpr static TimelineIndex   tiFloat = 0;
constexpr static TimelineIndex   tiV2f   = tiFloat + tiNorm;
constexpr static TimelineIndex   tiV3f   = tiV2f   + tiNorm;
constexpr static TimelineIndex   tiV4f   = tiV3f   + tiNorm;
constexpr static TimelineIndex   tiQuat  = tiV4f   + tiNorm;
constexpr static TimelineIndex   tiInt   = tiQuat  + tiNorm;

constexpr static TimelineIndex   tiFloatIndex   = tiFloat / tiNorm;
constexpr static TimelineIndex   tiV2fIndex     = tiV2f   / tiNorm;
constexpr static TimelineIndex   tiV3fIndex     = tiV3f   / tiNorm;
constexpr static TimelineIndex   tiV4fIndex     = tiV4f   / tiNorm;
constexpr static TimelineIndex   tiQuatIndex    = tiQuat  / tiNorm;
constexpr static TimelineIndex   tiIntIndex     = tiInt   / tiNorm;
