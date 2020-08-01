//
// Created by dado on 01/08/2020.
//

#pragma once

class RenderStats {
public:
    int NumTriangles() const { return mNumTriangles; }
    int& NumTriangles() { return mNumTriangles; }
    void NumTriangles( int val ) { mNumTriangles = val; }
    int NumVerts() const { return mNumVerts; }
    void NumVerts( int val ) { mNumVerts = val; }
    int& NumVerts() { return mNumVerts; }
    int NumIndices() const { return mNumIndices; }
    void NumIndices( int val ) { mNumIndices = val; }
    int& NumIndices() { return mNumIndices; }
    int NumNormals() const { return mNumNormals; }
    void NumNormals( int val ) { mNumNormals = val; }
    int& NumNormals() { return mNumNormals; }
    int NumGeoms() const { return mNumGeoms; }
    void NumGeoms( int val ) { mNumGeoms = val; }
    int& NumGeoms() { return mNumGeoms; }
    const int& getDrawCallsPerFrame() const {
        return drawCallsPerFrame;
    }
    void setDrawCallsPerFrame( int _drawCallsPerFrame ) {
        drawCallsPerFrame = _drawCallsPerFrame;
    }
    int getInMemoryMaterials() const {
        return inMemoryMaterials;
    }
    void setInMemoryMaterials( int _inMemoryMaterials ) {
        RenderStats::inMemoryMaterials = _inMemoryMaterials;
    }
    int getInMemoryVertexBuffers() const {
        return inMemoryVertexBuffers;
    }
    void setInMemoryVertexBuffers( int _inMemoryVertexBuffers ) {
        RenderStats::inMemoryVertexBuffers = _inMemoryVertexBuffers;
    }
    int getInMemoryTextures() const {
        return inMemoryTextures;
    }
    void setInMemoryTextures( int _inMemoryTextures ) {
        RenderStats::inMemoryTextures = _inMemoryTextures;
    }

//    void clear() {
//        drawCallsPerFrame=0;
//        inMemoryMaterials = 0;
//        inMemoryTextures = 0;
//        inMemoryVertexBuffers = 0;
//        mNumTriangles = 0;
//        mNumVerts = 0;
//        mNumIndices = 0;
//        mNumNormals = 0;
//        mNumGeoms = 0;
//    }

private:
    int drawCallsPerFrame=0;
    int inMemoryMaterials=0;
    int inMemoryTextures=0;
    int inMemoryVertexBuffers=0;
    int mNumTriangles=0;
    int mNumVerts=0;
    int mNumIndices=0;
    int mNumNormals=0;
    //	int mNumMaterials;
    int mNumGeoms=0;
    //	std::set < std::shared_ptr<RenderMaterial> > mMaterialSet;
};



