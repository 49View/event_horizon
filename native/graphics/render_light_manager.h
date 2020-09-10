#pragma once

#include <core/math/vector3f.h>
#include <core/math/anim_type.hpp>
#include <core/math/anim_type.hpp>
#include <core/resources/light.hpp>

class ProgramUniformSet;
class ShaderManager;
class ShadowMapManager;

class LightBase {
public:
	LightBase( const std::string& _key, float wattage, float intensity, const Vector3f& attenuation ) {
	    mKey = _key;
	    name = "Default";
		mType = LightType_Invalid;
		mWattage = wattage;
		mIntensity = std::make_shared<AnimType<float>>( intensity, "lightIntensity" );
		mAttenuation = std::make_shared<AnimType<Vector3f>>( attenuation, "lightAttenuation" );
	}

    [[nodiscard]] const std::string& Key() const { return mKey; }
    [[nodiscard]] bool GoingUp() const { return bGoingUp; }
    [[nodiscard]] bool& GoingUp() { return bGoingUp; }
    void goingUpToggle() { bGoingUp = !bGoingUp; }
    void toggleLightIntensity( float _intensity = 1.0f );
    void updateLightIntensityAfterToggle( float _intensity = 1.0f );

    [[nodiscard]] float Intensity() const { return mIntensity->value * mWattage; }
	std::shared_ptr<AnimType<float>>& IntensityAnim() { return mIntensity; }
	void Intensity( float val ) { mIntensity->value = val; }
	[[nodiscard]] Vector3f Attenuation() const { return mAttenuation->value; }
	void Attenuation( const Vector3f& val ) { mAttenuation->value = val; }
	[[nodiscard]] LightType Type() const { return mType; }
	void Type( LightType val ) { mType = val; }
    [[nodiscard]] float Wattage() const { return mWattage; }
    void Wattage( float _wattage ) { LightBase::mWattage = _wattage; }

protected:
    std::string mKey;
	std::string name;
	bool bGoingUp = false; // Direction of light when animating, going up == true == turning on.
	LightType mType;
	float mWattage;
	std::shared_ptr<AnimType<float>> mIntensity;
	std::shared_ptr<AnimType<Vector3f>>	mAttenuation;
};

class DirectionalLight : public LightBase {
public:
	explicit DirectionalLight( const Vector3f& dir, float wattage = 50.0f, float intensity = 1.0f, const Vector3f& attenuation = V3fc::ONE ) : LightBase( "dir", wattage, intensity, attenuation ) {
		Type( LightType_Point );
		mDir = std::make_shared<AnimType<Vector3f>>( dir, "lightDirection" );
	}

	[[nodiscard]] Vector3f Dir() const { return mDir->value; }
	void Dir( const Vector3f& val ) { mDir->value = val; }
private:
	std::shared_ptr<AnimType<Vector3f>>	mDir;
};

class PointLight : public LightBase {
public:
	explicit PointLight( const std::string& _key, const Vector3f& pos, float wattage = 50.0f, float intensity = 1.0f, const Vector3f& attenuation = V3fc::ONE ) : LightBase( _key, wattage, intensity, attenuation ) {
		Type( LightType_Point );
		mPos = std::make_shared<AnimType<Vector3f>>( pos, "lightPos" );
	}

	[[nodiscard]] Vector3f Pos() const { return mPos->value; }
	void Pos( const Vector3f& val ) { mPos->value = val; }
protected:
	std::shared_ptr<AnimType<Vector3f>>	mPos;
};

class SpotLight : public PointLight {
public:
	SpotLight( const Vector3f& pos, const Vector3f& dir, float beamAngle = 60.0f, float wattage = 50.0f, float intensity = 1.0f, const Vector3f& attenuation = V3fc::ONE ) : PointLight( "spot", pos, wattage, intensity, attenuation ) {
		Type( LightType_Spotlight );
		mDir = std::make_shared<AnimType<Vector3f>>( dir, "lightDirection" );
		mBeamAngle = std::make_shared<AnimType<float>>( beamAngle, "lightBeamAngle" );
	}

	[[nodiscard]] Vector3f Dir() const { return mDir->value; }
	void Dir( const Vector3f& val ) { mDir->value = val; }

	std::shared_ptr<AnimType<float>> BeamAngle() { return mBeamAngle; };
	[[nodiscard]] float BeamAngle() const { return mBeamAngle->value; }
	void BeamAngle( const float val ) { mBeamAngle->value = val; }
private:
	std::shared_ptr<AnimType<Vector3f>>	mDir;
	std::shared_ptr<AnimType<float>> mBeamAngle;
};

class RenderLightManager {
public:
    RenderLightManager();

	void addPointLight( const std::string& _key, const Vector3f& pos, float _wattage, float intensity = 1.0f, const Vector3f& attenuation = V3fc::ONE );
    void setPointLightPos( size_t index, const Vector3f& _pos );
    void setPointLightWattage( size_t index, float _watt );
    void setPointLightIntensity( size_t index, float _intensity );
    void togglePointLightIntensity( const std::string& _key, float _intensity );

    void setPointLightWattages( float _watt );
    void setPointLightIntensities( float _intensity );

	void removePointLight( size_t index );
	void removeAllPointLights();
	void toggleLightsOnOff();
	void switchLightsOn( float animTime, TimelineGroupCCF _ccf = nullptr );
	void switchLightsOff( float animTime, TimelineGroupCCF _ccf = nullptr );


	void setUniforms( const Vector3f& _cameraPos, std::shared_ptr<ShadowMapManager> smm,
	                  const V4f& _sunRadiance );
	void setUniforms_r();
	void generateUBO( std::shared_ptr<ShaderManager> sm );
	void update( float timeStamp );
	void setShadowOverBurnCofficient( float _overBurn );
    void setShadowZFightCofficient( float _value );
    void setIndoorSceneCoeff( float _value );
    void setSSAOKernelRadius( float _value );
    void setSSAOFalloffRadius( float _value );
    void setSSAONumRealTimeSamples( float _value );

    PointLight* findPointLight( const std::string& _key );
private:
	bool mbGlobalOnOffSwitch;
	std::vector<DirectionalLight> mDirectionalLights;
	std::vector<PointLight> mPointLights;
	std::vector<SpotLight> mSpotLights;
	std::vector<V3f> mHemisphereKernelSamples;
	V4f mSSAOParameters = V4f{ 10.0f, 32.0f, 0.9f, 1.0f};
	std::shared_ptr<AnimType<float>> mDirectionalLightIntensity;

	std::unique_ptr<ProgramUniformSet> mLigthingUniform;

	int mMaxLights = 16;
	int mNumHemiKernelSize = 64;
    V4f hdrExposures = V4f{2.0f, 1.0f, 1.0f, 1.0f};
    V4f shadowParameters = V4f{0.001f, 1.0f, 1.0f, 1.0f};
	//	DataVisualizerWindow* dvWindow;
};
