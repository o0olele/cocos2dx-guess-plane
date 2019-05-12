#pragma once

#include "cocos2d.h"
#include "ui\CocosGUI.h"

using namespace cocos2d::ui;
USING_NS_CC;

typedef enum tagGridChild
{
	FirstChild = 100,
	SecondChild,
	ThirdChild,
	FourthChild
} GridChild;

class DuelScene : public cocos2d::Scene
{
private:
	Sprite * gridSp;
	Sprite * turnSp;

	Button * clearbutton;
	Button * doublebutton;
	Button * bombbutton;

	int MaxMarkNum = 2;
	int MarkCnt = 0;

	int i1, j1;
	int i2, j2;

	int guess[10][10] = { 0 };
	int TurnCnt = 0;

public:
	EventListenerTouchOneByOne * listener;
	Label* ScoreLb;
	float scale;

	Texture2D * nulltexture;
	Texture2D * ptexture;
	Texture2D * phtexture;
	Texture2D * questexture;


	static cocos2d::Scene* createScene();

	virtual void onEnter() override;
	virtual void onExit() override;
	virtual bool init();

	CREATE_FUNC(DuelScene);

	void clearButtonCallback(Ref *pSender);
	void doubleButtonCallback(Ref *pSender);
	void bombButtonCallback(Ref *pSender);
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

	void DuelEventCallback(Ref *pSender);

	void SingleResult(std::string);
	void DoubleDead();

	void checkResult(float dt);
	void checkHeart(float dt);
};
