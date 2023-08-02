#pragma once

#include <stdexcept>
#include <Windows.h>
#include "IVec2.h"
#include <functional>

// Simple macro to clamp a value between a min and a max
#define CLAMP(v, mi, ma) min(max(v, mi), ma)

class Layer {

protected:

	byte *buffer;
	int w;	
	int h;	
	int sz; 

public:

	size_t width() { return w; }
	int height() { return h; }
	int size() { return sz; }

	IVec2 pos;

	Layer() : w(0), h(0), sz(0), buffer(NULL) {};

	Layer(Byte88 b, IVec2 pos) : w(8), h(8), sz(64) {
		buffer = (byte*)malloc(64);
		memcpy_s(buffer, sz, b.data, sz);
	}

	Layer(int w, int h) : w(w), h(h), sz(w * h), pos(0, 0) {
		buffer = (byte*)malloc(sz);
		std::fill_n(buffer, sz, 0);
	}

	Layer(int w, int h, byte val) : w(w), h(h), sz(w* h), pos(0, 0) {
		buffer = (byte*)malloc(sz);
		std::fill_n(buffer, sz, val);
	}

	Layer(int w, int h, byte val, IVec2 pos) : w(w), h(h), sz(w * h), pos(pos) {
		buffer = (byte*)malloc(sz);
		std::fill_n(buffer, sz, val);
	}
	
	Layer(const Layer &other) : w(other.w), h(other.h), sz(other.sz), pos(other.pos) {
		buffer = (byte*)malloc(sz);
		memcpy_s(buffer, sz, other.buffer, sz);
	}

	Layer(int w, int h, IVec2 pos, byte* data) : w(w), h(h), sz(w* h), pos(pos) { buffer = data; }

	Layer& operator=(const Layer& buff) {	// Copy pos & size info
		pos = buff.pos;
		w = buff.w;
		h = buff.h;
		sz = buff.sz;

		buffer = (byte*)malloc(sz);
		memcpy_s(buffer, sz, buff.buffer, sz);
		return *this;
	}

	void operator~() {
		free(buffer);
	}

	void resize(int width, int height) {
		byte* oldData = buffer;
		buffer = (byte*)malloc(static_cast<size_t>(width) * height);

		for (int y = 0; y < min(height, h); h++) {
			memcpy_s(buffer + y * width, min(width, w), oldData + y * w, min(width, w));
		}
		free(oldData); 
		w = width;
		h = height;
		sz = width * height;
	}

	void setAll(byte val) {
		std::fill_n(buffer, sz, val);
	}

	// Replace values in buffer
	void replace(byte oldVal, byte newVal) {
		for (int i = 0; i < sz; i++) {
			if (buffer[i] == oldVal) buffer[i] = newVal;
		}
	}

	void replaceData(const Layer& other) {
		if (other.w != w || other.h != h)
			throw std::runtime_error("Cannot overlay with different size buffer");

		memcpy_s(buffer, w, other.buffer, w);
	}

	void drawSprite(const Byte88& sprite, IVec2 layerPos, byte alpha) {

		IVec2 start = IVec2(CLAMP(layerPos.x, 0, w), CLAMP(layerPos.y, 0, h));
		IVec2 end = IVec2(CLAMP(layerPos.x + 8, 0, w), CLAMP(layerPos.y + 8, 0, h));

		for (int x = start.x; x < end.x; x++) {
			for (int y = start.y; y < end.y; y++) {
				IVec2 v = IVec2(x, y);
				if (sprite[v - layerPos] != alpha) buffer[y * w + x] = sprite[v - layerPos];
			}
		}
	}

	void overlay(const Layer &other, byte alpha) {

		IVec2 start = IVec2(CLAMP(other.pos.x, pos.x, pos.x + w), CLAMP(other.pos.y, pos.y, pos.y + h));
		IVec2 end = IVec2(CLAMP(other.pos.x + other.w, pos.x, pos.x + w), CLAMP(other.pos.y + other.h, pos.y, pos.y + h));

		for (int x = start.x; x < end.x; x++) {
			for (int y = start.y; y < end.y; y++) {
				IVec2 v = IVec2(x, y);
				if (other[v - other.pos] != alpha) this->operator[](v - pos) = other[v - other.pos];
			}
		}
	}

	void transform(std::function<byte(byte)> func) {
		for (int i = 0; i < sz; i++) buffer[i] = func(buffer[i]);
		
	}

	void transform(std::function<byte(byte, int)> func) {
		for (int i = 0; i < sz; i++) buffer[i] = func(buffer[i], i);
	}

	void transform(std::function<byte(byte, IVec2)> func) {
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				buffer[y * w + x] = func(buffer[y * w + x], IVec2(x, y));
			}
		}
	}

	// Index operators (safe, throws runtime error to avoid access violation)

	byte& operator[](int i) {
		if (i < 0 || i > sz)
			throw std::runtime_error("Index out of range");

		return buffer[i];
	}

	byte& operator[](IVec2 v) {
		return this->operator[](v.y * w + v.x);
	}

	byte operator[](int i) const {
		return buffer[i];
	}

	byte operator[](IVec2 v) const {
		return this->operator[](v.y * w + v.x);
	}
};