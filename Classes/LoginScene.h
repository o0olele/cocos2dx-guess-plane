#pragma once
#include "cocos2d.h"
#include "ui\CocosGUI.h"

using namespace cocos2d::ui;
USING_NS_CC;

class LoginScene : public cocos2d::Scene 
{
public:
	EventListenerCustom * _listener;
	Label * messageBox;

	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(LoginScene);

	void loginButtonCallback(Ref *pSender);
	void registerButtonCallback(Ref *pSender);

	void checkHeart(float dt);

private:
	TextField * usernameInput;
	TextField * passwordInput;
};
