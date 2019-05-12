#include "DuelScene.h"
#include "MyPlaneScene.h"
#include "WarnScene.h"
#include "LoginScene.h"
#include "Global.h"
#include "AppDelegate.h"

USING_NS_CC;

Scene* DuelScene::createScene()
{
	auto scene = Scene::create();
	auto layer = DuelScene::create();
	scene->addChild(layer);
	return scene;
}

void DuelScene::onEnter()
{
	Scene::onEnter();
	// Register Touch Event
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	if (app->isMyTurn())
	{
		listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = CC_CALLBACK_2(DuelScene::onTouchBegan, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, gridSp);
	}
}

void DuelScene::onExit()
{
	Scene::onExit();
}

// on "init" you need to initialize your instance
bool DuelScene::init()
{
	if (!Scene::init())
	{
		return false;
	}
	AppDelegate* app = (AppDelegate *)Application::getInstance();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	Texture2D * texture = Director::getInstance()->getTextureCache()->addImage("backp.png");
    	Sprite* spBack = Sprite::createWithTexture(texture);
    spBack->setPosition(origin+Vec2(visibleSize.width / 2, visibleSize.height / 2));
    spBack->setAnchorPoint(Point(0.5, 0.5));
    spBack->setScale(visibleSize.width / spBack->getContentSize().width, visibleSize.width / spBack->getContentSize().width);
    this->addChild(spBack);

	gridSp = Sprite::create("gridduel.png");
	gridSp->setPosition(origin+Vec2(visibleSize.width / 2, visibleSize.height * 0.6));
	gridSp->setAnchorPoint(Point(0.5, 0.5));

	Size size_pai = gridSp->getContentSize();

	scale = (float)visibleSize.width * 0.8 / (float)size_pai.width;
	gridSp->setScale(scale, scale);
	this->addChild(gridSp);

	auto backbutton= Button::create("back.png", "back.png", "back.png");
	backbutton->setPosition(origin+Vec2(0.06f*visibleSize.width, 0.95f*visibleSize.height));
	backbutton->setScale(scale, scale);
	backbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		AppDelegate* app = (AppDelegate *)Application::getInstance();
		app->setWarnType(Surrender);
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			Director::getInstance()->pushScene(WarnScene::create());
			break;
		default:
			break;
		}
	});
	this->addChild(backbutton);

	auto forwardbutton = Button::create("forward.png", "forward.png", "forward.png");
	forwardbutton->setScale(scale, scale);
	forwardbutton->setPosition(origin+Vec2(0.94f*visibleSize.width, 0.95f*visibleSize.height));
	forwardbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			Director::getInstance()->pushScene(MyPlaneScene::create());
			break;
		default:
			break;
		}
	});
	this->addChild(forwardbutton);

	doublebutton = Button::create("doubler.png", "double.png", "double.png");
	doublebutton->setPosition(origin+Vec2(visibleSize.width / 4, 0.65f*visibleSize.height + 0.5f*gridSp->getBoundingBox().size.height));
	doublebutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			doubleButtonCallback(this);
			break;
		default:
			break;
		}
	});
	this->addChild(doublebutton);

	bombbutton = Button::create("king.png", "king.png", "king.png");
	bombbutton->setPosition(origin + Vec2(visibleSize.width / 2, 0.65f*visibleSize.height + 0.5f*gridSp->getBoundingBox().size.height));
	bombbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			bombButtonCallback(this);
			break;
		default:
			break;
		}
	});
	this->addChild(bombbutton);

	clearbutton = Button::create("clear.png", "clear.png", "clear.png");
	clearbutton->setPosition(origin+Vec2(visibleSize.width * 3 / 4, 0.65f*visibleSize.height + 0.5f*gridSp->getBoundingBox().size.height));
	clearbutton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			clearButtonCallback(this);
			break;
		default:
			break;
		}
	});
	this->addChild(clearbutton);

	turnSp = Sprite::create("turn.png");
	turnSp->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 0.25));
	turnSp->setScale(scale, scale);
	this->addChild(turnSp);

	if (!app->isMyTurn())
		turnSp->setVisible(false);

	Sprite* ScoreSp = Sprite::create("button.png");
	ScoreSp->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 0.15));
	ScoreSp->setScale(scale, scale);
	this->addChild(ScoreSp);

	TTFConfig ttfConfig("fonts/showg.ttf", 12);
	ScoreLb = Label::createWithTTF(ttfConfig, std::to_string(app->getMyScore()));
	ScoreLb->setPosition(ScoreSp->getContentSize().width / 2, ScoreSp->getContentSize().height / 2);
	ScoreSp->addChild(ScoreLb);

	nulltexture = Director::getInstance()->getTextureCache()->addImage("null.png");
	ptexture = Director::getInstance()->getTextureCache()->addImage("p.png");
	phtexture = Director::getInstance()->getTextureCache()->addImage("ph.png");
	questexture = Director::getInstance()->getTextureCache()->addImage("ques.png");

	auto _listener = EventListenerCustom::create("duel_event", [=](EventCustom* event) {
		DuelEventCallback(this);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_listener, 1);

	auto _listener_game = EventListenerCustom::create("game_result_event", [=](EventCustom* event) {
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, WarnScene::createScene()));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_listener_game, 2);

	this->schedule(schedule_selector(DuelScene::checkResult), 0.5f);
	this->schedule(schedule_selector(DuelScene::checkHeart), float(HEART_CHECK));
	
	return true;
}

