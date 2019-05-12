#include "SetPlaneScene.h"
#include "DuelScene.h"
#include "LoginScene.h"
#include "WarnScene.h"

#include "Global.h"
#include "AppDelegate.h"


USING_NS_CC;

Scene* SetPlaneScene::createScene()
{
	auto scene = Scene::create();
	auto layer = SetPlaneScene::create();
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool SetPlaneScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

    Texture2D * texture = Director::getInstance()->getTextureCache()->addImage("backp.png");
	Sprite* spBack = Sprite::createWithTexture(texture);
	spBack->setPosition(origin+Vec2(visibleSize.width / 2, visibleSize.height / 2));
	spBack->setAnchorPoint(Point(0.5, 0.5));
	spBack->setScale(visibleSize.width / spBack->getContentSize().width, visibleSize.width / spBack->getContentSize().width);
	this->addChild(spBack);

	Sprite* sp = Sprite::create("grid.png");
	sp->setPosition(origin+Vec2(visibleSize.width / 2, visibleSize.height / 2 + 0.1f * visibleSize.height));
	sp->setAnchorPoint(Point(0.5, 0.5));

	Size size_pai = sp->getContentSize();

	scale = (float)visibleSize.width * 0.8 / (float)size_pai.width;
	sp->setScale(scale, scale);
	this->addChild(sp);

	auto planeTexture = Director::getInstance()->getTextureCache()->addImage("lplane.png");

	PlaneSp1 = PlaneSprite::createWithTexture(planeTexture);
	PlaneSp1->origin=origin;
	PlaneSp1->setGridPos(origin.x+visibleSize.width / 2 - sp->getBoundingBox().size.width / 2, origin.y+visibleSize.height / 2 + 0.1f * visibleSize.height - sp->getBoundingBox().size.height / 2, sp->getBoundingBox().size.height);
	PlaneSp1->setPosition(origin+Vec2(visibleSize.width / 2, 0.2f * visibleSize.height));
	PlaneSp1->setStartPos(origin.x+visibleSize.width / 2, origin.y+0.2f * visibleSize.height);
	PlaneSp1->setAnchorPoint(Point(0.4f, 0.5f));
	PlaneSp1->setScale(scale/2, scale/2);
	PlaneSp1->setStartScale(scale);
	PlaneSp1->setTag(10);
	
	this->addChild(PlaneSp1);

	rotate1 = Button::create("rotate.png", "rotate.png", "rotate.png");
	rotate1->setPosition(origin + Vec2(visibleSize.width / 2, 0.1f * visibleSize.height));
	rotate1->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			rotate1ButtonCallback(this);
			break;
		default:
			break;
		}
	});
	this->addChild(rotate1);

	PlaneSp2 = PlaneSprite::createWithTexture(planeTexture);
	PlaneSp2->origin=origin;
	PlaneSp2->setGridPos(origin.x+visibleSize.width / 2 - sp->getBoundingBox().size.width / 2, origin.y+visibleSize.height / 2 + 0.1f * visibleSize.height - sp->getBoundingBox().size.height / 2, sp->getBoundingBox().size.height);
	PlaneSp2->setPosition(origin+Vec2(visibleSize.width / 2+0.3f*visibleSize.width, 0.2f * visibleSize.height));
	PlaneSp2->setStartPos(origin.x+visibleSize.width / 2+0.3f*visibleSize.width, origin.y+0.2f * visibleSize.height);
	PlaneSp2->setAnchorPoint(Point(0.4f, 0.5f));
	PlaneSp2->setScale(scale / 2, scale / 2);
	PlaneSp2->setStartScale(scale);
	PlaneSp2->setTag(11);
	
	this->addChild(PlaneSp2);

	rotate2 = Button::create("rotate.png", "rotate.png", "rotate.png");
	rotate2->setPosition(origin + Vec2(visibleSize.width / 2 + 0.3f*visibleSize.width, 0.1f * visibleSize.height));
	rotate2->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			rotate2ButtonCallback(this);
			break;
		default:
			break;
		}
	});
	this->addChild(rotate2);

	PlaneSp3 = PlaneSprite::createWithTexture(planeTexture);
	PlaneSp3->origin=origin;
	PlaneSp3->setGridPos(origin.x+visibleSize.width / 2 - sp->getBoundingBox().size.width / 2, origin.y+visibleSize.height / 2 + 0.1f * visibleSize.height - sp->getBoundingBox().size.height / 2, sp->getBoundingBox().size.height);
	PlaneSp3->setPosition(origin+Vec2(visibleSize.width / 2 - 0.3f*visibleSize.width, 0.2f * visibleSize.height));
	PlaneSp3->setStartPos(origin.x+visibleSize.width / 2 - 0.3f*visibleSize.width, origin.y+0.2f * visibleSize.height);
	PlaneSp3->setAnchorPoint(Point(0.4f, 0.5f));
	PlaneSp3->setScale(scale / 2, scale / 2);
	PlaneSp3->setStartScale(scale);
	PlaneSp3->setTag(12);
	
	this->addChild(PlaneSp3);

	rotate3 = Button::create("rotate.png", "rotate.png", "rotate.png");
	rotate3->setPosition(origin + Vec2(visibleSize.width / 2 - 0.3f*visibleSize.width, 0.1f * visibleSize.height));
	rotate3->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			rotate3ButtonCallback(this);
			break;
		default:
			break;
		}
	});
	this->addChild(rotate3);

	auto button = Button::create("button.png", "inputbox.png", "inputbox.png");
	button->setPosition(origin+Vec2(visibleSize.width / 2, 0.2f * visibleSize.height));
	button->setTitleText("Confirm");
	button->setVisible(false);
	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			confirmButtonCallback(this);
			break;
		default:
			break;
		}
	});
	this->addChild(button);

	TTFConfig ttfConfig("fonts/showg.ttf", 18);
	messageBox = Label::createWithTTF(ttfConfig, "Loading");
	if (messageBox) {
		messageBox->setTextColor(Color4B::BLACK);
		messageBox->setPosition(origin + Vec2(visibleSize.width / 2, 0.2f*visibleSize.height));
		this->addChild(messageBox);
	}

	PlaneSp1->setVisible(false);
	PlaneSp2->setVisible(false);
	PlaneSp3->setVisible(false);
	rotate1->setVisible(false);
	rotate2->setVisible(false);
	rotate3->setVisible(false);

	_listener = EventListenerCustom::create("game_custom_event", [=](EventCustom* event) {

		static bool isA = false;
		static bool isB = false;
		static bool isC = false;
		int* eventSign = static_cast<int*>(event->getUserData());

		if (*eventSign / 10 == PlaneSp1->getTag())
			if (*eventSign % 10)
				isA = true;
			else
				isA = false;

		if (*eventSign / 10 == PlaneSp2->getTag())
		if (*eventSign % 10)
			isB = true;
		else
			isB = false;

		if (*eventSign / 10 == PlaneSp3->getTag())
		if (*eventSign % 10)
			isC = true;
		else
			isC = false;

		if (isA&&isB&&isC)
		{
			button->setVisible(true);
			rotate1->setVisible(false);
			rotate3->setVisible(false);
			rotate2->setVisible(false);
		}
		else
		{
			button->setVisible(false);
			rotate1->setVisible(true);
			rotate3->setVisible(true);
			rotate2->setVisible(true);
		}

		//CC_SAFE_DELETE(eventSign);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_listener, 1);
	
	/*Heart Check*/
	this->schedule(schedule_selector(SetPlaneScene::checkHeart), float(HEART_CHECK));

	this->scheduleOnce(schedule_selector(SetPlaneScene::EnableMove), 2.0f);

	return true;
}

void SetPlaneScene::confirmButtonCallback(Ref * pSender)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	char head[10] = { 0 };

	head[0] = 0x15; head[1] = 0x00;
	head[2] = 10; head[3] = 0x00;
	head[4] = char(PlaneSp1->getJ() * 16 + PlaneSp1->getI()); head[5] = char(PlaneSp1->getJtail() * 16 + PlaneSp1->getItail());
	head[6] = char(PlaneSp2->getJ() * 16 + PlaneSp2->getI()); head[7] = char(PlaneSp2->getJtail() * 16 + PlaneSp2->getItail());
	head[8] = char(PlaneSp3->getJ() * 16 + PlaneSp3->getI()); head[9] = char(PlaneSp3->getJtail() * 16 + PlaneSp3->getItail());

	app->_client->sendMessage(head, 10);
	app->setWarnType(DecideFirstIng);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, WarnScene::createScene()));
}

