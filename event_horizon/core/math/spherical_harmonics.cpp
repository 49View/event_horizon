
#include "spherical_harmonics.h"
#include <thread>

SphericalHarmonics::SphericalHarmonics() {
	mSqrtNumSamples = 50;
	mNumBands = 3;
	mNumSamples = mSqrtNumSamples*mSqrtNumSamples;
	mNumFunctions = mNumBands*mNumBands;
	lightCoeffs = std::unique_ptr<float[]>( new float[mNumFunctions] );
	rotatedLightCoeffs = std::unique_ptr<float[]>( new float[mNumFunctions] );
	mNumTotalVerticesToElaborate = 0;
};

void SphericalHarmonics::NumSamples( int _ns ) {
	mSqrtNumSamples = _ns;
	mNumSamples = mSqrtNumSamples*mSqrtNumSamples;
}

//Evaluate an Associated Legendre Polynomial P(l, m) at x
double P( int l, int m, double x ) {
	//First generate the value of P(m, m) at x
	double pmm = 1.0;

	if ( m > 0 ) {
		double sqrtOneMinusX2 = sqrt( 1.0 - x*x );

		double fact = 1.0;

		for ( int i = 1; i <= m; ++i ) {
			pmm *= ( -fact )*sqrtOneMinusX2;
			fact += 2.0;
		}
	}

	//If l==m, P(l, m)==P(m, m)
	if ( l == m )
		return pmm;

	//Use rule 3 to calculate P(m+1, m) from P(m, m)
	double pmp1m = x*( 2.0*m + 1.0 )*pmm;

	//If l==m+1, P(l, m)==P(m+1, m)
	if ( l == m + 1 )
		return pmp1m;

	//Otherwise, l>m+1.
	//Iterate rule 1 to get the result
	double plm = 0.0;

	for ( int i = m + 2; i <= l; ++i ) {
		plm = ( ( 2.0*i - 1.0 )*x*pmp1m - ( i + m - 1.0 )*pmm ) / ( i - m );
		pmm = pmp1m;
		pmp1m = plm;
	}

	return plm;
}

//Calculate the normalisation constant for an SH function
//No need to use |m| since SH always passes positive m
double K( int l, int m ) {
	double temp = ( ( 2.0*l + 1.0 )*Factorial( l - m ) ) / ( ( 4.0*M_PI )*Factorial( l + m ) );

	return sqrt( temp );
}

//Sample a spherical harmonic basis function Y(l, m) at a point on the unit sphere
double SHC( int l, int m, double theta, double phi ) {
	//return 0.5 / M_PI;

	const double sqrt2 = sqrt( 2.0 );

	if ( m == 0 )
		return K( l, 0 )*P( l, m, cos( theta ) );

	if ( m > 0 )
		return sqrt2*K( l, m )*cos( m*phi )*P( l, m, cos( theta ) );

	//m<0
	return sqrt2*K( l, -m )*sin( -m*phi )*P( l, -m, cos( theta ) );
}

//Calculate n! (n>=0)
int Factorial( int n ) {
	if ( n <= 1 )
		return 1;

	int result = n;

	while ( --n > 1 )
		result *= n;

	return result;
}

void SphericalHarmonics::generateSamples() {
	//Create space for the SH values in each sample
	mSamples = std::unique_ptr<SHSample[]>( new SHSample[mNumSamples] );
	for ( int i = 0; i < mNumSamples; ++i ) {
		mSamples[i].shValues = std::unique_ptr<double[]>( new double[mNumFunctions] );
	}

	int index = 0;

	for ( int i = 0; i < mSqrtNumSamples; ++i ) {
		for ( int j = 0; j < mSqrtNumSamples; ++j ) {
			//Generate the position of this sample in [0, 1)x[0, 1)
			double x = ( i + ( (double)rand() / RAND_MAX ) ) / mSqrtNumSamples;
			double y = ( j + ( (double)rand() / RAND_MAX ) ) / mSqrtNumSamples;

			//Convert to spherical polars
			double theta = 2.0*acos( sqrt( 1.0 - x ) );
			double phi = 2.0*M_PI*y;

			mSamples[index].theta = theta;
			mSamples[index].phi = phi;

			//Convert to cartesians
			mSamples[index].direction = { float( sin( theta )*cos( phi ) ), float( sin( theta )*sin( phi ) ), float( cos( theta ) ) };

			//Compute SH coefficients for this sample
			for ( int l = 0; l < mNumBands; ++l ) {
				for ( int m = -l; m <= l; ++m ) {
					int index2 = l*( l + 1 ) + m;
					double avrTheta = .5056209;
					double sh1 = SHC( l, m, theta, phi );
					//sh1 = sin( sh1 ) / sh1;
					mSamples[index].shValues[index2] = sh1 / ( 1.0 + ( avrTheta*l*l )*( ( l + 1.0 ) * ( l + 1.0 ) ) );
				}
			}

			++index;
		}
	}
}

