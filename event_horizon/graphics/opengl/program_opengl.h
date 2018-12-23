#pragma once

class Shader;
class Texture;
class SerializeBin;
class DeserializeBin;

#include <map>
#include <string>

#include "core/math/matrix4f.h"
#include "core/math/matrix2f.h"
#include "core/soa_utils.h"
#include "core/math/quad_vertices.h"

#include "../program.h"
#include "../graphic_constants.h"
#include "../texture_manager.h"
#include "gl_util.h"

enum class CreateUpdateFlag {
    Create,
    Update
};

class ProgramOpenGL : public Program {
public:
    ProgramOpenGL( const std::string& id, const std::string& vertexShaderId, const std::string& tesselationControlShaderId,
             const std::string& tesselationEvaluationShaderId, const std::string& geometryShaderId,
             const std::string& fragmentShaderId, const std::string& computeShaderId ) {
        mId = id;
        mVertexShaderId = vertexShaderId;
        mTesselationControlShaderId = tesselationControlShaderId;
        mTesselationEvaluationShaderId = tesselationEvaluationShaderId;
        mGeometryShaderId = geometryShaderId;
        mFragmentShaderId = fragmentShaderId;
        mComputeShaderId = computeShaderId;

        mHandle = 0;
    }

    // Clear the handle when opengl starts
    void clear() {
        mHandle = 0;
    }

    const std::string& getVertexShaderId() {
        return mVertexShaderId;
    }

    const std::string& getTesselationControlShaderId() {
        return mTesselationControlShaderId;
    }

    const std::string& getTesselationEvaluationShaderId() {
        return mTesselationEvaluationShaderId;
    }

    const std::string& getGeometryShaderId() {
        return mGeometryShaderId;
    }

    const std::string& getFragmentShaderId() {
        return mFragmentShaderId;
    }

    const std::string& getComputeShaderId() {
        return mComputeShaderId;
    }

    bool createOrUpdate( std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> tesselationControlShader,
                         std::shared_ptr<Shader> tesselationEvaluationShader, std::shared_ptr<Shader> geometryShader,
                         std::shared_ptr<Shader> fragmentShader, std::shared_ptr<Shader> computeShader );

private:
    std::string mVertexShaderId;
    std::string mTesselationControlShaderId;
    std::string mTesselationEvaluationShaderId;
    std::string mGeometryShaderId;
    std::string mFragmentShaderId;
    std::string mComputeShaderId;
};
