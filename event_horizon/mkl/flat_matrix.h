#pragma once
#include <iostream>

class FlatMatrix {
public:
	FlatMatrix( size_t rows, size_t columns, bool initToZero = false );
	FlatMatrix( FlatMatrix& m ) = delete;
	void operator=( FlatMatrix const &x ) = delete;

	FlatMatrix( FlatMatrix&& m );
	FlatMatrix& operator=( FlatMatrix&& other );

	~FlatMatrix();

	FlatMatrix multiplyTranspose( const FlatMatrix & m, size_t fromRows = -1, size_t toRows = -1, float alpha = 1.0f );
	FlatMatrix multiply( const FlatMatrix & m, float alpha = 1.0f );
	size_t countValues( float value );
	void dumpMatrixTo( std::basic_ostream<char, std::char_traits<char>> &stream, std::string description, const char separator = '\t' );

	size_t rows;
	size_t columns;
	float* data;
};