double SphericalHarmonics::light( double theta, double /*phi*/ ) {
	//return 1.0f;
	//return ( phi < 0.5 ) ? 1.0 : 0.0;
	return ( theta < M_PI / 6 ) ? M_PI : 0;
}

void SphericalHarmonics::generateLightsCoeffs( double initialTheta, double initialPhi ) {
	for ( int i = 0; i < mNumFunctions; ++i ) {
		lightCoeffs[i] = 0.0;

		for ( int j = 0; j < mNumSamples; ++j )
			lightCoeffs[i] += static_cast<float>( light( mSamples[j].theta, mSamples[j].phi ) * mSamples[j].shValues[i] );

		lightCoeffs[i] *= ( 4.0f * M_PI ) / mNumSamples;
	}

	rotateSHCoefficients( initialTheta, initialPhi );
}

void GetZRotationMatrix( int band, double * entries, double angle ) {
	//Calculate the size of the matrix
	int size = 2 * band + 1;

	//Entry index
	int currentEntry = 0;

	//Loop through the rows and columns of the matrix
	for ( int i = 0; i < size; ++i ) {
		for ( int j = 0; j < size; ++j, ++currentEntry ) {
			//Initialise this entry to zero
			entries[currentEntry] = 0.0;

			//For the central row (i=(size-1)/2), entry is 1 if j==i, else zero
			if ( i == ( size - 1 ) / 2 ) {
				if ( j == i )
					entries[currentEntry] = 1.0;

				continue;
			}

			//For i<(size-1)/2, entry is cos if j==i or sin if j==size-i-1
			//The angle used is k*angle where k=(size-1)/2-i
			if ( i < ( size - 1 ) / 2 ) {
				int k = ( size - 1 ) / 2 - i;

				if ( j == i )
					entries[currentEntry] = cos( k*angle );

				if ( j == size - i - 1 )
					entries[currentEntry] = sin( k*angle );

				continue;
			}

			//For i>(size-1)/2, entry is cos if j==i or -sin if j==size-i-1
			//The angle used is k*angle where k=i-(size-1)/2
			if ( i > ( size - 1 ) / 2 ) {
				int k = i - ( size - 1 ) / 2;

				if ( j == i )
					entries[currentEntry] = cos( k*angle );

				if ( j == size - i - 1 )
					entries[currentEntry] = -sin( k*angle );

				continue;
			}
		}
	}

	return;
}

void GetX90DegreeRotationMatrix( int band, double * entries ) {
	//Ensure that 0<=band<=3
	if ( band > 3 ) {
		throw "X rotation matrices are only known for bands 0-3";
	}

	if ( band == 0 ) {
		entries[0] = 1.0;
	}

	if ( band == 1 ) {
		entries[0] = 0.0;
		entries[1] = 1.0;
		entries[2] = 0.0;
		entries[3] = -1.0;
		entries[4] = 0.0;
		entries[5] = 0.0;
		entries[6] = 0.0;
		entries[7] = 0.0;
		entries[8] = 1.0;
	}

	if ( band == 2 ) {
		entries[0] = 0.0;
		entries[1] = 0.0;
		entries[2] = 0.0;
		entries[3] = 1.0;
		entries[4] = 0.0;
		entries[5] = 0.0;
		entries[6] = -1.0;
		entries[7] = 0.0;
		entries[8] = 0.0;
		entries[9] = 0.0;
		entries[10] = 0.0;
		entries[11] = 0.0;
		entries[12] = -0.5;
		entries[13] = 0.0;
		entries[14] = -sqrt( 3.0 ) / 2;
		entries[15] = -1.0;
		entries[16] = 0.0;
		entries[17] = 0.0;
		entries[18] = 0.0;
		entries[19] = 0.0;
		entries[20] = 0.0;
		entries[21] = 0.0;
		entries[22] = -sqrt( 3.0 ) / 2;
		entries[23] = 0.0;
		entries[24] = 0.5;
	}

	if ( band == 3 ) {
		//Initialise all entries to 0
		for ( int i = 0; i < 49; ++i )
			entries[i] = 0.0;

		entries[3] = -sqrt( 0.625 );
		entries[5] = sqrt( 0.375 );

		entries[8] = -1.0;

		entries[17] = -sqrt( 0.375 );
		entries[19] = -sqrt( 0.625 );

		entries[21] = sqrt( 0.625 );
		entries[23] = sqrt( 0.375 );

		entries[32] = -0.25;
		entries[34] = -sqrt( 15.0 ) / 4;

		entries[35] = -sqrt( 0.375 );
		entries[37] = sqrt( 0.625 );

		entries[46] = -sqrt( 15.0 ) / 4;
		entries[48] = 0.25;
	}

	return;
}

