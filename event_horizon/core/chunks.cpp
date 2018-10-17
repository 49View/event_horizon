#include "chunks.h"

Chunks::Chunks() {
	chunksNumber = 0;
	totalElements = 0;
}

Chunks::~Chunks() {
	chunkInfos.clear();
}

Chunks Chunks::BySize( size_t maxSize, size_t totalElements ) {
	size_t chunksNumber = maxSize == 0 ? 0 : ( totalElements + maxSize - 1 ) / maxSize;
	return ByNumber( chunksNumber, totalElements );
}

Chunks Chunks::ByNumber( size_t chunksNumber, size_t totalElements ) {
	Chunks chunks;

	if ( totalElements == 0 ) {
		return chunks;
	}

	if ( chunksNumber < 2 ) {
		chunks.chunksNumber = 1;
		chunks.totalElements = totalElements;
		ChunkInfo bi = { 0, totalElements, 1 };
		chunks.chunkInfos.push_back( bi );
		return chunks;
	}

	chunks.chunksNumber = chunksNumber;
	chunks.totalElements = totalElements;

	size_t chunkSize = totalElements / chunksNumber;
	size_t overSized = totalElements % chunksNumber;

	size_t from = 0;
	size_t to = 0;

	for ( size_t i = 0; i < chunksNumber; i++ ) {
		from = to; to += chunkSize;	if ( overSized > 0 ) { to++; overSized--; }
		ChunkInfo bi = { from, to, i };
		chunks.chunkInfos.push_back( bi );
	}

	return chunks;
}