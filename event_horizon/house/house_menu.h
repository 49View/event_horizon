#pragma once

#include <vector>
#include "graphics/ui/ui_button.h"
#include "graphics/ui/ui_label.h"
#include "graphics/ui/ui_background.h"
#include "graphics/ui/ui_image.h"
#include "graphics/ui/ui_control_group.h"

#include "core/service_factory.h"
#include "ray_tracing/scene_manager.h"
#include "ray_tracing/menu_scene.h"
#include "vr/triggerable.h"

class Camera;
class HouseMenuListItem;

class HouseMenu : public Triggerable {
public:
	HouseMenu();
	void init( const std::vector<std::shared_ptr<HouseMenuListItem>>& items );

	void update();

	void hide();
	bool isVisible() { return mRoot->isVisible(); }
	Vector2f size() const;
	void SetLayout( int tilesOnRow, int totalRows, bool withHeader, bool withFooter );

	virtual bool isManagingObject( int64_t id ) override;
	virtual void executeTriggerPressed( const TriggerPressedInfo& i ) override;

private:
	void CreateHeader();
	void CreateFooter();
	void CreateBody( const std::vector<std::shared_ptr<HouseMenuListItem>>& items );
	void CreateTile();
	void CreatePageSelector();

	float rowWidth() const { return ( tilesPerRow * tileSize.x() ) + ( tilesPerRow * tilesMargins.x() * 2.0f ) + 0.001f; }
	Vector2f headerSize() const { return showHeader ? Vector2f{ rowWidth(), logoSize.y() } : Vector2f::ZERO; }
	Vector2f footerSize() const { return showFooter ? Vector2f{ rowWidth(), logoSize.y() } : Vector2f::ZERO; }
	Vector2f bodySize() const { return Vector2f{ rowWidth(), rows * ( tileSize.y() + tilesMargins.y() + tilesMargins.w() ) }; }
	Vector2f menuSize() const { return headerSize() + bodySize() + footerSize(); }

private:

	int64_t menuId = 100;
	int64_t controlsMaxId = 100;

	bool showHeader = false;
	bool showFooter = false;

	bool mIsVisible = false;

	Vector2f initialPositon = Vector2f::ONE * -2.0f;

	//x is left, y is top, z is right, w is bottom
	Vector4f tilesMargins = Vector4f::ONE * 0.005f;
	Vector4f logoMargins = Vector4f::ONE * 0.05f;

	Vector4f navigationMargins = Vector4f::ONE * 0.05f;

	//x is the width, y the height
	Vector2f tileSize = Vector2f::ONE * 0.05f;
	Vector2f logoSize = Vector2f{ 1.0, 0.5 };

	size_t tilesPerRow = 3;
	size_t rows = 1;

	std::shared_ptr<UiControlGroup> mRoot;

	std::shared_ptr<UiButton> backButton;
	std::shared_ptr<UiButton> forwardButton;

	std::vector<std::shared_ptr<UiButton>> tileButtons;
	std::shared_ptr<SceneManager> sceneManager;
	std::shared_ptr<MenuScene> menuScene;
};
