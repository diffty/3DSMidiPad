#include "bko_grid.hpp"

Grid::~Grid() {
	printf("Deleting Grid\n");

	for (int i = 0; i < m_nbBricksW * m_nbBricksH; i++) {
		delete m_brickList[i];
	}

	delete[] m_brickList;

	printf("End Grid deletion\n");
}

void Grid::init() {
	/*initList(&m_brickList);

	for (int i = 0; i < GRID_W * GRID_H; i++) {
		LLNode* newNode = new LLNode;
		newNode->pData = (void *) new Brick(0, m_pRscManager);
		addNodeToList(&m_brickList, newNode);
	}*/

	size2d_t spriteSize = m_pRscManager->getImgRsc(0)->getSizei();

	m_brickList = new Brick*[m_nbBricksW * m_nbBricksH];
	
	for (int i = 0; i < m_nbBricksW * m_nbBricksH; i++) {
		m_brickList[i] = new Brick(0, m_pRscManager);
		m_brickList[i]->translate(
			getRect()->getPos().x + (spriteSize.w - 1) * (i % m_nbBricksW),
			getRect()->getPos().y + (spriteSize.h - 1) * (i / m_nbBricksW),
			TRANSFORM_ABS);
	}
}

void Grid::update() {
	updateChildren();
}

void Grid::draw(uint8* buffer) {
	if (m_bIsActive) {
		drawChildren(buffer);

		for (int i = 0; i < m_nbBricksW * m_nbBricksH; i++) {
			m_brickList[i]->draw(buffer);
		}
	}
}

bool Grid::checkBrickAtPos(vect2df_t pos, uint* collidingBrickId) {
    for (int i = 0; i < m_nbBricks; i++) {
        Rectf* currBrickRect = m_brickList[i]->getRect();
        
        if (m_brickList[i]->isActive() && currBrickRect->isPointInRect(pos.x, pos.y)) {
            *collidingBrickId = i;

            return true;
        }
    }
    return false;
}

Brick* Grid::getBrickFromId(uint id) {
    return m_brickList[id];
}