void SetPlaneScene::checkHeart(float dt)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	if (!app->IsHeart)
	{
		log("SetPlaneScene: network disconnect!");
		app->IsHeart = false;
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, LoginScene::createScene()));
	}

	app->IsHeart = false;
}

void SetPlaneScene::rotate1ButtonCallback(Ref * pSender)
{
	if (PlaneSp1->getScale() < PlaneSp1->_start_scale)
	{
		PlaneSp1->setDirect((PlaneSp1->getDirect() + 1) % 4);
		PlaneSp1->setRotation(PlaneSp1->getDirect() * 90);
	}
}

void SetPlaneScene::rotate2ButtonCallback(Ref * pSender)
{
	if (PlaneSp2->getScale() < PlaneSp2->_start_scale)
	{
		PlaneSp2->setDirect((PlaneSp2->getDirect() + 1) % 4);
		PlaneSp2->setRotation(PlaneSp2->getDirect() * 90);
	}
}

void SetPlaneScene::rotate3ButtonCallback(Ref * pSender)
{
	if (PlaneSp3->getScale() < PlaneSp3->_start_scale)
	{
		PlaneSp3->setDirect((PlaneSp3->getDirect() + 1) % 4);
		PlaneSp3->setRotation(PlaneSp3->getDirect() * 90);
	}
}

void SetPlaneScene::EnableMove(float dt)
{
	PlaneSp1->setVisible(true);
	PlaneSp2->setVisible(true);
	PlaneSp3->setVisible(true);
	rotate1->setVisible(true);
	rotate2->setVisible(true);
	rotate3->setVisible(true);

	messageBox->setVisible(false);
}