void DuelScene::clearButtonCallback(Ref * pSender)
{
	doublebutton->setEnabled(true);

	if (gridSp->getChildByTag(FirstChild))
		gridSp->removeChildByTag(FirstChild);
	if (gridSp->getChildByTag(SecondChild))
		gridSp->removeChildByTag(SecondChild);
	if (gridSp->getChildByTag(ThirdChild))
		gridSp->removeChildByTag(ThirdChild);
	if (gridSp->getChildByTag(FourthChild))
		gridSp->removeChildByTag(FourthChild);

	MarkCnt = 0;
	MaxMarkNum = 2;

	i1 = 0; i2 = 0;
	j1 = 0; j2 = 0;
}

void DuelScene::doubleButtonCallback(Ref * pSender)
{
	MaxMarkNum = 4;
	doublebutton->setEnabled(false);
}

void DuelScene::bombButtonCallback(Ref * pSender)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	if (MarkCnt <= 2 && MarkCnt > 0)
	{
		char head[5] = { 0 };
		head[0] = 0x17; head[1] = 0x01;
		head[2] = 5; head[3] = 0x00;
		head[4] = i1 * 16 + j1;

		app->_client->sendMessage(head, sizeof(head));
	}
	else if(MarkCnt > 2)
	{
		char head[6] = { 0 };
		head[0] = 0x17; head[1] = 0x02;
		head[2] = 6; head[3] = 0x00;
		head[4] = i1 * 16 + j1;
		head[5] = i2 * 16 + j2;

		app->_client->sendMessage(head, sizeof(head));
	}

	//TurnCnt++;
}

bool DuelScene::onTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event)
{
	if (MarkCnt < MaxMarkNum)
	{
		auto target = static_cast<Sprite*>(event->getCurrentTarget());

		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width, s.height);

		int nx = locationInNode.x / (s.width / MINE_GRID_SIZE);
		int ny = locationInNode.y / (s.width / MINE_GRID_SIZE);

		if (nx < 10 && ny < 10 && nx >= 0 && ny >= 0 && ((guess[9 - ny][nx] < 10 && MaxMarkNum==2) || MaxMarkNum==4))
		{
			MarkCnt++;

			Sprite* _nullsp = Sprite::createWithTexture(questexture);
			_nullsp->setAnchorPoint(Point(0, 0));
			_nullsp->setPosition(Point(nx*(s.width / MINE_GRID_SIZE), ny*(s.width / MINE_GRID_SIZE)));
			_nullsp->setTag(FirstChild - 1 + MarkCnt);
			target->addChild(_nullsp);

			if (MarkCnt <= 2)
			{
				i1 = nx;
				j1 = 9 - ny;
			}
			else
			{
				i2 = nx;
				j2 = 9 - ny;
			}

		}
	}
	return false;
}

void DuelScene::DuelEventCallback(Ref * pSender)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	if (app->isMyTurn())
	{
		listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = CC_CALLBACK_2(DuelScene::onTouchBegan, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, gridSp);

		turnSp->setVisible(true);
	}
	else
	{
		_eventDispatcher->removeEventListenersForTarget(gridSp);
		turnSp->setVisible(false);
	}

	ScoreLb->setString(std::to_string(app->getMyScore()));
}

void DuelScene::SingleResult(std::string pic)
{
	MarkCnt = 0;
	if (gridSp->getChildByTag(FirstChild))
		gridSp->removeChildByTag(FirstChild);
	if (gridSp->getChildByTag(SecondChild))
		gridSp->removeChildByTag(SecondChild);
	if (gridSp->getChildByTag(ThirdChild))
		gridSp->removeChildByTag(ThirdChild);
	if (gridSp->getChildByTag(FourthChild))
		gridSp->removeChildByTag(FourthChild);

	Sprite* _nullsp;
	if (pic == "null.png")
		_nullsp = Sprite::createWithTexture(nulltexture);
	else if(pic == "p.png")
		_nullsp = Sprite::createWithTexture(ptexture);
	else
		_nullsp = Sprite::createWithTexture(phtexture);

	_nullsp->setAnchorPoint(Point(0, 0));
	_nullsp->setPosition(Point(i1*(gridSp->getContentSize().width / MINE_GRID_SIZE), (9-j1)*(gridSp->getContentSize().width / MINE_GRID_SIZE)));
	//_nullsp->setTag(110);
	gridSp->addChild(_nullsp);
}

