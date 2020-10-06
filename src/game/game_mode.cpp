#include "game_mode.hpp"
#include "../manager/game_manager.hpp"
#include "../manager/rsc_manager.hpp"

#include "../graphics/spritesheet.hpp"
#include "../graphics/animatedsprite.hpp"
#include "../core/graphics.hpp"

#include <cmath>

GameMode* GameMode::s_pInstance = NULL;


GameMode::GameMode(Scene* pMainScene) {
	GameMode::s_pInstance = this;
    
	m_pMainScene = pMainScene;

    m_pTrain = new CTTrain();
    
    m_pMainScene->addChildWidget(m_pTrain);

    m_pTrain->addWagon();
    m_pTrain->addTransition();
    m_pTrain->addWagon();
    m_pTrain->addTransition();
    m_pTrain->addWagon();
    m_pTrain->addTransition();
    
    //m_pTrain->translate(fb.width / 2 - train.getRect()->getSize().w / 2, 0, TRANSFORM_ABS);

    initList(&m_llWidgetTrash);
    initScene();
}

GameMode::~GameMode() {
	m_pMainScene->garbageCollect();
	m_pMainScene->destroy();
}

void GameMode::initScene() {

}

void GameMode::update() {
    double fDeltaTime = System::get()->getDeltaTime();

    // ** Game logic code here **
    destroyWidgetsInTrash();
}

void GameMode::debugDraw(drawbuffer* pBuffer) {
    
}

Scene* GameMode::getScene() {
	return m_pMainScene;
}

void GameMode::destroyWidget(IWidget* pWidget) {
    addDataToList(&m_llWidgetTrash, pWidget);
}

void GameMode::destroyWidgetsInTrash() {
	LLNode* pCurrNode = m_llWidgetTrash.pHead;

	while (pCurrNode != NULL) {
		IWidget* pCurrWidget = (IWidget*)pCurrNode->pData;
		delete pCurrWidget;
		pCurrNode = pCurrNode->pNext;
	}

	clearList(&m_llWidgetTrash);
}

GameMode* GameMode::get() {
	return GameMode::s_pInstance;
}

void GameMode::initGameMode(GameMode** pGameMode, Scene* pScene) {
	if (*pGameMode) {
		delete *pGameMode;
	}

	*pGameMode = new GameMode(pScene);
}
