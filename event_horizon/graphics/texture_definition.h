#ifndef __TextureDefinition__
#define __TextureDefinition__

#include "graphic_constants.h"

class TextureDefinition {
public:
	TextureDefinition( const char* id, PixelFormat forceFormat = PIXEL_FORMAT_UNFORCED, WrapMode wrapMode = WRAP_MODE_REPEAT, Filter filterMode = FILTER_LINEAR ) {
		mId = id;
		mForceFormat = forceFormat;
		mWrapMode = wrapMode;
		mFilterMode = filterMode;
	}

	const char* getId() const {
		return mId;
	}

	WrapMode getWrapMode() const {
		return mWrapMode;
	}

	Filter getFilter() const {
		return mFilterMode;
	}

	PixelFormat getForceFormat() const {
		return mForceFormat;
	}

private:
	const char* mId;
	WrapMode mWrapMode;
	Filter   mFilterMode;
	PixelFormat mForceFormat;
};

#endif /* defined(__TextureDefinition__) */
