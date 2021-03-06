//
//  spritesheet.cpp
//  Muggine
//
//  Created by DiFFtY on 02/09/2017.
//  Copyright © 2017 RIX. All rights reserved.
//

#include "spritesheet.hpp"


SpriteSheet::SpriteSheet(char* fileName, int gridWidth, int gridHeight, uint length)
	: Image(fileName) {

	m_gridSize.w = gridWidth;
	m_gridSize.h = gridHeight;

	m_frameSize.w = (m_size.w / gridWidth);
	m_frameSize.h = (m_size.h / gridHeight);

	m_length = length;

	m_currFrame = 0;
}

void SpriteSheet::draw(uint8* buffer, int dstX, int dstY, bool reversed, bool masked) {
    draw(buffer, m_currFrame, dstX, dstY, reversed, masked);
}

void SpriteSheet::draw(uint8* buffer, uint frame, int dstX, int dstY, bool reversed, bool masked) {
    //printf("%i: %i\n", frame, (m_frameSize.w * (frame % m_gridSize.w)));
    Image::draw(buffer, dstX, dstY, (m_frameSize.w * (frame % m_gridSize.w)), (m_frameSize.h * (m_gridSize.h - 1 - (frame / m_gridSize.w))), m_frameSize.w, m_frameSize.h, reversed, masked);
}

size2d_t SpriteSheet::getFrameSize() {
	return m_frameSize;
}

uint SpriteSheet::getFrame() {
	return m_currFrame;
}

void SpriteSheet::setFrame(uint newFrame) {
	m_currFrame = newFrame;
}

void SpriteSheet::nextFrame() {
    m_currFrame = (m_currFrame + 1) % m_length;
}

void SpriteSheet::prevFrame() {
    int newFrame = (m_currFrame - 1);
    
    if (newFrame < 0) {
        newFrame = m_length - 1;
    }
    m_currFrame = newFrame;
}
