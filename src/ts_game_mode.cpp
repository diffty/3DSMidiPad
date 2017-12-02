#include "ts_game_mode.hpp"
#include "ts_things_manager.hpp"
#include "ts_things_store.hpp"
#include "ts_draggable_thing.hpp"
#include "ts_workguy_thing.hpp"
#include "ts_critical_thing.hpp"
#include "ts_winning_thing.hpp"
#include "ts_main_character.hpp"
#include "ts_win_character.hpp"
#include "ts_game_manager.hpp"


TSGameMode* TSGameMode::s_pInstance = NULL;


TSGameMode::TSGameMode(Scene* pMainScene, int iNbInitSpawns, float fTimeBetweenInitSpawns) {
	TSGameMode::s_pInstance = this;
	
	m_pFinalMainCharacter = NULL;
	m_fHealth = 100;
	m_iMoney = 0;
	m_pMainScene = pMainScene;
	m_iNbInitSpawns = iNbInitSpawns;
	m_fTimeBetweenInitSpawns = fTimeBetweenInitSpawns;
	m_fTimeBeforeFirstSpawn = 5;
	
	initList(&m_llCharacters);

	m_bWinTimerActivated = false;

	initScene();
}

TSGameMode::~TSGameMode() {
	clearList(&m_llCharacters);

	delete m_pThingsManager;

	m_pMainScene->destroy();
}

