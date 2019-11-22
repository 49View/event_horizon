#pragma once

struct Triangle {
	Triangle( const int inV0, const int inV1, const int inV2 ) {
		v0 = inV0;
		v1 = inV1;
		v2 = inV2;
	}
	int v0, v1, v2;
};

struct Quad {
	Quad( const int inV0, const int inV1, const int inV2, const int inV3 ) {
		v0 = inV0;
		v1 = inV1;
		v2 = inV2;
		v3 = inV3;
	}

	int v0, v1, v2, v3;
};