void ApplyMatrix( int size, double * matrix, bool transpose, float * inVector, double * outVector ) {
	//Loop through entries
	for ( int i = 0; i < size; ++i ) {
		//Clear this entry of outVector
		outVector[i] = 0.0;

		//Loop through matrix row/column
		for ( int j = 0; j < size; ++j ) {
			if ( transpose )
				outVector[i] += matrix[j*size + i] * inVector[j];
			else
				outVector[i] += matrix[i*size + j] * inVector[j];
		}
	}
}

void ApplyMatrix( int size, double * matrix, bool transpose, double * inVector, double * outVector ) {
	//Loop through entries
	for ( int i = 0; i < size; ++i ) {
		//Clear this entry of outVector
		outVector[i] = 0.0;

		//Loop through matrix row/column
		for ( int j = 0; j < size; ++j ) {
			if ( transpose )
				outVector[i] += matrix[j*size + i] * inVector[j];
			else
				outVector[i] += matrix[i*size + j] * inVector[j];
		}
	}
}

void ApplyMatrix( int size, double * matrix, bool transpose, double * inVector, float * outVector ) {
	//Loop through entries
	for ( int i = 0; i < size; ++i ) {
		//Clear this entry of outVector
		outVector[i] = 0.0;

		//Loop through matrix row/column
		for ( int j = 0; j < size; ++j ) {
			if ( transpose )
				outVector[i] += static_cast<float>( matrix[j*size + i] * inVector[j] );
			else
				outVector[i] += static_cast<float>( matrix[i*size + j] * inVector[j] );
		}
	}
}

void SphericalHarmonics::rotateSHCoefficients( double theta, double phi ) {
	int numFunctions = mNumBands*mNumBands;

	for ( int i = 0; i < numFunctions; ++i )
		rotatedLightCoeffs[i] = lightCoeffs[i];

	//Band 0 coefficient is unchanged
	rotatedLightCoeffs[0] = lightCoeffs[0];

	//Rotate band 1 coefficients
	if ( mNumBands > 1 ) {
		//Get the rotation matrices for band 1 (want to apply Z1*Xt*Z2*X)
		double band1X[9];
		double band1Z1[9];
		double band1Z2[9];

		GetZRotationMatrix( 1, band1Z1, phi );
		GetX90DegreeRotationMatrix( 1, band1X );
		GetZRotationMatrix( 1, band1Z2, theta );

		//Create space to hold the intermediate results
		double band1A[3], band1B[3], band1C[3];

		//Apply the matrices
		ApplyMatrix( 3, band1X, false, &lightCoeffs[1], band1A );
		ApplyMatrix( 3, band1Z2, false, band1A, band1B );
		ApplyMatrix( 3, band1X, true, band1B, band1C );

		ApplyMatrix( 3, band1Z1, false, band1C, &rotatedLightCoeffs[1] );
	}

	//Rotate band 2 coefficients
	if ( mNumBands > 2 ) {
		double band2X[25];
		double band2Z1[25];
		double band2Z2[25];

		GetZRotationMatrix( 2, band2Z1, phi );
		GetX90DegreeRotationMatrix( 2, band2X );
		GetZRotationMatrix( 2, band2Z2, theta );

		//Create space to hold the intermediate results
		double band2A[5], band2B[5], band2C[5];

		//Apply the matrices
		ApplyMatrix( 5, band2X, false, &lightCoeffs[4], band2A );
		ApplyMatrix( 5, band2Z2, false, band2A, band2B );
		ApplyMatrix( 5, band2X, true, band2B, band2C );

		ApplyMatrix( 5, band2Z1, false, band2C, &rotatedLightCoeffs[4] );
	}

	//Rotate band 3 coefficients
	if ( mNumBands > 3 ) {
		double band3X[49];
		double band3Z1[49];
		double band3Z2[49];

		GetZRotationMatrix( 3, band3Z1, phi );
		GetX90DegreeRotationMatrix( 3, band3X );
		GetZRotationMatrix( 3, band3Z2, theta );

		//Create space to hold the intermediate results
		double band3A[7], band3B[7], band3C[7];

		//Apply the matrices
		ApplyMatrix( 7, band3X, false, &lightCoeffs[9], band3A );
		ApplyMatrix( 7, band3Z2, false, band3A, band3B );
		ApplyMatrix( 7, band3X, true, band3B, band3C );

		ApplyMatrix( 7, band3Z1, false, band3C, &rotatedLightCoeffs[9] );
	}

	//for ( auto t = 0; t < numFunctions; t++ ) {
	//	LOGR( "LIG SHCoeff%d = %f", t, lightCoeffs[t] );
	//	LOGR( "ROT SHCoeff%d = %f", t, rotatedLightCoeffs[t] );
	//}
}