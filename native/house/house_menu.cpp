#include "house_menu.h"

#include "house/house_menu_list.h"
#include "graphics/ui/ui_control_manager.hpp"
#include "core/raw_image.h"
#include "vr_manager.hpp"

HouseMenu::HouseMenu() {
	sceneManager = ServiceFactory::get<SceneManager>();
	menuScene = std::make_shared<MenuScene>();
	menuScene->Enabled( false );
	sceneManager->addScene( menuScene );

//	mRoot = std::make_shared<UiControlGroup>();
}

void HouseMenu::CreateHeader() {
//	mRoot->startTag( "Header" );
//	mRoot->addControl<UiImage>( logoSize, C4fc::WHITE, "logo", UiControlFlag::FloatLeft );
//
//	mRoot->addControl<UiImage>( Vector2f{ rowWidth(), 0.01f }, C4fc::WHITE, "menu_header_bottom", UiControlFlag::FloatLeft );
//	mRoot->endTag( "Header" );
}

void HouseMenu::CreateFooter() {
}

void HouseMenu::CreateBody( [[maybe_unused]] const std::vector<std::shared_ptr<HouseMenuListItem>>& items ) {
//	mRoot->startTag( "Tiles" );
//
//	for ( auto& item : items ) {
//		TM.addTextureWithData( item->imageName, item->image );
//		auto img = mRoot->addControl<UiImage>( tileSize, C4fc::WHITE, item->imageName, UiControlFlag::FloatLeft );
//		img->setOnClickAction( item->onSelected );
//	}
//
//	mRoot->endTag( "Tiles" );
}

void HouseMenu::CreateTile() {
}

void HouseMenu::CreatePageSelector() {
}

void HouseMenu::init( [[maybe_unused]] const std::vector<std::shared_ptr<HouseMenuListItem>>& items ) {
//	menuScene->reset();
//	UICM.removeControlGroup( mRoot );
//
//	Vector2f hs = menuSize();
//	MatrixAnim animMatrix;
//	animMatrix.Pos( Vector3f( 0.0f, 0.0f, 0.0f ) );
//	animMatrix.AngleX( M_PI_2 );
//
//	Matrix4f preRot = Matrix4f::IDENTITY();
//
//	Matrix4f xRot = Matrix4f::IDENTITY();
//	Matrix4f yRot = Matrix4f::IDENTITY();
//
//	//!!menu rotation angles!!
//	xRot.setRotation( M_PI_4, V3fc::X_AXIS );
//	yRot.setRotation( M_PI_2, V3fc::Z_AXIS );
//
//	preRot = xRot * yRot * preRot;
//
//	//mRoot->init(Rect2f(-hs*Vector2f{ 0.5f,-0.5f }, hs*Vector2f{ 0.5f,-.5f }), C4fc::PASTEL_GRAY, UiControlFlag::HasRoundedCorners, animMatrix, preRot);
//	mRoot->init( Rect2f( hs * Vector2f{ -0.5f, 1.0f }, hs*Vector2f{ 0.5f, 0.0f } ), C4fc::PASTEL_GRAY, UiControlFlag::HasRoundedCorners, animMatrix, preRot );
//
//	mRoot->Padding( tilesMargins );
//
//	if ( showHeader )
//		CreateHeader();
//
//	CreateBody( items );
//
//	if ( showFooter )
//		CreateFooter();
//
//	UICM.addControlGroup( mRoot );
//	mRoot->setVisible( false );
//
//	auto buffers = mRoot->createVertexBuffers();
//	for ( auto& buffer : buffers ) {
//		int64_t id = menuId + buffer->Id();
//		buffer->Id( id );
//		if ( id > controlsMaxId )
//			controlsMaxId = id;
//	}
//
//	menuScene->addBuffers( buffers );
//
//	mRoot->setVisible( true );
//	menuScene->Enabled( true );
//	mIsVisible = true;
}

void HouseMenu::update() {
	if ( !mIsVisible ) return;

	auto controllerInfo = false ? VRM.RightHandController() : VRM.LeftHandController();

	Matrix4f rotMat = controllerInfo.transformMatrix;

	Matrix4f rotS;
	rotMat.invert( rotS );

	rotS.setTranslation( V3fc::ZERO );
	rotS.invert( rotS );

	Matrix4f rotSF = rotS;

	rotSF.setRow( 0, rotS.getRow( 2 ) );
	rotSF.setRow( 1, rotS.getRow( 0 ) );
	rotSF.setRow( 2, rotS.getRow( 1 ) );

	mRoot->OrigRot( rotSF );

	Vector4f position4f = controllerInfo.transformMatrix * Vector4f( 0, 0.05, -0.1, 1 );
	Vector3f position = { position4f.x(), position4f.y(), position4f.z() };
	mRoot->AnimMatrix().Pos( position );
}

void HouseMenu::hide() {
	mRoot->setVisible( false );
	menuScene->Enabled( false );
	mIsVisible = false;
}

Vector2f HouseMenu::size() const {
	return mRoot->size();
}

void HouseMenu::SetLayout( int tilesOnRow, int totalRows, bool withHeader, bool withFooter ) {
	tilesPerRow = tilesOnRow;
	rows = totalRows;
	showHeader = withHeader;
	showFooter = withFooter;
}

bool HouseMenu::isManagingObject( int64_t id ) {
	return id >= menuId && id <= controlsMaxId;
}

void HouseMenu::executeTriggerPressed( [[maybe_unused]] const TriggerPressedInfo& i ) {
	//mRoot->onTouchUp( i.hitInfo.objectId - menuId, /*i.hitInfo.finalPosition*/ V2fc::ZERO );
}