void TSGameMode::initScene() {
	RscManager* rscManager = RscManager::get();
	System* sys = System::get();
	
	// Loading CSV Data
	CSVReader objectsData("data/objects.csv");

	// Setting up things
	vect2df_t roomPos;
	roomPos.x = 0.;
	roomPos.y = 0.;

	Sprite* pRoom = new Sprite(rscManager->getImgRsc(2), roomPos);
	pRoom->setDrawOrder(1000);
	m_pMainScene->addComponent(pRoom);

	vect2df_t vStorePos;
	vStorePos.x = 290;
	vStorePos.y = 0;

	size2df_t sStoreSize;
	sStoreSize.w = 30;
	sStoreSize.h = 180;

	m_pThingsStore = new ThingsStore(vStorePos, sStoreSize, 1, 6);
	m_pThingsManager = new ThingsManager(m_pThingsStore);


	vect2df_t testThingPos;
	testThingPos.x = 0;
	testThingPos.y = 0;

	int nbObjects = objectsData.countDataRows();

	vect2df_t vNewThingPos;
	vNewThingPos.x = 0;
	vNewThingPos.y = 0;

	for (int i = 0; i < nbObjects; i++) {
		DraggableThing* newThing = NULL;

		if (strcmp(objectsData.getData("Nice Name", i), "Billet d'Avion") == 0) {
			newThing = (DraggableThing*) new WinningThing(
				rscManager->getSprShtRsc(atoi(objectsData.getData("SprSht", i))),
				atoi(objectsData.getData("FrameNum", i)),
				vNewThingPos);

			newThing->setPrice(atoi(objectsData.getData("Price", i)) * m_iNbInitSpawns);
		}
		else {
			newThing = (DraggableThing*) new DraggableThing(
				rscManager->getSprShtRsc(atoi(objectsData.getData("SprSht", i))),
				atoi(objectsData.getData("FrameNum", i)),
				vNewThingPos);

			newThing->setPrice(atoi(objectsData.getData("Price", i)));
		}

		newThing->setAppealPower(atoi(objectsData.getData("AppealPower", i)));
		newThing->setOccupationTime(atoi(objectsData.getData("OccupationTime", i)));
		newThing->setCooldownTime(atoi(objectsData.getData("CooldownTime", i)));
		newThing->setActionRadius(atoi(objectsData.getData("ActionRadius", i)));
		newThing->setUsableOnce((objectsData.getData("bUsableOnce", i)[0] == 'O') ? true : false);
		newThing->setSingleUser((objectsData.getData("bSingleUser", i)[0] == 'O') ? true : false);
		newThing->setDraggable((objectsData.getData("bIsDraggable", i)[0] == 'O') ? true : false);
		newThing->setIsInStore((objectsData.getData("bInStore", i)[0] == 'O') ? true : false);
		newThing->setUsableOnlyDuringWork((objectsData.getData("bUsableOnlyDuringWork", i)[0] == 'O') ? true : false);
		newThing->setIsWorkThing((objectsData.getData("bIsWorkThing", i)[0] == 'O') ? true : false);
		newThing->setDestroyAfterUse((objectsData.getData("bDestroyAfterUse", i)[0] == 'O') ? true : false);
		//newThing->setTitle(objectsData.getData("Name", i));
		//newThing->setDesc(objectsData.getData("Description", i));
		newThing->setWorkEfficiency(atoi(objectsData.getData("WorkEfficiency", i)));

		newThing->init(objectsData.getData("Name", i), objectsData.getData("Description", i));

		m_pThingsStore->addThingToStore(newThing);
	}

	m_pMainScene->addComponent(m_pThingsStore);

	// m_pThingsStore->addPageBtnComponentsToMainScene(); // lol

	// Tableaux Nord
	testThingPos.x = 45;
	testThingPos.y = 35;
	CriticalThing* ctN1 = new CriticalThing(rscManager->getSprShtRsc(1), 3, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	testThingPos.x = 100;
	CriticalThing* ctN2 = new CriticalThing(rscManager->getSprShtRsc(1), 3, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	testThingPos.x = 165;
	CriticalThing* ctN3 = new CriticalThing(rscManager->getSprShtRsc(1), 3, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	testThingPos.x = 220;
	CriticalThing* ctN4 = new CriticalThing(rscManager->getSprShtRsc(1), 3, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	// Tableaux Sud
	testThingPos.x = 225;
	testThingPos.y = 170;
	CriticalThing* ctS1 = new CriticalThing(rscManager->getSprShtRsc(1), 5, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	testThingPos.x = 185;
	CriticalThing* ctS2 = new CriticalThing(rscManager->getSprShtRsc(1), 5, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	testThingPos.x = 100;
	CriticalThing* ctS3 = new CriticalThing(rscManager->getSprShtRsc(1), 5, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	testThingPos.x = 50;
	CriticalThing* ctS4 = new CriticalThing(rscManager->getSprShtRsc(1), 5, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	// Tableaux Ouest
	testThingPos.x = 35;
	testThingPos.y = 55;
	CriticalThing* ctO1 = new CriticalThing(rscManager->getSprShtRsc(1), 6, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	testThingPos.y = 110;
	CriticalThing* ctO2 = new CriticalThing(rscManager->getSprShtRsc(1), 6, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	testThingPos.y = 155;
	CriticalThing* ctO3 = new CriticalThing(rscManager->getSprShtRsc(1), 6, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	// Tableaux Est
	testThingPos.x = 235;
	testThingPos.y = 55;
	CriticalThing* ctE1 = new CriticalThing(rscManager->getSprShtRsc(1), 4, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	testThingPos.y = 100;
	CriticalThing* ctE2 = new CriticalThing(rscManager->getSprShtRsc(1), 4, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	testThingPos.y = 155;
	CriticalThing* ctE3 = new CriticalThing(rscManager->getSprShtRsc(1), 4, testThingPos, m_pThingsManager, sys->getInputSys(), 1, -1, false, true);

	// Mec du travail
	testThingPos.x = 165;
	testThingPos.y = 130;
	WorkguyThing* wg1 = new WorkguyThing(rscManager->getSprShtRsc(3), 0, testThingPos);

	wg1->setAppealPower(1);
	wg1->setOccupationTime(5);
	wg1->setCooldownTime(90);
	wg1->setActionRadius(75);
	wg1->setUsableOnce(false);
	wg1->setSingleUser(true);
	wg1->setDrawOrder(5000 + wg1->getRect()->getPos().y);

	vect2df_t cloneMachinePos;
	cloneMachinePos.x = 132.;
	cloneMachinePos.y = 95.;

	// Clone machine/Spawn
	CloneMachine* cloneMachine = new CloneMachine(rscManager->getSprShtRsc(9), cloneMachinePos, m_iNbInitSpawns, m_fTimeBetweenInitSpawns, m_fTimeBeforeFirstSpawn);

	cloneMachine->setDrawOrder(3000);

	m_pThingsManager->addThing(ctN1);
	m_pThingsManager->addThing(ctN2);
	m_pThingsManager->addThing(ctN3);
	m_pThingsManager->addThing(ctN4);
	m_pThingsManager->addThing(ctS1);
	m_pThingsManager->addThing(ctS2);
	m_pThingsManager->addThing(ctS3);
	m_pThingsManager->addThing(ctS4);
	m_pThingsManager->addThing(ctO1);
	m_pThingsManager->addThing(ctO2);
	m_pThingsManager->addThing(ctO3);
	m_pThingsManager->addThing(ctE1);
	m_pThingsManager->addThing(ctE2);
	m_pThingsManager->addThing(ctE3);
	m_pThingsManager->addThing(wg1);

	/*m_pMainScene->addComponent(ctN1);
	m_pMainScene->addComponent(ctN2);
	m_pMainScene->addComponent(ctN3);
	m_pMainScene->addComponent(ctN4);
	m_pMainScene->addComponent(ctS1);
	m_pMainScene->addComponent(ctS2);
	m_pMainScene->addComponent(ctS3);
	m_pMainScene->addComponent(ctS4);
	m_pMainScene->addComponent(ctO1);
	m_pMainScene->addComponent(ctO2);
	m_pMainScene->addComponent(ctO3);
	m_pMainScene->addComponent(ctE1);
	m_pMainScene->addComponent(ctE2);
	m_pMainScene->addComponent(ctE3);*/

	testThingPos.x = 20;
	testThingPos.y = 226;
	m_pMoneyLabel = new Text(0, RscManager::get()->getFontRsc(4), testThingPos);

	testThingPos.x = 270;
	m_pHealthLabel = new Text(100, RscManager::get()->getFontRsc(4), testThingPos);

	testThingPos.x = 130;
	m_pTimerLabel = new Text(0, RscManager::get()->getFontRsc(4), testThingPos);
	m_pTimerLabel->setActive(false);

	testThingPos.x = 5;
	testThingPos.y = 227;
	Sprite* pMoneySprite = new Sprite(rscManager->getSprShtRsc(15), 0, testThingPos);

	testThingPos.x = 115;
	m_pTimerSprite = new Sprite(rscManager->getSprShtRsc(15), 1, testThingPos);
	m_pTimerSprite->setActive(false);

	testThingPos.x = 249;
	testThingPos.y = 220;
	m_pArtistSpr = new AnimatedSprite(rscManager->getSprShtRsc(16), testThingPos);

	m_pMainScene->addComponent(wg1);
	m_pMainScene->addComponent(cloneMachine);

	m_pMainScene->addComponent(m_pMoneyLabel);
	m_pMainScene->addComponent(m_pHealthLabel);
	m_pMainScene->addComponent(m_pTimerLabel);
	m_pMainScene->addComponent(pMoneySprite);
	m_pMainScene->addComponent(m_pTimerSprite);
	m_pMainScene->addComponent(m_pArtistSpr);
}

void TSGameMode::spawnMainCharacter(vect2df_t vCharPos) {
	MainCharacter* pNewChar = new MainCharacter(RscManager::get()->getSprShtRsc(0), vCharPos, m_pThingsManager);

	addDataToList(&m_llCharacters, pNewChar);
	//m_pMainScene->addComponent(pNewChar);
	pNewChar->setParentWidget(m_pMainScene);
}

void TSGameMode::spawnWinCharacter(vect2df_t vCharPos) {
	WinCharacter* pNewWinChar = new WinCharacter(RscManager::get()->getSprShtRsc(8), vCharPos);

	addDataToList(&m_llCharacters, pNewWinChar);
	m_pMainScene->addComponent(pNewWinChar);

	m_pWinCharacter = pNewWinChar;
}


float TSGameMode::getHealth() {
	return m_fHealth;
}

void TSGameMode::setHealth(float fHealth) {
	if (fHealth <= 0) fHealth = 0;
	m_fHealth = fHealth;
	char* szNewHealth = Text::intToStr((int) m_fHealth);
	m_pHealthLabel->setText(szNewHealth);
	delete szNewHealth;
	
	onHealthUpdate();
}

void TSGameMode::decreaseHealth(float fHealthMalus) {
	setHealth(m_fHealth - fHealthMalus);
}

void TSGameMode::increaseHealth(float fHealthBonus) {
	setHealth(m_fHealth + fHealthBonus);
}


int TSGameMode::getMoney() {
	return m_iMoney;
}

void TSGameMode::setMoney(int iMoney) {
	m_iMoney = iMoney;
	char* szNewMoney = Text::intToStr(iMoney);
	m_pMoneyLabel->setText(szNewMoney);
	delete szNewMoney;
}

void TSGameMode::decreaseMoney(int iMoneyMalus) {
	setMoney(m_iMoney - iMoneyMalus);
}

void TSGameMode::increaseMoney(int iMoneyBonus) {
	setMoney(m_iMoney + iMoneyBonus);
}


void TSGameMode::onThingMoved() {
	LLNode* pCurrNode = m_llCharacters.pHead;

	while (pCurrNode != NULL) {
		MainCharacter* pCurrCharacter = (MainCharacter*) pCurrNode->pData;
		
		pCurrCharacter->onThingMoved();

		pCurrNode = pCurrNode->pNext;
	}
}

void TSGameMode::onHealthUpdate() {
	if (m_fHealth == 0) {
		TSGameManager::get()->onLevelFail();
	}

	m_pArtistSpr->setFrame((int) 6 - ((m_fHealth / 100.) * 6));
}

void TSGameMode::launchWinTimer() {
	if (!m_bWinTimerActivated) {
		m_fWinTimer = 10;
		m_bWinTimerActivated = true;
		m_pTimerLabel->setActive(true);
		m_pTimerSprite->setActive(true);
	}

	m_iNbActiveWinThings++;
	printf("%i\n", m_iNbActiveWinThings);
}

void TSGameMode::stopWinTimer() {
	if (--m_iNbActiveWinThings == 0) {
		printf("%i\n", m_iNbActiveWinThings);
		m_bWinTimerActivated = false;
		m_pTimerLabel->setActive(false);
		m_pTimerSprite->setActive(false);
	}
}

void TSGameMode::onWinTimerEnd() {
	vect2df_t charPos;
	charPos.x = 135;
	charPos.y = 185;

	spawnWinCharacter(charPos);

	LLNode* pCurrNode = m_llCharacters.pHead;

	int i = 0;

	while (pCurrNode != NULL) {
		AnimatedSprite* pCurrChar = (AnimatedSprite*)pCurrNode->pData;

		if (pCurrChar->getClassType()->getClassTypeName() == "MainCharacter") {
			if (i == 0) {
				((MainCharacter*) pCurrChar)->onWinWalk();
				m_pFinalMainCharacter = ((MainCharacter*)pCurrChar);
			}
			else {
				((MainCharacter*) pCurrChar)->onWinVanish();
			}
		}

		i++;

		pCurrNode = pCurrNode->pNext;
	}
}

MainCharacter* TSGameMode::getFinalMainCharacter() {
	return m_pFinalMainCharacter;
}

WinCharacter* TSGameMode::getWinCharacter() {
	return m_pWinCharacter;
}

WinCharacter* TSGameMode::findWinCharacter() {
	LLNode* pCurrNode = m_llCharacters.pHead;

	int i = 0;

	while (pCurrNode != NULL) {
		AnimatedSprite* pCurrChar = (AnimatedSprite*) pCurrNode->pData;

		if (pCurrChar->getClassType()->getClassTypeName() == "WinCharacter")
			return (WinCharacter*) pCurrChar;

		pCurrNode = pCurrNode->pNext;
	}
}


void TSGameMode::destroyCharacters() {
	LLNode* pCurrNode = m_llCharacters.pHead;

	while (pCurrNode != NULL) {
		AnimatedSprite* pCurrChar = (AnimatedSprite*)pCurrNode->pData;

		m_pMainScene->removeComponent(pCurrChar);

		if (pCurrChar->getClassType()->getClassTypeName() == "WinCharacter") {
			delete ((WinCharacter*) pCurrChar);
		}
		else {
			delete ((MainCharacter*) pCurrChar);
		}

		pCurrNode = pCurrNode->pNext;
	}
}

void TSGameMode::update() {
	m_pThingsManager->processThingsToDeleteList();

	if (System::get()->getInputSys()->IsKeyPressed(KEYB_A)) {
		increaseMoney(10);
	}
	if (System::get()->getInputSys()->IsKeyPressed(KEYB_Z)) {
		decreaseHealth(1);
	}

	if (m_bWinTimerActivated) {
		m_fWinTimer -= System::get()->getDeltaTime();

		char* szNewTime = Text::intToStr(m_fWinTimer);
		m_pTimerLabel->setText(szNewTime);
		delete szNewTime;

		if (m_fWinTimer <= 0) {
			onWinTimerEnd();
			m_bWinTimerActivated = false;
		}
	}

	if (m_pFinalMainCharacter && m_pFinalMainCharacter->getCurrentState() == E_MAINCHAR_STATE_WIN_OUT) {
		TSGameManager::get()->onLevelSuccess();
	}
}

ThingsStore* TSGameMode::getThingsStore() {
	return m_pThingsStore;
}

ThingsManager* TSGameMode::getThingsManager() {
	return m_pThingsManager;
}

Scene* TSGameMode::getScene() {
	return m_pMainScene;
}

TSGameMode* TSGameMode::get() {
	return TSGameMode::s_pInstance;
}

void TSGameMode::initGameMode(TSGameMode** pGameMode, Scene* pScene, int iNbInitSpawns, float fTimeBetweenInitSpawns) {
	if (*pGameMode) delete *pGameMode;
	*pGameMode = new TSGameMode(pScene, iNbInitSpawns, fTimeBetweenInitSpawns);

}
