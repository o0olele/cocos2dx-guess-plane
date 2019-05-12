#include "cocos2d.h"
#include "ui\CocosGUI.h"

using namespace cocos2d::ui;
USING_NS_CC;

class RegScene : public cocos2d::Scene
{
public:
	EventListenerCustom * _listener;
	Label * messageBox;
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(RegScene);

	void regButtonCallback(Ref *pSender);

	void EnterLoginScene(float);
	void checkHeart(float dt);

private:
	TextField * usernameInput;
	TextField * passwordInput;
};
