//
// Created by dado on 12/10/2020.
//

#pragma once

#include <string>
#include <core/htypes_shared.hpp>

namespace OSMElementName {
    [[maybe_unused]] static inline std::string unclassified() {
        return "unclassified";
    }
    [[maybe_unused]] static inline std::string park() {
        return "park";
    }
    [[maybe_unused]] static inline std::string parking() {
        return "parking";
    }
    [[maybe_unused]] static inline std::string water() {
        return "water";
    }

    [[maybe_unused]] static inline std::string building() {
        return "building";
    }
    [[maybe_unused]] static inline std::string entity() {
        return "entity";
    }
    [[maybe_unused]] static inline std::string tree() {
        return "tree";
    }
    [[maybe_unused]] static inline std::string amenity() {
        return "amenity";
    }
    [[maybe_unused]] static inline std::string telephone() {
        return "telephone";
    }
    [[maybe_unused]] static inline std::string natural() {
        return "natural";
    }
    [[maybe_unused]] static inline std::string historic() {
        return "historic";
    }
    [[maybe_unused]] static inline std::string monument() {
        return "monument";
    }
    [[maybe_unused]] static inline std::string bus_stop() {
        return "bus_stop";
    }

    // Roads
    [[maybe_unused]] static inline std::string road() {
        return "road";
    }
    [[maybe_unused]] static inline std::string highway() {
        return "highway";
    }
    [[maybe_unused]] static inline std::string motorway() {
        return "motorway";
    }
    [[maybe_unused]] static inline std::string primary() {
        return "primary";
    }
    [[maybe_unused]] static inline std::string secondary() {
        return "secondary";
    }
    [[maybe_unused]] static inline std::string crossing() {
        return "crossing";
    }
    [[maybe_unused]] static inline std::string cycleway() {
        return "cycleway";
    }
    [[maybe_unused]] static inline std::string footway() {
        return "footway";
    }
    [[maybe_unused]] static inline std::string give_way() {
        return "give_way";
    }
    [[maybe_unused]] static inline std::string mini_roundabout() {
        return "mini_roundabout";
    }
    [[maybe_unused]] static inline std::string path() {
        return "path";
    }
    [[maybe_unused]] static inline std::string pedestrian() {
        return "pedestrian";
    }
    [[maybe_unused]] static inline std::string residential() {
        return "residential";
    }
    [[maybe_unused]] static inline std::string service() {
        return "service";
    }
    [[maybe_unused]] static inline std::string steps() {
        return "steps";
    }
    [[maybe_unused]] static inline std::string tertiary() {
        return "tertiary";
    }
    [[maybe_unused]] static inline std::string traffic_signals() {
        return "traffic_signals";
    }


    // Barrier
    [[maybe_unused]] static inline std::string barrier() {
        return "barrier";
    }
    [[maybe_unused]] static inline std::string bollard() {
        return "bollard";
    }
    [[maybe_unused]] static inline std::string fence() {
        return "fence";
    }
    [[maybe_unused]] static inline std::string gate() {
        return "gate";
    }
    [[maybe_unused]] static inline std::string kerb() {
        return "kerb";
    }
    [[maybe_unused]] static inline std::string retaining_wall() {
        return "retaining_wall";
    }
    [[maybe_unused]] static inline std::string wall() {
        return "wall";
    }

    // Trees
    [[maybe_unused]] static inline std::string fir_tree() {
        return "fir,tree";
    }
    [[maybe_unused]] static inline std::string oak_tree() {
        return "oak,tree";
    }
    [[maybe_unused]] static inline std::string poplar_tree() {
        return "poplar,tree";
    }
    [[maybe_unused]] static inline std::string palm_tree() {
        return "palm,tree";
    }
}

[[maybe_unused]] static inline ddContainer OSMTreeList() {
    static ddContainer ret{};

    if ( ret.empty() ) {
        ret.emplace_back(OSMElementName::fir_tree());
        ret.emplace_back(OSMElementName::oak_tree());
        ret.emplace_back(OSMElementName::palm_tree());
        ret.emplace_back(OSMElementName::poplar_tree());
    }

    return ret;
}
