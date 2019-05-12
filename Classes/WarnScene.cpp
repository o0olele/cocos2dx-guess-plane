#include "WarnScene.h"
#include "HomeScene.h"
#include "SetPlaneScene.h"
#include "DuelScene.h"
#include "AppDelegate.h"
#include "Global.h"

USING_NS_CC;

Scene* WarnScene::createScene()
{
	auto scene = Scene::create();
	auto layer = WarnScene::create();
	scene->addChild(layer);
	return scene;
}

bool WarnScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	TTFConfig ttfConfig("fonts/showg.ttf", 16);

	/*Warning - Loading*/
	if (app->getWarnType() == GetAllUserIng || app->getWarnType() == GetAllUserEd)
	{
		Sprite* sp = Sprite::create("warnload.png");
		sp->setPosition(origin + Vec2(visibleSize) - Vec2(sp->getContentSize() / 2));

		loadingSp = Sprite::create("warnloadc.png");
		loadingSp->setPosition(origin + Vec2(visibleSize) / 2);

		Size size_pai = loadingSp->getContentSize();
		auto scale = (float)visibleSize.width * 0.5 / (float)size_pai.width;

		loadingSp->setScale(scale, scale);

		Sprite* loadingTSp = Sprite::create("warnloadt.png");
		loadingTSp->setPosition(origin + Vec2(visibleSize) / 2);
		loadingTSp->setScale(scale, scale);

		this->addChild(sp);
		this->addChild(loadingSp);
		this->addChild(loadingTSp);

		this->scheduleUpdate();
	}
	/*Warning - after Choose online user*/
	else if (app->getWarnType() == ChooseUserIng || app->getWarnType() == ChooseUserEd)
	{
		Sprite* sp = Sprite::create("warnload.png");
		sp->setPosition(origin + Vec2(visibleSize) - Vec2(sp->getContentSize() / 2));
		this->addChild(sp);

		Sprite* reqSp = Sprite::create("request.png");
		reqSp->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height*0.65f));
		reqSp->setScale(visibleSize.width*0.8f / reqSp->getContentSize().width, visibleSize.width*0.8f / reqSp->getContentSize().width);
		this->addChild(reqSp);

		TTFConfig LttfConfig("fonts/bauh.ttf", 16);
		auto nameL = Label::createWithTTF(LttfConfig, GLOBAL_TRIM(app->getDuelUser()));
		nameL->setPosition(Vec2(reqSp->getContentSize().width / 2, reqSp->getContentSize().height / 2));
		nameL->setTextColor(Color4B::BLACK);
		reqSp->addChild(nameL);

		auto button = Button::create("button.png", "button.png", "inputbox.png");
		button->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 0.25));
		button->setTitleText("Yes");
		button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				break;
			case ui::Widget::TouchEventType::ENDED:
				yesButtonCallback(this);
				break;
			default:
				break;
			}
		});
		this->addChild(button);

		auto nobutton = Button::create("inputbox.png", "inputbox.png", "inputbox.png");
		nobutton->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 0.15));
		nobutton->setTitleText("No");
		nobutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				break;
			case ui::Widget::TouchEventType::ENDED:
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, HomeScene::createScene()));
				break;
			default:
				break;
			}
		});
		this->addChild(nobutton);
	}
	/*Warning - Waiting result of game invitation*/
	else if (app->getWarnType() == WaitAcceptIng || app->getWarnType() == WaitAcceptEd)
	{
		Sprite* sp = Sprite::create("warnload.png");
		sp->setPosition(origin + Vec2(visibleSize) - Vec2(sp->getContentSize() / 2));
		this->addChild(sp);

		Sprite* waitSp = Sprite::create("waitaccept.png");
		waitSp->setPosition(origin + Vec2(visibleSize) / 2);
		this->addChild(waitSp);

		auto _listener = EventListenerCustom::create("invite_result_event", [=](EventCustom* event) 
		{
			AppDelegate* app = (AppDelegate *)Application::getInstance();
			int* eventSign = static_cast<int*>(event->getUserData());

			switch (*eventSign)
			{
			case 1:
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, SetPlaneScene::createScene())); 
				break;//accept
			case 2:
				app->setWarnType(InviteRefused);
				Director::getInstance()->replaceScene(WarnScene::createScene()); 
				break;//refuse
			default:
				break;
			}

			//CC_SAFE_DELETE(eventSign);
		});

		Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_listener, 1);
	}
	else if (app->getWarnType() == InviteRefused)
	{
		Sprite* sp = Sprite::create("warnload.png");
		sp->setPosition(origin + Vec2(visibleSize) - Vec2(sp->getContentSize() / 2));
		this->addChild(sp);

		Sprite* refuseSp = Sprite::create("refuse.png");
		refuseSp->setPosition(origin + Vec2(visibleSize) / 2);
		refuseSp->setScale(visibleSize.width / refuseSp->getContentSize().width, visibleSize.width / refuseSp->getContentSize().width);
		this->addChild(refuseSp);

		auto backbutton = Button::create("back.png", "back.png", "back.png");
		backbutton->setPosition(origin + Vec2(0.06f*visibleSize.width, 0.95f*visibleSize.height));
		backbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				break;
			case ui::Widget::TouchEventType::ENDED:
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, HomeScene::createScene()));
				break;
			default:
				break;
			}
		});
		this->addChild(backbutton);
	}
	/*Warn - get game invitation from other online user*/
	else if (app->getWarnType() == GetChallengeIng || app->getWarnType() == GetChallengeIng)
	{
		Sprite* sp = Sprite::create("warnload.png");
		sp->setPosition(origin + Vec2(visibleSize) - Vec2(sp->getContentSize() / 2));
		this->addChild(sp);

		Sprite* reqSp = Sprite::create("challenge.png");
		reqSp->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height*0.65f));
		reqSp->setScale(visibleSize.width*0.8f / reqSp->getContentSize().width, visibleSize.width*0.8f / reqSp->getContentSize().width);
		this->addChild(reqSp);

		TTFConfig LttfConfig("fonts/bauh.ttf", 16);
		auto nameL = Label::createWithTTF(LttfConfig, GLOBAL_TRIM(app->getDuelUser()));
		nameL->setPosition(Vec2(reqSp->getContentSize().width / 2, reqSp->getContentSize().height / 2));
		nameL->setTextColor(Color4B::BLACK);
		reqSp->addChild(nameL);

		auto button = Button::create("button.png", "button.png", "inputbox.png");
		button->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 0.25));
		button->setTitleText("Yes");
		button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				break;
			case ui::Widget::TouchEventType::ENDED:
				acceptButtonCallback(this);
				break;
			default:
				break;
			}
		});
		this->addChild(button);

		auto nobutton = Button::create("inputbox.png", "inputbox.png", "inputbox.png");
		nobutton->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 0.15));
		nobutton->setTitleText("No");
		nobutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				break;
			case ui::Widget::TouchEventType::ENDED:
				refuseButtonCallback(this);
				break;
			default:
				break;
			}
		});
		this->addChild(nobutton);
	}
	else if (app->getWarnType() == DecideFirstIng || app->getWarnType() == DecideFirstEd)
	{
		Sprite* sp = Sprite::create("warnload.png");
		sp->setPosition(origin + Vec2(visibleSize) - Vec2(sp->getContentSize() / 2));
		this->addChild(sp);

		Sprite* waitSp = Sprite::create("waitaccept.png");
		waitSp->setPosition(origin + Vec2(visibleSize) / 2);
		this->addChild(waitSp);

		this->schedule(schedule_selector(WarnScene::checkStart), 1.0f);
	}
	else if (app->getWarnType() == GameResult)
	{
		Texture2D * texture = Director::getInstance()->getTextureCache()->addImage("backp.png");
		Sprite* sp = Sprite::createWithTexture(texture);
		sp->setPosition(origin + Vec2(visibleSize) - Vec2(sp->getContentSize() / 2));
		this->addChild(sp);

		auto backbutton = Button::create("back.png", "back.png", "back.png");
		backbutton->setPosition(origin + Vec2(0.06f*visibleSize.width, 0.95f*visibleSize.height));
		backbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				break;
			case ui::Widget::TouchEventType::ENDED:
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, HomeScene::createScene()));
				break;
			default:
				break;
			}
		});
		this->addChild(backbutton);

		if (app->getGameResult() == 0x10 || app->getGameResult() == 0x11 || app->getGameResult() == 0x12)
		{
			Sprite* winSp = Sprite::create("win.jpeg");
			winSp->setPosition(origin + Vec2(visibleSize) / 2);
			winSp->setScale(visibleSize.width / winSp->getContentSize().width, visibleSize.width / winSp->getContentSize().width);
			this->addChild(winSp);
		}
		else if (app->getGameResult() == 0x20)
		{
			Sprite* loseSp = Sprite::create("lose.jpeg");
			loseSp->setPosition(origin + Vec2(visibleSize) / 2);
			loseSp->setScale(visibleSize.width / loseSp->getContentSize().width, visibleSize.width / loseSp->getContentSize().width);
			this->addChild(loseSp);
		}
		else if (app->getGameResult() == 0x30)
		{
			Sprite* tieSp = Sprite::create("tie.jpg");
			tieSp->setPosition(origin + Vec2(visibleSize) / 2);
			tieSp->setScale(visibleSize.width / tieSp->getContentSize().width, visibleSize.width / tieSp->getContentSize().width);
			this->addChild(tieSp);
		}
	}
	else if (app->getWarnType() == Surrender)
	{
		Sprite* sp = Sprite::create("warnload.png");
		sp->setPosition(origin + Vec2(visibleSize) - Vec2(sp->getContentSize() / 2));
		this->addChild(sp);

		Sprite* giveUp = Sprite::create("giveup.png");
		giveUp->setPosition(origin + Vec2(visibleSize) / 2);
		giveUp->setScale(visibleSize.width / giveUp->getContentSize().width, visibleSize.width / giveUp->getContentSize().width);
		this->addChild(giveUp);

		auto button = Button::create("button.png", "button.png", "inputbox.png");
		button->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 0.25));
		button->setTitleText("Yes");
		button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				break;
			case ui::Widget::TouchEventType::ENDED:
				giveButtonCallback(this);
				break;
			default:
				break;
			}
		});
		this->addChild(button);

		auto nobutton = Button::create("inputbox.png", "inputbox.png", "inputbox.png");
		nobutton->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 0.15));
		nobutton->setTitleText("No");
		nobutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
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
		this->addChild(nobutton);
	}

	return true;
}

