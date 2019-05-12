#include "HomeScene.h"
#include "Global.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "AppDelegate.h"
#include "WarnScene.h"
#include "LoginScene.h"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* HomeScene::createScene()
{
	auto scene = Scene::create();
	auto layer = HomeScene::create();
	scene->addChild(layer);
	return scene;
}

bool HomeScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/*Background Image*/
	Sprite* spBack = Sprite::create("allback.png");
    spBack->setPosition(origin + Vec2(visibleSize) / 2);
	spBack->setScale(visibleSize.width / spBack->getContentSize().width, visibleSize.width / spBack->getContentSize().width);
    this->addChild(spBack);

	/*User Info Image*/
	Sprite* mine= Sprite::create("user.png");
	mine->setPosition(origin + Vec2(visibleSize.width/2,visibleSize.height*0.85));

	Size size_pai = mine->getContentSize();
	scale = (float)visibleSize.width * 0.8 / (float)size_pai.width;

	mine->setScale(scale, scale);
	this->addChild(mine);

	/*User Info Label*/
	AppDelegate* app = (AppDelegate *)Application::getInstance();
	std::string userinfo = app->getUser();
	userinfo.push_back('\0');
	//log("HomeScene %s", app->getOnlineUser());
	app->setWarnType(ChooseUserIng);

	TTFConfig ttfConfig("fonts/showg.ttf", 12);
	TTFConfig LttfConfig("fonts/bauh.ttf", 12);

	auto userBox = Label::createWithTTF(LttfConfig, "");
	if (userBox) {
		userBox->setString(GLOBAL_TRIM(userinfo.substr(0, 8)));
		userBox->setPosition(Vec2(mine->getContentSize().width / 2, mine->getContentSize().height * 0.7));
		userBox->setTextColor(Color4B::BLACK);
		mine->addChild(userBox, 1);
	}

	auto succBox = Label::createWithTTF(ttfConfig, "");
	if (succBox) {
		succBox->setString(std::to_string(u_char(userinfo[8]) + 256 * u_char(userinfo[9])));
		succBox->setPosition(Vec2(mine->getContentSize().width * 0.25f, mine->getContentSize().height * 0.25));
		succBox->setTextColor(Color4B::BLACK);
		mine->addChild(succBox, 1); 
	}

	auto failBox = Label::createWithTTF(ttfConfig, "");
	if (failBox) {
		failBox->setString(std::to_string(u_char(userinfo[10]) + 256 * u_char(userinfo[11])));
		failBox->setPosition(Vec2(mine->getContentSize().width * 0.75f, mine->getContentSize().height * 0.25));
		failBox->setTextColor(Color4B::BLACK);
		mine->addChild(failBox, 1);
	}

	/*Table View Image*/
	Sprite* table = Sprite::create("cell.png");
	table->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height*0.8-scale*size_pai.height/2-table->getContentSize().height*scale/2));
	table->setScale(scale, scale);
	this->addChild(table);

	_cellwidth = visibleSize.width;
	_cellheight = visibleSize.height * 0.1;

	/*Table View*/
	TableView *tv = TableView::create(this, cocos2d::Size(visibleSize.width, visibleSize.height*0.6));
	tv->setAnchorPoint(Point(0, 0));
	tv->setPosition(origin + Vec2(0, visibleSize.height*0.1));
	tv->setDelegate(this);
	this->addChild(tv);

	auto button = Button::create("inputbox.png", "inputbox.png", "inputbox.png");
	button->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 0.05));
	button->setTitleText("Exit");
	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			Director::getInstance()->end();
			break;
		default:
			break;
		}
	});
	this->addChild(button);

	/*Custom Event Listener - Online user Changed*/
	_listener = EventListenerCustom::create("user_add_event", [=](EventCustom* event) {
		Director::getInstance()->replaceScene(HomeScene::createScene());
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_listener, 1);

	/*Custom Event Listener - Get Game Invitation from other user*/
	_listener_ch = EventListenerCustom::create("get_challenge_event", [=](EventCustom* event) {
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, WarnScene::createScene()));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_listener_ch, 2);

	/*Heart Check*/
	this->schedule(schedule_selector(HomeScene::checkHeart), float(HEART_CHECK));

	return true;
}

Size HomeScene::cellSizeForTable(cocos2d::extension::TableView *table) 
{
	return Size(_cellwidth, _cellheight);
}


TableViewCell* HomeScene::tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx) 
{

	AppDelegate* app = (AppDelegate *)Application::getInstance();
	std::string otheruser = app->getOnlineUser().substr(12 * idx, 12 * (idx + 1) - 1);

	TableViewCell *cell = table->dequeueCell();
	Label *nameL;
	Label *succL;
	Label *failL;

	if (cell == NULL) {
		cell = TableViewCell::create();
		TTFConfig ttfConfig("fonts/showg.ttf", 12);
		TTFConfig LttfConfig("fonts/bauh.ttf", 12);
		nameL = Label::createWithTTF(LttfConfig, "");
		succL = Label::createWithTTF(ttfConfig, "");
		failL = Label::createWithTTF(ttfConfig, "");

		nameL->setTag(1201);
		nameL->setColor(Color3B::BLACK);
		nameL->setAnchorPoint(Point(0, 0));
		nameL->setPosition(Vec2(0.11f*_cellwidth,0));
		nameL->setString(GLOBAL_TRIM(otheruser.substr(0, 8)));

		succL->setTag(1202);
		succL->setColor(Color3B::BLACK);
		succL->setAnchorPoint(Point(0, 0));
		succL->setPosition(Vec2(0.4f*_cellwidth, 0));
		succL->setString("win:" + std::to_string(u_char(otheruser[8]) + 256 * u_char(otheruser[9])));

		failL->setTag(1203);
		failL->setColor(Color3B::BLACK);
		failL->setAnchorPoint(Point(0, 0));
		failL->setPosition(Vec2(0.7f*_cellwidth, 0));
		failL->setString("lose:" + std::to_string(u_char(otheruser[10]) + 256 * u_char(otheruser[11])));

		cell->addChild(nameL);
		cell->addChild(succL);
		cell->addChild(failL);
	}
	else
	{
		nameL = (Label*)cell->getChildByTag(1201);
		succL = (Label*)cell->getChildByTag(1202);
		failL = (Label*)cell->getChildByTag(1203);

		nameL->setString(otheruser.substr(0, 8));
		succL->setString("win:" + std::to_string(u_char(otheruser[8]) + 256 * u_char(otheruser[9])));
		failL->setString("lose:" + std::to_string(u_char(otheruser[10]) + 256 * u_char(otheruser[11])));
	}

	return cell;
}

ssize_t HomeScene::numberOfCellsInTableView(cocos2d::extension::TableView *table) {
	AppDelegate* app = (AppDelegate *)Application::getInstance();
	return app->getOnlineUser().length()/12;
}

void HomeScene::tableCellTouched(cocos2d::extension::TableView *table, cocos2d::extension::TableViewCell *cell) {
	AppDelegate* app = (AppDelegate *)Application::getInstance();
	Label *label = (Label*)cell->getChildByTag(1201);

	//Set Duel User
	app->setDuelUser(label->getString());

	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, WarnScene::createScene()));
}

void HomeScene::checkHeart(float dt)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	if (!app->IsHeart)
	{
		log("HomeScene: network disconnect!");
		app->IsHeart = false;
		app->CleanAll();
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, LoginScene::createScene()));
	}

	app->IsHeart = false;
}
