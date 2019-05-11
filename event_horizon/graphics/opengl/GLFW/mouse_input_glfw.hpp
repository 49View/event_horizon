//
// Created by Dado on 2019-05-11.
//

#pragma once

struct GLFWwindow;
struct GLFWcursor;

void GscrollCallback( GLFWwindow* window, double xoffset, double yoffset );
void GMouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
