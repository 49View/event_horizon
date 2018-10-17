#include "ui_label.h"

#include "../font_manager.h"

void UiLabel::initImpl() {
    mFontSize = mSize.y();
	//auto lTextRect = FontManager::optimalSizeForText( mSize->value, fm[mFontName], mTitle, mFontSize );
	//alignOn( lTextRect );
}
