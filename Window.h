#pragma once

#include <Windows.h>
#include <string>


class Window {
public:
	HWND hwnd;
	HINSTANCE hinstance;

	std::string name;
	int width;
	int height;

	bool keys[256];
	int mousex;
	int mousey;
	bool mouseButtons[3];

	void create(int window_width, int window_height, const std::string window_name);

	void processMessages();

	bool keyPressed(int key) {
		return keys[key];
	}

	void updateMouse(int x, int y) {
		mousex = x;
		mousey = y;
	}

	// Restricts the mouse cursor to the window's client area
	void clipMouseToWindow()
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		POINT ul;
		ul.x = rect.left;
		ul.y = rect.top;
		POINT lr;
		lr.x = rect.right;
		lr.y = rect.bottom;
		MapWindowPoints(hwnd, nullptr, &ul, 1);
		MapWindowPoints(hwnd, nullptr, &lr, 1);
		rect.left = ul.x;
		rect.top = ul.y;
		rect.right = lr.x;
		rect.bottom = lr.y;
		ClipCursor(&rect);
	}
};