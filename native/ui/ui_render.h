#pragma  once

#include <memory>

#include "core/math/vector4f.h"
#include "core/math/matrix4f.h"
#include "core/math/matrix_anim.h"

class UiControl;
class UiBackground;
class UiButton;
class VPList;
class UiLabel;
class UiImage;
class UiCheckBox;
class UiSlider;
class UiInputBox;
//class QuadVertexBuffer;
class Texture;
class Renderer;

namespace Utility { namespace TTFCore { class Font; }}

class UiTextInputRenderer {
public:
	~UiTextInputRenderer() = default;
	UiTextInputRenderer( Renderer& _rr );

	void printTextInput( const std::string& _title, const std::string& _text,
						 const Font& _font );
	void clear();

protected:
	Renderer& rr;
};

class UiControlRenderer {
public:
    UiControlRenderer( Renderer& _rr, std::shared_ptr<UiControl> _c ) : rr(_rr), mControl(_c) {
		mTransform = std::make_shared<Matrix4f>();
    }

	virtual ~UiControlRenderer();

	const std::shared_ptr<Matrix4f> getTransform() const { return mTransform; }
	MatrixAnim& AnimMatrix() { return mAnimMatrix; }
	const Vector3f& UINormalAxis() const { return mUINormalAxis; }
	void UINormalAxis( const Vector3f& val );
	void OrigRot( const Matrix4f& val );
	void PreRotation( const Matrix4f& val ) { mPreRotation = val; }
	const MatrixAnim& getAnimMatrix() const {
		return mAnimMatrix;
	}

	void update( const MatrixAnim& dadTransform );
	virtual void make() = 0;

protected:
	Renderer& rr;
	MatrixAnim mAnimMatrix;
	Matrix4f origRot = Matrix4f::IDENTITY();
	Matrix4f mPreRotation = Matrix4f::IDENTITY();
	std::shared_ptr<Matrix4f> mTransform;
	Vector3f mUINormalAxis = V3fc::Z_AXIS;
	std::weak_ptr<UiControl> mControl;
};

class UiBackgroundRender : public UiControlRenderer {
public:
	using UiControlRenderer::UiControlRenderer;
	void make();
};

class UiButtonRender : public UiControlRenderer {
public:
	using UiControlRenderer::UiControlRenderer;
	void make();
};

class UiLabelRender : public UiControlRenderer {
public:
	using UiControlRenderer::UiControlRenderer;
	void make();
};

class UiImageRender : public UiControlRenderer {
public:
	using UiControlRenderer::UiControlRenderer;
	void make();
};

class UiCheckBoxRender : public UiControlRenderer {
public:
	using UiControlRenderer::UiControlRenderer;
	void make();
};

class UiSeparatorRender : public UiControlRenderer {
public:
	using UiControlRenderer::UiControlRenderer;
	void make();
};

class UiSliderRender : public UiControlRenderer {
public:
	using UiControlRenderer::UiControlRenderer;
	void make();
private:
	std::shared_ptr<Texture> ui_slider_bg_left;
	std::shared_ptr<Texture> ui_slider_bg_right;
	std::shared_ptr<Texture> ui_slider_cap_left;
	std::shared_ptr<Texture> ui_slider_cap_right;
	std::shared_ptr<Texture> ui_slider_ball;
};

class UiInputBoxRender : public UiControlRenderer {
public:
	using UiControlRenderer::UiControlRenderer;
	void make();
};
