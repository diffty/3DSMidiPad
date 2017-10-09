#include "ts_main_character.hpp"


MainCharacter::MainCharacter(SpriteSheet* pSprSh, vect2df_t vPos, ThingsManager* pThingsManager)
	: AnimatedSprite(pSprSh, vPos)
{
	// Creating FSM
	m_pEventTimer = new eventTimer_t;
	m_pDistToFocusedThing = new float(0);

	m_pEventTimer->currTime = 0;
	m_pEventTimer->limit = 5;

	// Init progress bar
	vect2df_t vProgressBarPos;
	vProgressBarPos.x = m_rect.getPos().x;
	vProgressBarPos.y = m_rect.getPos().y - 10;

	size2df_t sProgressBarSize;
	sProgressBarSize.w = 20;
	sProgressBarSize.h = 3;

	m_pProgressBar = new TinyProgressBar(vProgressBarPos, sProgressBarSize);
	m_pProgressBar->setActive(false);

	// Init FSM
	FSMNode* fsmNodeIdle = new FSMNode(E_MAINCHAR_STATE_IDLE, "idle");
	FSMNode* fsmNodeSeeking = new FSMNode(E_MAINCHAR_STATE_SEEKING, "seeking");
	FSMNode* fsmNodeAppealed = new FSMNode(E_MAINCHAR_STATE_APPEALED, "appealed");
	FSMNode* fsmNodeWalking = new FSMNode(E_MAINCHAR_STATE_WALKING, "walking");
	FSMNode* fsmNodeArrived = new FSMNode(E_MAINCHAR_STATE_ARRIVED, "arrived");
	FSMNode* fsmNodeOccupiedObj = new FSMNode(E_MAINCHAR_STATE_OCCUPIED_OBJ, "occupied_object");
	FSMNode* fsmNodeOccupiedCrit = new FSMNode(E_MAINCHAR_STATE_OCCUPIED_CRIT, "occupied_critical");

	fsmNodeIdle->addEvent("bored", fsmNodeSeeking, &boredEventFunc, &m_pEventTimer);
	fsmNodeSeeking->addEvent("found", fsmNodeAppealed, &foundThingEventFunc, &m_pCurrFocusedThing);
	fsmNodeAppealed->addEvent("start_walking", fsmNodeWalking, &startWalkingEventFunc, &m_pEventTimer);
	fsmNodeWalking->addEvent("arrived_on_thing", fsmNodeArrived, &startBusyEventFunc, &m_pDistToFocusedThing);
	fsmNodeWalking->addEvent("start_walking_to_another", fsmNodeAppealed, &seenAnotherThingEventFunc, &m_pEventTimer);
	fsmNodeArrived->addEvent("thing_is_an_object", fsmNodeOccupiedObj, &determineThingIsObjFuncEvent, &m_pCurrFocusedThing);
	fsmNodeArrived->addEvent("thing_is_a_critical_object", fsmNodeOccupiedCrit, &determineThingIsCritFuncEvent, &m_pCurrFocusedThing);
	fsmNodeOccupiedObj->addEvent("end_busy", fsmNodeIdle, &endBusyEventFunc, &m_pEventTimer);
	fsmNodeOccupiedCrit->addEvent("end_crit", fsmNodeAppealed, &endAttackEventFunc, &m_pNewFocusedThing);

	m_fsm.addNode(fsmNodeIdle);
	m_fsm.addNode(fsmNodeSeeking);
	m_fsm.addNode(fsmNodeAppealed);
	m_fsm.addNode(fsmNodeWalking);
	m_fsm.addNode(fsmNodeOccupiedObj);
	m_fsm.addNode(fsmNodeOccupiedCrit);

	m_pThingsManager = pThingsManager;

	m_pCurrFocusedThing = NULL;

	m_fWalkSpeed = 20.;

	initList(&m_llThingsHistory);
    
    addState("walk_n", 0, 7, 10, true);
    addState("walk_ne", 8, 15, 10, true);
    addState("walk_e", 16, 23, 10, true);
    addState("walk_se", 24, 31, 10, true);
    addState("walk_s", 32, 39, 10, true);
    addState("walk_so", 40, 47, 10, true);
    addState("walk_o", 48, 55, 10, true);
    addState("walk_no", 56, 63, 10, true);
    addState("walk_amazed1_e", 64, 71, 10, true);
    addState("walk_amazed2_e", 72, 79, 10, true);
    addState("walk_amazed1_se", 80, 87, 10, true);
    addState("walk_amazed2_se", 88, 95, 10, true);
    addState("walk_amazed1_s", 96, 103, 10, true);
    addState("walk_amazed2_s", 104, 111, 10, true);
    addState("occupied", 112, 115, 5, true);
    addState("idle_n", 116, 116, 5, true);
    addState("idle_no", 117, 117, 1, true);
    addState("idle_o", 118, 121, 4, true);
    addState("idle_s", 122, 126, 4, true);
    addState("idle_so", 127, 130, 4, true);
}

