#include "LoginScene.h"
//Other
#include "Global.h"
#include "HomeScene.h"
#include "RegScene.h"
#include "SettingScene.h"
#include "WarnScene.h"
#include "AppDelegate.h"

USING_NS_CC;

Scene* LoginScene::createScene()
{
	auto scene = Scene::create();
	auto layer = LoginScene::create();
	scene->addChild(layer);
	return scene;
}

bool LoginScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Sprite* spBack = Sprite::create("allback.png");
	spBack->setPosition(origin + Vec2(visibleSize) / 2);
	this->addChild(spBack);

	Sprite* usernameBack = Sprite::create("inputbox.png");
	usernameBack->setOpacity(128);
	usernameBack->setPosition(origin+Vec2(visibleSize.width / 2, 0.7f*visibleSize.height));
	this->addChild(usernameBack);

	Sprite* passwordBack = Sprite::create("inputbox.png");
	passwordBack->setOpacity(128);
	passwordBack->setPosition(origin+Vec2(visibleSize.width / 2, 0.6f*visibleSize.height));
	this->addChild(passwordBack);

	usernameInput = TextField::create("ID", "arial", 14);
	if (usernameInput) {
		usernameInput->setPosition(origin+Vec2(visibleSize.width / 2, 0.7f*visibleSize.height));
		this->addChild(usernameInput, 1);
	}

	passwordInput = TextField::create("Key", "arial", 14);
	if (passwordInput) {
		passwordInput->setPosition(origin+Vec2(visibleSize.width / 2, 0.6f*visibleSize.height));
		this->addChild(passwordInput, 1);
	}

	TTFConfig ttfConfig("fonts/showg.ttf", 14);
	messageBox = Label::createWithTTF(ttfConfig, "");
	if (messageBox) {
		messageBox->setTextColor(Color4B::BLACK);
		messageBox->setPosition(origin+Vec2(visibleSize.width / 2, 0.8f*visibleSize.height));
		this->addChild(messageBox, 1);
	}

	auto button = Button::create("button.png", "button.png", "inputbox.png");
	button->setPosition(origin+Vec2(visibleSize.width / 2, visibleSize.height *0.3f));
	button->setTitleText("Login");
	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			loginButtonCallback(this);
			break;
		default:
			break;
		}
	});
	this->addChild(button);

	auto regbutton = Button::create();
	regbutton->setPosition(origin+Vec2(visibleSize.width / 2, visibleSize.height *0.2f));
	regbutton->setTitleText("Reg");
	regbutton->setTitleColor(Color3B::BLACK);
	regbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			Director::getInstance()->replaceScene(TransitionMoveInR::create(0.5f, RegScene::createScene()));
			break;
		default:
			break;
		}
	});
	this->addChild(regbutton);

	auto backbutton = Button::create("reload.png", "reload.png", "reload.png");
	backbutton->setPosition(origin + Vec2(0.94f*visibleSize.width, 0.95f*visibleSize.height));
	backbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		AppDelegate * app = (AppDelegate *)Application::getInstance();
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			app->initConnect();
			break;
		default:
			break;
		}
	});
	this->addChild(backbutton);

	auto setbutton = Button::create("set.png", "set.png", "set.png");
	setbutton->setPosition(origin + Vec2(0.06f*visibleSize.width, 0.95f*visibleSize.height));
	setbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			Director::getInstance()->pushScene(SettingScene::create());
			break;
		default:
			break;
		}
	});
	this->addChild(setbutton);

	_listener = EventListenerCustom::create("user_login_event", [=](EventCustom* event) {
		int* eventSign = static_cast<int*>(event->getUserData());
		switch (*eventSign)
		{
		case 0:messageBox->setString("Wrong Username or Password!"); break;
		case 1:;
		case 4:
			Director::getInstance()->replaceScene(TransitionFade::create(0.5f, WarnScene::createScene())); break;
		default:
			break;
		}

		//CC_SAFE_DELETE(eventSign);
	});

	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_listener, 1);

	this->schedule(schedule_selector(LoginScene::checkHeart), float(HEART_CHECK));

	return true;
}

void LoginScene::loginButtonCallback(Ref *pSender)
{
	if (usernameInput->getString().length() > 8)
	{
		messageBox->setString("Username too long!");
	}
	else if (usernameInput->getString().length() == 0)
	{
		messageBox->setString("Require username!");
	}
	else if (passwordInput->getString().length() > 32)
	{
		messageBox->setString("Password too long!");
	}
	else if (passwordInput->getString().length() == 0)
	{
		messageBox->setString("Require password!");
	}
	else if (passwordInput->getString().length() < 6)
	{
		messageBox->setString("Password too short!");
	}
	else
	{
		messageBox->setString("");

		char head[4] = { 0 };
		char username[8] = { 0 };
		char password[32] = { 0 };
		strcpy(username, usernameInput->getString().c_str());
		strcpy(password, passwordInput->getString().c_str());


		head[0] = 0x11; head[1] = 0x00;
		head[2] = 44; head[3] = 0x00;

		std::string Message;
		Message.append(head, sizeof(head));
		Message.append(username, sizeof(username));
		Message.append(password, 32);

		AppDelegate* app = (AppDelegate *)Application::getInstance();
		app->_client->sendMessage(Message.c_str(), Message.length());
		app->setUsername(usernameInput->getString());

		//Director::getInstance()->replaceScene(TransitionFade::create(0.5f, HomeScene::createScene()));
	}
}

void LoginScene::registerButtonCallback(Ref *pSender)
{

}

void LoginScene::checkHeart(float dt)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	if (!app->IsHeart)
	{
		app->IsHeart = false;
		messageBox->setString("network disconnect!");
	}
	else
		messageBox->setString("");

	app->IsHeart = false;
}
