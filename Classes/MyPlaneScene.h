#pragma once

#include "cocos2d.h"
#include "ui\CocosGUI.h"

using namespace cocos2d::ui;
USING_NS_CC;

class MyPlaneScene : public cocos2d::Scene
{
private:
	Sprite * gridSp;

public:
	float scale;
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(MyPlaneScene);

};
