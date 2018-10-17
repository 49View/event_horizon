#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "core/raw_image.h"

class HouseMenuListItem {
public:
	std::string description;
	std::string imageName;
	RawImage image;

	std::function<void( void )> onSelected;
};

#define ITEMS_PER_PAGE 6

class HouseMenuList {
public:
	HouseMenuList( std::string listName );

	void setDataSource( std::vector<std::shared_ptr<HouseMenuListItem>> items );
	void itemSelected( size_t index );
	void pageChanged( size_t page );
	std::vector<std::shared_ptr<HouseMenuListItem>> getCurrentPageItems();

	std::string Name() const { return name; }
	size_t CurrentPage() const { return currentPage; }
	size_t Pages() const { return totalPages; }

private:

private:
	std::string name;
	size_t currentPage;
	size_t totalPages;
	std::vector<std::shared_ptr<HouseMenuListItem>> dataSource;
};
