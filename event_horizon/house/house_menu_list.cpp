#include "house_menu_list.h"
#include <cmath>

HouseMenuList::HouseMenuList( std::string listName ) {
	name = listName;
	currentPage = 0;
	totalPages = 0;
}

void HouseMenuList::setDataSource( std::vector<std::shared_ptr<HouseMenuListItem>> items ) {
	dataSource = items;
	currentPage = 1;
	totalPages = (size_t)ceil( items.size() / static_cast<float>( ITEMS_PER_PAGE ) );
}

void HouseMenuList::itemSelected( size_t index ) {
	//if (index < 0) return;
	size_t current = ( currentPage - 1 ) * ITEMS_PER_PAGE + index;
	if ( current >= dataSource.size() ) return;
	dataSource[current]->onSelected();
}

void HouseMenuList::pageChanged( size_t page ) {
	if ( page < 1 || page> totalPages ) return;
	currentPage = page;
}

std::vector<std::shared_ptr<HouseMenuListItem>>  HouseMenuList::getCurrentPageItems() {
	size_t from = ( currentPage - 1 ) * ITEMS_PER_PAGE;
	size_t to = from + ( dataSource.size() - from < ITEMS_PER_PAGE ? dataSource.size() - from : ITEMS_PER_PAGE ) - 1;
	return std::vector<std::shared_ptr<HouseMenuListItem>>( dataSource.begin() + from, dataSource.end() + to );
}