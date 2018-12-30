#ifndef D_MATH_SH_H
#define D_MATH_SH_H

#include "vector3f.h"
#include "matrix4f.h"
#include "rect2f.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone

//Evaluate an Associated Legendre Polynomial P(l, m) at x
double P( int l, int m, double x );

//Calculate the normalisation constant for an SH function
double K( int l, int m );

//Sample a spherical harmonic basis function Y(l, m) at a point on the unit sphere
double SHC( int l, int m, double theta, double phi );

//Calculate n!
int Factorial( int n );

class SHSample {
public:
	//Spherical polar coords
	double theta;
	double phi;

	//Cartesian direction
	Vector3f direction;

	//Values of each SH function at this point
	std::unique_ptr<double[]> shValues;
};

class SphericalHarmonics {
public:
	static SphericalHarmonics& getInstance() {
		static SphericalHarmonics instance; // Guaranteed to be destroyed.
		return instance;// Instantiated on first use.
	}
private:
	SphericalHarmonics();
	SphericalHarmonics( SphericalHarmonics const& ) = delete;
	void operator=( SphericalHarmonics const& ) = delete;

public:
	void generateSamples();
	void generateLightsCoeffs( double initialTheta = 0, double initialPhi = 0 );
	double light( double theta, double phi );
	int NumSamples() const { return mNumSamples; }
	void NumSamples( int _ns );
	int NumFunctions() { return mNumFunctions; }
	SHSample* Sample( int index ) { return &mSamples[index]; }
	float* LightCoeffs() { return rotatedLightCoeffs.get(); }
	void rotateSHCoefficients( double theta, double phi );
private:

private:
	int mSqrtNumSamples;
	int mNumBands;
	int mNumSamples;
	int mNumFunctions;
	int mNumTotalVerticesToElaborate;
	//	int mNumVerticesElaborated;
	//	float mCompletedPercentage;
	//	int mRoundedCompletedPercentage;
	//	bool mTriggerPercChange;

	std::unique_ptr<SHSample[]> mSamples;

	////SH coefficients for light source, before and after rotation
	std::unique_ptr<float[]> lightCoeffs;
	std::unique_ptr<float[]> rotatedLightCoeffs;
};

#define SSH SphericalHarmonics::getInstance()

#endif // D_SH_H