void WarnScene::update(float dt)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();


	theta += 2;
	loadingSp->setRotation(theta);

	if (theta > 360 && app->getWarnType() == GetAllUserEd)
		Director::getInstance()->replaceScene(HomeScene::createScene());

}

void WarnScene::checkStart(float dt)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	if (app->IsStart)
		Director::getInstance()->replaceScene(DuelScene::createScene());
}

/*belong to Warning - after Choose online user*/
void WarnScene::yesButtonCallback(Ref * pSender)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();
	app->setWarnType(WaitAcceptIng);

	char head[4] = { 0 };
	char username[8] = { 0 };
	strcpy(username, app->getDuelUser().c_str());


	head[0] = 0x14; head[1] = 0x00;
	head[2] = 12; head[3] = 0x00;

	std::string Message;
	Message.append(head, sizeof(head));
	Message.append(username, sizeof(username));

	app->_client->sendMessage(Message.c_str(), Message.length());

	Director::getInstance()->replaceScene(WarnScene::createScene());
}

void WarnScene::acceptButtonCallback(Ref * pSender)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	char head[4] = { 0 };
	char username[8] = { 0 };
	strcpy(username, app->getDuelUser().c_str());


	head[0] = 0x14; head[1] = 0x01;
	head[2] = 12; head[3] = 0x00;

	std::string Message;
	Message.append(head, sizeof(head));
	Message.append(username, sizeof(username));

	app->_client->sendMessage(Message.c_str(), Message.length());

	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, SetPlaneScene::createScene()));
}

void WarnScene::refuseButtonCallback(Ref * pSender)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	char head[4] = { 0 };
	char username[8] = { 0 };
	strcpy(username, app->getDuelUser().c_str());


	head[0] = 0x14; head[1] = 0x02;
	head[2] = 12; head[3] = 0x00;

	std::string Message;
	Message.append(head, sizeof(head));
	Message.append(username, sizeof(username));

	app->_client->sendMessage(Message.c_str(), Message.length());
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, HomeScene::createScene()));
}

void WarnScene::giveButtonCallback(Ref * pSender)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	char head[4] = { 0 };

	head[0] = 0x18; head[1] = 0x00;
	head[2] = 4; head[3] = 0x00;

	app->_client->sendMessage(head, 4);
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, HomeScene::createScene()));
}
