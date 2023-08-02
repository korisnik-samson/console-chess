#pragma once
#define _WIN32_WINNT 0x0500

#include <Windows.h>
#include "Byte88.h"
#include "Layer.h"
#include "PixelFont.h"
#include <functional>
#include <vector>

// Typedefs for event handler delegate types
typedef std::function<void(KEY_EVENT_RECORD)> KEY_EVENT_PROC;
typedef std::function<void(MOUSE_EVENT_RECORD)> MOUSE_EVENT_PROC;
typedef std::function<void(MENU_EVENT_RECORD)> MENU_EVENT_PROC;
typedef std::function<void(FOCUS_EVENT_RECORD)> FOCUS_EVENT_PROC;
typedef std::function<void(WINDOW_BUFFER_SIZE_RECORD)> BUFFER_EVENT_PROC;

// Size of the event record buffer for GameWindow object
#define SZ_RECORD_BUFFER 128

class GameWindow {

private:
	HANDLE hConsoleIn;
	HANDLE hConsoleOut;
	INPUT_RECORD inputRecords[SZ_RECORD_BUFFER];

	COLORREF cmapDefault[16];

	bool ready;
	int width;
	int height;

	Layer renderBuffer;	
	Layer backBuffer;	

	void setConsoleBufferSize(int row, int col) {
		// Use SetConsoleScreenBufferSize to set the screen buffer size
		// to match the window size
		COORD c;
		c.X = col + 1;
		c.Y = row + 1;

		SetConsoleScreenBufferSize(hConsoleOut, c);

		SMALL_RECT rect = SMALL_RECT();
		rect.Top = 0;
		rect.Left = 0;
		rect.Right = col;
		rect.Bottom = row;
		int ret = SetConsoleWindowInfo(hConsoleOut, true, &rect);
	};

	// Set the font used in the console and its size.
	BOOL setFont(const WCHAR* fontFace, int width, int height) {
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);	
		cfi.nFont = 0;
		cfi.dwFontSize.X = width; 
		cfi.dwFontSize.Y = height; 
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		wcscpy_s(cfi.FaceName, fontFace);
		return SetCurrentConsoleFontEx(hConsoleOut, FALSE, &cfi);
	}

