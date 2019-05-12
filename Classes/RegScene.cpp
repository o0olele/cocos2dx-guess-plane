#include "RegScene.h"
#include "AppDelegate.h"
#include "LoginScene.h"
#include "Global.h"

USING_NS_CC;

Scene* RegScene::createScene()
{
	auto scene = Scene::create();
	auto layer = RegScene::create();
	scene->addChild(layer);
	return scene;
}

bool RegScene::init()
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

    messageBox = Label::create("", "arial", 14);
    if (messageBox) {
    	float x = visibleSize.width / 2;
    	float y = 0.8f*visibleSize.height;
    	messageBox->setPosition(origin+Vec2(x, y));
    	messageBox->setTextColor(Color4B::RED);
    	this->addChild(messageBox, 1);
    }

	auto backbutton = Button::create("back.png", "back.png", "back.png");
	backbutton->setPosition(origin+Vec2(0.06f*visibleSize.width, 0.95f*visibleSize.height));
	backbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			Director::getInstance()->replaceScene(TransitionMoveInL::create(0.5f, LoginScene::createScene()));
			break;
		default:
			break;
		}
	});
	this->addChild(backbutton);

	auto button = Button::create("button.png", "button.png", "inputbox.png");
	button->setPosition(origin+Vec2(visibleSize.width / 2, visibleSize.height *0.4));
	button->setTitleText("Reg");
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

	_listener = EventListenerCustom::create("user_reg_event", [=](EventCustom* event) {
		int* eventSign = static_cast<int*>(event->getUserData());

		switch (*eventSign)
		{
		case 0:messageBox->setString("Username Existed!"); break;
		case 1:
			messageBox->setString("Success!");
			this->scheduleOnce(schedule_selector(RegScene::EnterLoginScene), 1.0f); break;
		default:
			break;
		}

		//CC_SAFE_DELETE(eventSign);
	});

	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_listener, 1);

	this->schedule(schedule_selector(RegScene::checkHeart), HEART_CHECK);

	return true;
}

void RegScene::regButtonCallback(Ref *pSender)
{
	if(usernameInput->getString().length() > 8)
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


		head[0] = 0x12; head[1] = 0x00;
		head[2] = 44; head[3] = 0x00;

		std::string Message;
		Message.append(head, sizeof(head));
		Message.append(username, sizeof(username));
		Message.append(password, sizeof(password));

		AppDelegate* app = (AppDelegate *)Application::getInstance();
		app->_client->sendMessage(Message.c_str(), Message.length());

		
	}
	
}

void RegScene::EnterLoginScene(float dt)
{
	Director::getInstance()->replaceScene(TransitionMoveInL::create(0.5f, LoginScene::createScene()));
}

void RegScene::checkHeart(float dt)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	if (!app->IsHeart)
	{
		app->IsHeart = false;
		messageBox->setString("network disconnect!");
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, LoginScene::createScene()));
	}
	else
		messageBox->setString("");

	app->IsHeart = false;
}
