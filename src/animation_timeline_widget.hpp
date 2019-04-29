#ifndef ANIMATION_TIMELINE_WIDGET_HPP
#define ANIMATION_TIMELINE_WIDGET_HPP

#ifdef TARGET_3DS
#include <3ds.h>
#endif

#include "common_types.hpp"
#include "drawing.hpp"
#include "iwidget.hpp"
#include "animationtimeline.hpp"
#include "mathf.hpp"


class AnimationTimelineWidget : public IWidget {
private:
	AnimationTimeline* m_pDrivenTimeline = NULL;
	bool m_bPressedThisLoop = false;
	bool m_bIsPressed = false;
	float m_fMinTime = 0.0;
	float m_fMaxTime = 1.0;
	int m_iState = 0;

public:
	AnimationTimelineWidget(int x, int y, int w, int h, AnimationTimeline* pDrivenTimeline, float fMinTime, float fMaxTime);

	bool receiveTouchInput(vect2d_t touchPt);
	void onPress();
	void onRelease();

	void update();
	void draw(uint8* buffer);
};


#endif