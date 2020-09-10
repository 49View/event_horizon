#pragma once

#include "math/vector4f.h"

class UIZHelper {
public:
	static UIZHelper& getInstance() {
		static UIZHelper instance; // Guaranteed to be destroyed.
		return instance;// Instantiated on first use.
	}
private:
	UIZHelper() {
		mZMult = 0.001f;
	};

	UIZHelper( UIZHelper const& ) = delete;
	void operator=( UIZHelper const& ) = delete;
public:
	float level( int level ) {
		return mZMult * level;
	}
private:
	float mZMult;
};

#define UIZ UIZHelper::getInstance()

class UiAlignElement {
public:
	UiAlignElement( const Vector4f& value = V4fc::ZERO );
	UiAlignElement( const float x, const float y, const float z, const float w );

    inline float Width() const { return mElement[0] + mElement[1]; }
	inline float Height() const { return mElement[2] + mElement[3]; }
	inline float Top() const { return mElement[2]; }
	inline float Bottom() const { return mElement[3]; }
	inline float Left() const { return mElement[0]; }
	inline float Right() const { return mElement[1]; }

	Vector2f TopLeft() const { return{ Left(), Top() }; }
	Vector2f TopLeftI() const { return{ Left(), -Top() }; }
	Vector2f BottomRight() const { return{ Right(), Bottom() }; }

	Vector2f Size() const { return{ Width(), Height() }; }

	Vector4f raw() const { return mElement; }

	void injectIfValid( const UiAlignElement& _rhs );

	bool isValid() const { return Left() >= 0.0f; }

public:
	static const UiAlignElement ZERO;

private:
private:
	Vector4f mElement;
};
