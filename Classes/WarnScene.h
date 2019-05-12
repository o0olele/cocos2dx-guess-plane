
#include "cocos2d.h"
#include "ui\CocosGUI.h"

using namespace cocos2d::ui;
USING_NS_CC;

class WarnScene : public cocos2d::Scene
{
	Sprite* loadingSp;
	int theta=0;

public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(WarnScene);

	void update(float dt);
	void checkStart(float dt);

	void yesButtonCallback(Ref *pSender);

	void acceptButtonCallback(Ref *pSender);
	void refuseButtonCallback(Ref *pSender);
	void giveButtonCallback(Ref *pSender);
};
