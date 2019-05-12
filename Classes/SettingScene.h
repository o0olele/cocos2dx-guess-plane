#include "cocos2d.h"
#include "ui\CocosGUI.h"

using namespace cocos2d::ui;
USING_NS_CC;

class SettingScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(SettingScene);

	void regButtonCallback(Ref *pSender);

private:
	TextField * IpInput;
	TextField * PortInput;
};
