#pragma once

#include <core/math/vector3f.h>
#include <core/math/anim_type.hpp>
#include <core/math/anim.h>
#include <core/resources/light.hpp>

class ProgramUniformSet;
class ShaderManager;
class ShadowMapManager;

class LightBase {
public:
	LightBase( float wattage, float intensity, const Vector3f& attenuation ) {
	    name = "Default";
		mType = LightType_Invalid;
		mWattage = wattage;
		mIntensity = std::make_shared<AnimType<float>>( intensity, "lightIntensity" );
		mAttenuation = std::make_shared<AnimType<Vector3f>>( attenuation, "lightAttenuation" );
	}

	[[nodiscard]] float Intensity() const { return mIntensity->value * mWattage; }
	std::shared_ptr<AnimType<float>>& IntensityAnim() { return mIntensity; }
	void Intensity( float val ) { mIntensity->value = val; }
	[[nodiscard]] Vector3f Attenuation() const { return mAttenuation->value; }
	void Attenuation( const Vector3f& val ) { mAttenuation->value = val; }
	[[nodiscard]] LightType Type() const { return mType; }
	void Type( LightType val ) { mType = val; }
    [[nodiscard]] float getWattage() const { return mWattage; }
    void setWattage( float _wattage ) { LightBase::mWattage = _wattage; }

protected:
	std::string name;
	LightType mType;
	float mWattage;
	std::shared_ptr<AnimType<float>> mIntensity;
	std::shared_ptr<AnimType<Vector3f>>	mAttenuation;
};

class DirectionalLight : public LightBase {
public:
	explicit DirectionalLight( const Vector3f& dir, float wattage = 50.0f, float intensity = 1.0f, const Vector3f& attenuation = Vector3f::ONE ) : LightBase( wattage, intensity, attenuation ) {
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
	explicit PointLight( const Vector3f& pos, float wattage = 50.0f, float intensity = 1.0f, const Vector3f& attenuation = Vector3f::ONE ) : LightBase( wattage, intensity, attenuation ) {
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
	SpotLight( const Vector3f& pos, const Vector3f& dir, float beamAngle = 60.0f, float wattage = 50.0f, float intensity = 1.0f, const Vector3f& attenuation = Vector3f::ONE ) : PointLight( pos, wattage, intensity, attenuation ) {
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

	void addPointLight( const Vector3f& pos, float _wattage, float intensity = 1.0f, const Vector3f& attenuation = Vector3f::ONE );
    void setPointLightWattages( float _watt );
	void removePointLight( size_t index );
	void removeAllPointLights();
	void toggleLightsOnOff();
	void switchLightsOn( float animTime, TimelineGroupCCF _ccf = nullptr );
	void switchLightsOff( float animTime, TimelineGroupCCF _ccf = nullptr );

	void setLightsIntensity( float _intensity );

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
    V4f hdrExposures = V4f::WHITE;
    V4f shadowParameters = V4f{0.001f, 1.0f, 0.0f, 1.0f};
	//	DataVisualizerWindow* dvWindow;
};