MainCharacter::~MainCharacter() {

}

void MainCharacter::onNewState(FSM_MAINCHAR_STATE currState) {
	switch (currState) {
	case E_MAINCHAR_STATE_IDLE:
		m_pEventTimer->currTime = 0;
		m_pEventTimer->limit = 5;
		m_pCurrFocusedThing = NULL;
		break;

	case E_MAINCHAR_STATE_SEEKING:
		m_pCurrFocusedThing = NULL;
		break;

	case E_MAINCHAR_STATE_OCCUPIED_OBJ:
	{
        changeState(14);
        
		m_pEventTimer->currTime = 0;
		m_pEventTimer->limit = m_pCurrFocusedThing->getOccupationTime();

		historyThingInfo_t* pHistoryThingInfo = new historyThingInfo_t; // NE PAS OUBLIER DE LE DELETE
		pHistoryThingInfo->pThingObj = m_pCurrFocusedThing;
		pHistoryThingInfo->fCooldownTimeLeft = m_pCurrFocusedThing->getCooldownTime();

		addDataToList(&m_llThingsHistory, pHistoryThingInfo);

		onBeginUsing();
		m_pCurrFocusedThing->onBeginUsing();
		break;
	}

	case E_MAINCHAR_STATE_OCCUPIED_CRIT:
		m_pNewFocusedThing = NULL;
		m_pEventTimer->currTime = 0;
		m_pEventTimer->limit = 1;
		onBeginAttack();
		break;
	}
}

void MainCharacter::onEndState(FSM_MAINCHAR_STATE currState) {
	switch (currState) {
	case E_MAINCHAR_STATE_OCCUPIED_OBJ:
		onEndUsing();
		m_pCurrFocusedThing->onEndUsing();
		break;

	case E_MAINCHAR_STATE_OCCUPIED_CRIT:
		onEndAttack();
		m_pCurrFocusedThing = m_pNewFocusedThing;
		break;
	}
}

void MainCharacter::draw(uint8* fb) {
	AnimatedSprite::draw(fb);
	if (m_pProgressBar->isActive())
		m_pProgressBar->draw(fb);
}