public:
	KEY_EVENT_PROC onKeyEvent; 
	MOUSE_EVENT_PROC onMouseEvent; 
	MENU_EVENT_PROC onMenuEvent; 
	FOCUS_EVENT_PROC onFocusEvent;	
	BUFFER_EVENT_PROC onBufferEvent; 
	COLORREF colormap[16];	

	byte textFill;
	byte textOutline;
	byte textBackground;

	byte alphaColor;

	std::vector<Layer> layers;


	GameWindow() : inputRecords{} {
		
		hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
		hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		onKeyEvent = NULL;
		onMouseEvent = NULL;
		onMenuEvent = NULL;
		onFocusEvent = NULL;
		onBufferEvent = NULL;

		CONSOLE_SCREEN_BUFFER_INFOEX cBuffInfo = CONSOLE_SCREEN_BUFFER_INFOEX();
		cBuffInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(hConsoleOut, &cBuffInfo);

		memcpy_s(cmapDefault, sizeof(cmapDefault), cBuffInfo.ColorTable, sizeof(cmapDefault));
		memcpy_s(colormap, sizeof(colormap), cmapDefault, sizeof(colormap));

		ready = false;
		textFill = 0xF0;
		textOutline = 0x70;
		textBackground = 0x00;
		alphaColor = 0x00;
	}

	// Setup (initialize) the game window to the specified settings.
	void setup(int width, int height, int fontWidth, int fontHeight) {	
		// Set console mode to block select user input
		SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ~ENABLE_QUICK_EDIT_MODE);
		setFont(L"Courrier New", fontWidth, fontHeight);

		// Block console window resizing using SetWindowLong from Windows.h
		HWND hWindow = GetConsoleWindow();
		SetWindowLong(hWindow, GWL_STYLE, GetWindowLong(hWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
		
		// Set console to provided size
		setConsoleBufferSize(height, width);

		// Init basic properties and buffers
		this->width = width;
		this->height = height;
		renderBuffer = Layer(width, height, alphaColor, IVec2(0, 0));
		backBuffer = Layer(width, height, alphaColor, IVec2(0, 0));
		layers = std::vector<Layer>{ };
		ready = true;
	}

	void eventTick() {
		DWORD nRecordsRead;
		if (ReadConsoleInput(hConsoleIn, inputRecords, SZ_RECORD_BUFFER, &nRecordsRead)) {	// Go through the records read and dispatch them to the correct event handler
			for (int i = 0; i < nRecordsRead; i++) {
				INPUT_RECORD record = inputRecords[i];
				switch (record.EventType) {
					case KEY_EVENT: 
						if (onKeyEvent != NULL) onKeyEvent(record.Event.KeyEvent); 
						break;
					case MOUSE_EVENT: 
						if (onMouseEvent != NULL) onMouseEvent(record.Event.MouseEvent); 
						break;
					case MENU_EVENT: 
						if (onMenuEvent != NULL) onMenuEvent(record.Event.MenuEvent); 
						break;
					case FOCUS_EVENT: 
						if (onFocusEvent != NULL) onFocusEvent(record.Event.FocusEvent); 
						break;
					case WINDOW_BUFFER_SIZE_EVENT: 
						if (onBufferEvent != NULL) onBufferEvent(record.Event.WindowBufferSizeEvent); 
						break;
				}
			}
		}
	}

	void applyColormap() {
		CONSOLE_SCREEN_BUFFER_INFOEX cBuffInfo = CONSOLE_SCREEN_BUFFER_INFOEX();
		cBuffInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(hConsoleOut, &cBuffInfo);

		memcpy_s(cBuffInfo.ColorTable, sizeof(cBuffInfo.ColorTable), colormap, sizeof(colormap));
		SetConsoleScreenBufferInfoEx(hConsoleOut, &cBuffInfo);
	}

	void resetColormap() {
		memcpy_s(colormap, sizeof(colormap), cmapDefault, sizeof(colormap));
		applyColormap();
	}

	void setColor(int bg, int fg) {
		SetConsoleTextAttribute(hConsoleOut, (bg & 0xf) << 4 | (fg & 0xf));
	}

	void setColor(int col) {
		SetConsoleTextAttribute(hConsoleOut, col & 0xff);
	}

	void setCursorPosition(int row, int col) {
		COORD c;
		c.X = col;
		c.Y = row;
		SetConsoleCursorPosition(hConsoleOut, c);
	}

	void spriteText(const char* text, int layer, IVec2 pos, byte bg, byte fill, byte outline) {
		int i = -1;
		IVec2 cPos = pos;

		while (text[++i]) { 

			if (text[i] == '\n') {
				cPos.y += 8;
				cPos.x = pos.x;
				continue;
			}
			Byte88 sprite = getCharSprite(text[i], bg, fill, outline);
			layers[layer].drawSprite(sprite, cPos, alphaColor);

			cPos.x += 8;
		}
	}

	// Draw text as 8x8 sprites using the pixel font.
	void spriteText(const char* text, int layer, IVec2 pos) {
		spriteText(text, layer, pos, textBackground, textFill, textOutline);
	}

	void invalidate() {
		renderBuffer.setAll(alphaColor);

		for (int i = 0; i < layers.size(); i++) {
			renderBuffer.overlay(layers[i], alphaColor);
		}

		for (int i = 0; i < width * height; i++) {

			if (renderBuffer[i] != backBuffer[i]) {	
				setCursorPosition(i / width, i % width);
				setColor(renderBuffer[i]);
				printf(" ");

				backBuffer[i] = renderBuffer[i];
			}
		}
	}
};