void DuelScene::DoubleDead()
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	MarkCnt = 0;
	MaxMarkNum = 2;
	doublebutton->setEnabled(true);
	if (gridSp->getChildByTag(FirstChild))
		gridSp->removeChildByTag(FirstChild);
	if (gridSp->getChildByTag(SecondChild))
		gridSp->removeChildByTag(SecondChild);
	if (gridSp->getChildByTag(ThirdChild))
		gridSp->removeChildByTag(ThirdChild);
	if (gridSp->getChildByTag(FourthChild))
		gridSp->removeChildByTag(FourthChild);


	int Headi = app->getDoubleHead() / 16;
	int Headj = app->getDoubleHead() % 16;
	int Taili = app->getDoubleTail() / 16;
	int Tailj = app->getDoubleTail() % 16;

	int i = Headj; int j = Headi; int num = 0x21;

	if (Headi == Taili && Headj < Tailj)
	{
		
		guess[i][j] = num ? num + 1 : num;
		guess[i + 1][j - 2] = num; guess[i + 1][j - 1] = num; guess[i + 1][j] = num; guess[i + 1][j + 1] = num; guess[i + 1][j + 2] = num;
		guess[i + 2][j] = num;
		guess[i + 3][j - 1] = num; guess[i + 3][j] = num; guess[i + 3][j + 1] = num;
	}
	else if (Headj = Tailj && Headi > Taili)
	{

		guess[i][j] = num ? num + 1 : num;
		guess[i - 2][j - 1] = num; guess[i - 1][j - 1] = num; guess[i][j - 1] = num; guess[i + 1][j - 1] = num; guess[i + 2][j - 1] = num;
		guess[i][j - 2] = num;
		guess[i - 1][j - 3] = num; guess[i][j - 3] = num; guess[i + 1][j - 3] = num;
	}
	else if (Headi = Taili && Headj > Tailj)
	{
		guess[i][j] = num ? num + 1 : num;
		guess[i - 1][j - 2] = num; guess[i - 1][j - 1] = num; guess[i - 1][j] = num; guess[i - 1][j + 1] = num; guess[i - 1][j + 2] = num;
		guess[i - 2][j] = num;
		guess[i - 3][j - 1] = num; guess[i - 3][j] = num; guess[i - 3][j + 1] = num;
	}
	else
	{
		guess[i][j] = num ? num + 1 : num;
		guess[i - 2][j + 1] = num; guess[i - 1][j + 1] = num; guess[i][j + 1] = num; guess[i + 1][j + 1] = num; guess[i + 2][j + 1] = num;
		guess[i][j + 2] = num;
		guess[i - 1][j + 3] = num; guess[i][j + 3] = num; guess[i + 1][j + 3] = num;
	}

	for(int a=0; a < 10; a++)
		for (int b = 0; b < 10; b++)
		{
			if (guess[a][b] == 0x21)
			{
				Sprite* _nullsp = Sprite::createWithTexture(ptexture);
				_nullsp->setAnchorPoint(Point(0, 0));
				_nullsp->setPosition(Point(b*(gridSp->getContentSize().width / MINE_GRID_SIZE), (9 - a)*(gridSp->getContentSize().width / MINE_GRID_SIZE)));
				//_nullsp->setTag(110);
				gridSp->addChild(_nullsp);
			}
			else if (guess[a][b] == 0x22)
			{
				Sprite* _nullsp = Sprite::createWithTexture(phtexture);
				_nullsp->setAnchorPoint(Point(0, 0));
				_nullsp->setPosition(Point(b*(gridSp->getContentSize().width / MINE_GRID_SIZE), (9 - a)*(gridSp->getContentSize().width / MINE_GRID_SIZE)));
				//_nullsp->setTag(110);
				gridSp->addChild(_nullsp);
			}
		}
}

void DuelScene::checkResult(float dt)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();
	if (app->getResult() == 0x10)
	{
		SingleResult("null.png");
		app->setPresResult(0);
		guess[j1][i1] = 0x10;
	}
	else if (app->getResult() == 0x11)
	{
		SingleResult("p.png");
		app->setPresResult(0);
		guess[j1][i1] = 0x11;
	}
	else if (app->getResult() == 0x12)
	{
		SingleResult("ph.png");
		app->setPresResult(0);
		guess[j1][i1] = 0x12;
	}
	else if (app->getResult() == 0x21)
	{
		DoubleDead();
		app->setPresResult(0);
	}
	else if (app->getResult() == 0x20)
	{
		doublebutton->setEnabled(true);

		if (gridSp->getChildByTag(FirstChild))
			gridSp->removeChildByTag(FirstChild);
		if (gridSp->getChildByTag(SecondChild))
			gridSp->removeChildByTag(SecondChild);
		if (gridSp->getChildByTag(ThirdChild))
			gridSp->removeChildByTag(ThirdChild);
		if (gridSp->getChildByTag(FourthChild))
			gridSp->removeChildByTag(FourthChild);

		MarkCnt = 0;
		MaxMarkNum = 2;

		i1 = 0; i2 = 0;
		j1 = 0; j2 = 0;

		app->setPresResult(0);
	}

}

void DuelScene::checkHeart(float dt)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	if (!app->IsHeart)
	{
		//log("HomeScene: network disconnect!");
		app->IsHeart = false;
		app->CleanAll();
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, LoginScene::createScene()));
	}

	app->IsHeart = false;
}