void MainCharacter::update() {
    AnimatedSprite::update();
    
	double deltaTime = System::get()->getDeltaTime();

	m_fsm.update();

	FSM_MAINCHAR_STATE currState = (FSM_MAINCHAR_STATE) m_fsm.getActiveState()->getStateId();

	if (m_prevState != currState) {
		if (m_prevState != NULL)
			onEndState(m_prevState);

		onNewState(currState);

		m_prevState = currState;
		printf("Current state: %s\n", m_fsm.getActiveState()->getName());
	}

	switch (currState) {
		case E_MAINCHAR_STATE_SEEKING:
		{
			m_pCurrFocusedThing = searchForAvailableThings();
			break;
		}

		case E_MAINCHAR_STATE_WALKING:
		{
			vect2df_t vCurrFocusedThingPos = m_pCurrFocusedThing->getRect()->getPos();
			vect2df_t vCurrPos = m_rect.getPos();

			vect2df_t vMoveVector;
			vect2df_t vNormalizedVect;

			vMoveVector.x = vCurrPos.x - vCurrFocusedThingPos.x;
			vMoveVector.y = vCurrPos.y - vCurrFocusedThingPos.y;

			if (abs(vMoveVector.x) < abs(vMoveVector.y)) {
				vNormalizedVect.x = vMoveVector.x / abs(vMoveVector.y);
				vNormalizedVect.y = vMoveVector.y / abs(vMoveVector.y);
			}
			else if (abs(vMoveVector.x) > abs(vMoveVector.y)) {
				vNormalizedVect.x = vMoveVector.x / abs(vMoveVector.x);
				vNormalizedVect.y = vMoveVector.y / abs(vMoveVector.x);
			}
			else {
				vNormalizedVect.x = vMoveVector.x / abs(vMoveVector.x);
				vNormalizedVect.y = vMoveVector.y / abs(vMoveVector.y);
			}

			//printf("(%f, %f) ", vNormalizedVect.x, vNormalizedVect.y);

			*m_pDistToFocusedThing = (float) sqrt((vMoveVector.x * vMoveVector.x) + (vMoveVector.y * vMoveVector.y));

			translate((-vNormalizedVect.x * deltaTime * m_fWalkSpeed), (-vNormalizedVect.y * deltaTime * m_fWalkSpeed), TRANSFORM_REL);

			break;
		}

		case E_MAINCHAR_STATE_OCCUPIED_OBJ:
			onUsing(m_pEventTimer->currTime);
			m_pCurrFocusedThing->onUsing();
			break;

		case E_MAINCHAR_STATE_OCCUPIED_CRIT:
		{
			DraggableThing* newFocusedThing = searchForAvailableThings();
			if (newFocusedThing->getClassType()->getClassTypeName() != "CriticalThing") {
				m_pNewFocusedThing = newFocusedThing;
			}
			onAttacking();
			break;
		}
	}

	// Updating things cooldown countdown
	LLNode* currHistoryThingsNode = m_llThingsHistory.pHead;
	LLNode* nextHistoryThingsNode = NULL;

	while (currHistoryThingsNode != NULL) {
		historyThingInfo_t* currHistoryNodeInfo = (historyThingInfo_t*)currHistoryThingsNode->pData;

		nextHistoryThingsNode = currHistoryThingsNode->pNext;

		if (currHistoryNodeInfo->fCooldownTimeLeft != -1
			&& currHistoryNodeInfo->pThingObj != m_pCurrFocusedThing) {

			currHistoryNodeInfo->fCooldownTimeLeft -= System::get()->getDeltaTime();

			if (currHistoryNodeInfo->fCooldownTimeLeft < 0.) {
				removeNodeFromList(&m_llThingsHistory, currHistoryThingsNode);

				delete currHistoryNodeInfo;
				delete currHistoryThingsNode;
			}
		}
		currHistoryThingsNode = nextHistoryThingsNode;
	}
    
    /*if (System::get()->getInputSys()->IsKeyPressed(KEYB_Z)) {
        changeState(0);
    }
    else if (System::get()->getInputSys()->IsKeyPressed(KEYB_D)) {
        changeState(2);
    }
    else if (System::get()->getInputSys()->IsKeyPressed(KEYB_S)) {
        changeState(4);
    }
    else if (System::get()->getInputSys()->IsKeyPressed(KEYB_Q)) {
        changeState(6);
    }*/
}

void MainCharacter::translate(float x, float y, ETransformMode transformMode) {
    vect2df_t vCurPos = getRect()->getPos();
    
    vect2df_t vDeltaPos;
    
    if (transformMode == TRANSFORM_REL) {
        vDeltaPos.x = x;
        vDeltaPos.y = y;
    }
    else {
        vDeltaPos.x = x - vCurPos.x;
        vDeltaPos.y = y - vCurPos.y;
    }
    
    //printf("%f, %f\n", vDeltaPos.x, vDeltaPos.y);
    
    if (vDeltaPos.x > 0.) {
        if (vDeltaPos.y > 0.) {
            changeState(3);
        }
        else if (vDeltaPos.y == 0.) {
            changeState(2);
        }
        else {
            changeState(1);
        }
    }
    else if (vDeltaPos.x == 0.) {
        if (vDeltaPos.y > 0.) {
            changeState(4);
        }
        else if (vDeltaPos.y == 0.) {
            
        }
        else {
            changeState(0);
        }
    }
    else if (vDeltaPos.x < 0.) {
        if (vDeltaPos.y > 0.) {
            changeState(5);
        }
        else if (vDeltaPos.y == 0.) {
            changeState(6);
        }
        else {
            changeState(7);
        }
    }

    Sprite::translate(x, y, transformMode);
}

// Event funcs
void MainCharacter::onBeginUsing() {
	m_pProgressBar->setActive(true);
	m_pProgressBar->setValue(0.);
}

void MainCharacter::onEndUsing() {
	m_pProgressBar->setActive(false);
}

void MainCharacter::onUsing(float fUsageTimeLeft) {
	m_pProgressBar->getRect()->setPos(m_rect.getPos().x, m_rect.getPos().y - 10);
	m_pProgressBar->setValue(fUsageTimeLeft / (float) m_pCurrFocusedThing->getOccupationTime());
}

void MainCharacter::onBeginAttack() {

}

void MainCharacter::onEndAttack() {

}

