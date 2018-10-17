#pragma once

#include <iterator>
#include <vector>

struct ChunkInfo {
	size_t from;
	size_t to;
	size_t index;
};

class Chunks {
public:
	auto begin() { return chunkInfos.begin(); }
	auto end() { return chunkInfos.end(); }
	auto begin() const { return chunkInfos.begin(); }
	auto end() const { return chunkInfos.end(); }
	auto cbegin() const { return chunkInfos.cbegin(); }
	auto cend() const { return chunkInfos.cend(); }

	static Chunks ByNumber( size_t chunksNumber, size_t totalElements );
	static Chunks BySize( size_t maxSize, size_t totalElements );

	~Chunks();
private:
	Chunks();

	size_t chunksNumber;
	size_t totalElements;
	std::vector<ChunkInfo> chunkInfos;
};
