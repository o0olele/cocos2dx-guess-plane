#include "SettingScene.h"
#include "AppDelegate.h"
#include "LoginScene.h"
#include "Global.h"

USING_NS_CC;

Scene* SettingScene::createScene()
{
	auto scene = Scene::create();
	auto layer = SettingScene::create();
	scene->addChild(layer);
	return scene;
}

bool SettingScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Texture2D * texture = Director::getInstance()->getTextureCache()->addImage("allback.png");
	Sprite* spBack = Sprite::createWithTexture(texture);
	spBack->setPosition(origin + Vec2(visibleSize) / 2);
	this->addChild(spBack);

	Sprite* usernameBack = Sprite::create("inputbox.png");
	usernameBack->setOpacity(128);
	usernameBack->setPosition(origin + Vec2(visibleSize.width / 2, 0.7f*visibleSize.height));
	this->addChild(usernameBack);

	Sprite* passwordBack = Sprite::create("inputbox.png");
	passwordBack->setOpacity(128);
	passwordBack->setPosition(origin + Vec2(visibleSize.width / 2, 0.6f*visibleSize.height));
	this->addChild(passwordBack);

	IpInput = TextField::create("ip", "arial", 14);
	if (IpInput) {
		IpInput->setPosition(origin + Vec2(visibleSize.width / 2, 0.7f*visibleSize.height));
		this->addChild(IpInput, 1);
	}

	PortInput = TextField::create("port", "arial", 14);
	if (PortInput) {
		PortInput->setPosition(origin + Vec2(visibleSize.width / 2, 0.6f*visibleSize.height));
		this->addChild(PortInput, 1);
	}

	auto backbutton = Button::create("back.png", "back.png", "back.png");
	backbutton->setPosition(origin + Vec2(0.06f*visibleSize.width, 0.95f*visibleSize.height));
	backbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			Director::getInstance()->popScene();
			break;
		default:
			break;
		}
	});
	this->addChild(backbutton);

	auto button = Button::create("inputbox.png", "inputbox.png", "inputbox.png");
	button->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height *0.2));
	button->setTitleText("save");
	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			regButtonCallback(this);
			break;
		default:
			break;
		}
	});
	this->addChild(button);

	return true;
}

void SettingScene::regButtonCallback(Ref *pSender)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	std::string p = PortInput->getString();

	for (int i = 0; i < p.length(); i++)
		if (!(p[i] >= '0'&&p[i] <= '9'))
			return;

	int port = std::stoi(PortInput->getString());
	app->setPort(port);
	if (IpInput->getString().length() > 0)
		app->setIp(IpInput->getString());

	IpInput->setText("");
	PortInput->setText("");

}