void MainCharacter::onAttacking() {
	if (m_pEventTimer->currTime > m_pEventTimer->limit) {
		m_pEventTimer->currTime = 0;
		onSlay();
	}
	else {
		m_pEventTimer->currTime += System::get()->getDeltaTime();
	}
}

void MainCharacter::onSlay() {
	printf("SLAYED\n");
}

void MainCharacter::onThingMoved() {
	if (m_pCurrFocusedThing != NULL) {
		FSM_MAINCHAR_STATE currState = (FSM_MAINCHAR_STATE) m_fsm.getActiveState()->getStateId();

		if (currState == E_MAINCHAR_STATE_WALKING) {
			m_pCurrFocusedThing = NULL;
			m_fsm.changeState(E_MAINCHAR_STATE_SEEKING);
		}
	}
}

DraggableThing* MainCharacter::searchForAvailableThings() {
	LinkedList llSortedAvailableThings;
	initList(&llSortedAvailableThings);

	m_pThingsManager->getClosestAvailableThingsToPoint(&llSortedAvailableThings, m_rect.getPosi());

	DraggableThing* pFoundThing = NULL;

	LLNode* currAvailThingsNode = llSortedAvailableThings.pHead;

	//printf("llSortedAvailableThings: %i\n", (int) llSortedAvailableThings.size);

	while (currAvailThingsNode != NULL) {
		DraggableThing* pCurrThingCandidate = (DraggableThing*)currAvailThingsNode->pData;
		//printf("Current candidate thing : %p\n", pCurrThingCandidate);

		if (pCurrThingCandidate != m_pCurrFocusedThing) {
			LLNode* currHistoryThingsNode = m_llThingsHistory.pHead;

			bool bThingFoundInHistory = false;

			while (currHistoryThingsNode != NULL) {
				historyThingInfo_t* currHistoryNodeInfo = (historyThingInfo_t*)currHistoryThingsNode->pData;

				//printf("%p : %p\n", currAvailThingsNode->pData, currHistoryNodeInfo->pThingObj);

				if (pCurrThingCandidate == currHistoryNodeInfo->pThingObj) {
					bThingFoundInHistory = true;
					break;
				}
				currHistoryThingsNode = currHistoryThingsNode->pNext;
			}

			if (!bThingFoundInHistory) {
				pFoundThing = pCurrThingCandidate;
				//printf("Thing selected: %p\n", pCurrThingCandidate);
				break;
			}
		}

		currAvailThingsNode = currAvailThingsNode->pNext;
	}

	destroyList(&llSortedAvailableThings);

	return pFoundThing;
}

// Static FSM events funcs
bool MainCharacter::boredEventFunc(void* arg) {
	eventTimer_t* pTimerArg = *((eventTimer_t**) arg);

	if (pTimerArg->currTime > pTimerArg->limit) {
		pTimerArg->currTime = 0;
		return true;
	}

	pTimerArg->currTime += System::get()->getDeltaTime();

	return false;
}

bool MainCharacter::foundThingEventFunc(void* arg) {
	DraggableThing* pCurrThing = *((DraggableThing**) arg);

	if (pCurrThing) {
		return true;
	}

	return false;
}

bool MainCharacter::startWalkingEventFunc(void* arg) {
	return true;
}

bool MainCharacter::startBusyEventFunc(void* arg) {
	float* fDistToFocusedThing = (float*) *((float**) arg);

	if (*fDistToFocusedThing < 5) {
		return true;
	}

	return false;
}

bool MainCharacter::seenAnotherThingEventFunc(void* arg) {
	return false;
}

bool MainCharacter::endBusyEventFunc(void* arg) {
	eventTimer_t* pTimerArg = *((eventTimer_t**)arg);

	if (pTimerArg->currTime > pTimerArg->limit) {
		pTimerArg->currTime = 0;
		return true;
	}

	pTimerArg->currTime += System::get()->getDeltaTime();

	return false;
}

bool MainCharacter::endAttackEventFunc(void* arg) {
	DraggableThing* pNewThing = *((DraggableThing**) arg);

	if (pNewThing != NULL) {
		return true;
	}

	return false;
}

bool MainCharacter::determineThingIsObjFuncEvent(void* arg) {
	DraggableThing* pThing = *((DraggableThing**)arg);
	if (pThing->getClassType()->getClassTypeName() == "DraggableThing") {
		return true;
	}
	return false;
}

bool MainCharacter::determineThingIsCritFuncEvent(void* arg) {
	DraggableThing* pThing = *((DraggableThing**)arg);
	if (pThing->getClassType()->getClassTypeName() == "CriticalThing") {
		return true;
	}
	return false;
}