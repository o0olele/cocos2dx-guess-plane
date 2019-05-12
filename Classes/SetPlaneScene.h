#pragma once

#include "cocos2d.h"
#include "ui\CocosGUI.h"
#include "PlaneSprite.h"

using namespace cocos2d::ui;
USING_NS_CC;

class SetPlaneScene : public cocos2d::Scene
{
public:
	PlaneSprite * PlaneSp1;
	PlaneSprite * PlaneSp2;
	PlaneSprite * PlaneSp3;

	Button * rotate1;
	Button * rotate2;
	Button * rotate3;

	Label * messageBox;

    Vec2 origin;
	EventListenerCustom * _listener;
	float scale;
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(SetPlaneScene);

	void confirmButtonCallback(Ref *pSender);
	void checkHeart(float dt);

	void rotate1ButtonCallback(Ref *pSender);
	void rotate2ButtonCallback(Ref *pSender);
	void rotate3ButtonCallback(Ref *pSender);

	void EnableMove(float dt);

};
