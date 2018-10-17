//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <memory>

class PresenterLayout;
class UiPresenter;

std::shared_ptr<PresenterLayout> fullEditor();
void allConversionsdragAndDropCallback( UiPresenter* p, const std::string& _path );
