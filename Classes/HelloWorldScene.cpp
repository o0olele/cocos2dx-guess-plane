#include "HelloWorldScene.h"
#include "LoginScene.h"
#include "AppDelegate.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
	auto scene = Scene::create();
	auto layer = HelloWorld::create();
	scene->addChild(layer);
    return scene;
}

bool HelloWorld::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

	Sprite* sp = Sprite::create("backp.png");
	sp->setPosition(origin + Vec2(visibleSize) / 2);// -Vec2(sp->getContentSize() / 2));

	TTFConfig ttfConfig("fonts/showg.ttf", 24);
	auto messageBox = Label::createWithTTF(ttfConfig, "Hit Plane");
	if (messageBox) {
		messageBox->setTextColor(Color4B::BLACK);
		messageBox->setPosition(origin + Vec2(visibleSize.width / 2, 0.5f*visibleSize.height));
		this->addChild(messageBox, 1);
	}

	auto scale = (float)visibleSize.width / (float)sp->getContentSize().width;
	sp->setScale(scale, scale);

	this->addChild(sp);
	this->scheduleOnce(schedule_selector(HelloWorld::EnterLoginScene), 1.0f);

	return true;
}

void HelloWorld::EnterLoginScene(float dt)
{
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, LoginScene::createScene()));
}
