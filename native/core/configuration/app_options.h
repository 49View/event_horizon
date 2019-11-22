#pragma once

#include "../serialization.hpp"

JSONDATA( HouseOptions, startupFloorplanName, startupFloorplanConfigurationName, forceRemakeFloorplans, furnishHouse,
          defaultHouseFurnitureTags, doorOpeningAngle )

    std::string startupFloorplanName = "springfield_court.png";
    std::string startupFloorplanConfigurationName = "_default";

    bool forceRemakeFloorplans = false;

    bool furnishHouse = false; //if true and there is no cache (or forceremake) furnishes the house with following defaultHouseFurnitureTags
    std::vector<std::string> defaultHouseFurnitureTags = { "alpha" };

    float doorOpeningAngle = 1.0f;
};

JSONDATA( SunOptions, location, dateHint, timeHint, autoTransition, transitionSpeed )

    std::string location = "Kingston";
    std::string dateHint = "spring";
    std::string timeHint = "noon";
    bool autoTransition = false;
    float transitionSpeed = 1440.0f; //for example 1440 means one entire day transition in one minute (24h x 60m x 60s / 60s)
};

JSONDATA( CatalogingServerOptions, protocol, host, port )

    std::string protocol = "http";
    std::string host = "192.168.99.100";
    int32_t port = 80;
};

JSONDATA( FolderOptions, dataFolder, floorplanFolder, cacheFolder, tempFolder )

    std::string dataFolder = "";
    std::string floorplanFolder = "";
    std::string cacheFolder = "";
    std::string tempFolder = "";
};

JSONDATA( LightingOptions, sphericalHarmonicsSamplesSquaredRooted, sphericalHarmonicsMaxBouncing,
          sphericalHarmonicsBouncingThreshold, relightDuringCalculation )

    int32_t sphericalHarmonicsSamplesSquaredRooted = 15;
    int32_t sphericalHarmonicsMaxBouncing = 2;
    float sphericalHarmonicsBouncingThreshold = 0.0001f;
    bool relightDuringCalculation = true;
};

JSONDATA_R( AppOptions, name, houseOptions, sunOptions, catalogingServerOptions, lightingOptions )
    std::string name = "";
    HouseOptions houseOptions;
    SunOptions sunOptions;
    CatalogingServerOptions catalogingServerOptions;
    LightingOptions lightingOptions;
};

