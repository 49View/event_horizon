#include "flat_matrix.h"
#include <string>
#include "mkl.h"

FlatMatrix::FlatMatrix( size_t rows, size_t columns, bool initToZero ) {
	this->rows = rows;
	this->columns = columns;
	data = (float *)mkl_malloc( rows * columns * sizeof( float ), 64 );

	if ( initToZero ) {
		for ( size_t i = 0; i < rows * columns; i++ ) {
			data[i] = 0.0f;
		}
	}
}

FlatMatrix::FlatMatrix( FlatMatrix&& m )
	:rows{ m.rows }, columns{ m.columns }, data{ m.data } {
	m.data = nullptr;
}

FlatMatrix& FlatMatrix::operator=( FlatMatrix&& other ) {
	std::swap( rows, other.rows );
	std::swap( columns, other.columns );
	std::swap( data, other.data );
	return *this;
}

FlatMatrix::~FlatMatrix() {
	if ( data )
		mkl_free( data );
}

FlatMatrix FlatMatrix::multiplyTranspose( const FlatMatrix& m, size_t /*fromRows*/, size_t /*toRows*/, float alpha ) {
	size_t lda = columns;
	size_t ldb = columns;
	size_t ldc = m.rows;

	float beta = 0.0f;

	FlatMatrix result = FlatMatrix( rows, m.rows, true );

	cblas_sgemm( CblasRowMajor, CblasNoTrans, CblasTrans, static_cast<MKL_INT>( rows ), static_cast<MKL_INT>(m.rows),
				 static_cast<MKL_INT>(columns), alpha, data, static_cast<MKL_INT>(lda), m.data, static_cast<MKL_INT>(ldb), beta, result.data, static_cast<MKL_INT>(ldc) );

	return result;
}

FlatMatrix FlatMatrix::multiply( const FlatMatrix& m, float alpha ) {
	size_t lda = columns;
	size_t ldb = m.columns;
	size_t ldc = m.columns;

	float beta = 0.0f;

	FlatMatrix result = FlatMatrix( rows, m.columns );

	cblas_sgemm( CblasRowMajor, CblasNoTrans, CblasNoTrans, static_cast<MKL_INT>(rows), static_cast<MKL_INT>(m.columns), static_cast<MKL_INT>(columns), alpha, data, static_cast<MKL_INT>(lda), m.data, static_cast<MKL_INT>(ldb), beta, result.data, static_cast<MKL_INT>(ldc) );

	return result;
}

size_t FlatMatrix::countValues( float value ) {
	int count = 0;

	for ( size_t i = 0; i < rows * columns; i++ ) {
		if ( data[i] == value ) {
			count++;
		}
	}
	return count;
}

void FlatMatrix::dumpMatrixTo( std::basic_ostream<char, std::char_traits<char>> &stream, std::string description, const char separator ) {
	//if ( !mainParameters.dumpMatrices ) return;

	char newLine = '\n';
	stream << description << newLine;
	for ( size_t r = 0; r < rows; r++ ) {
		for ( size_t c = 0; c < columns; c++ ) {
			stream << separator << data[r * columns + c];
		}
		stream << newLine;
	}
	stream << newLine;
}
