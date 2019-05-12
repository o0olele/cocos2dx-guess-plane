#include "MyPlaneScene.h"

#include "AppDelegate.h"
#include "Global.h"

USING_NS_CC;

Scene* MyPlaneScene::createScene()
{
	auto scene = Scene::create();
	auto layer = MyPlaneScene::create();
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool MyPlaneScene::init()
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

	gridSp = Sprite::create("grid.png");
	gridSp->setPosition(origin+Vec2(visibleSize.width / 2, visibleSize.height / 2));
	gridSp->setAnchorPoint(Point(0.5, 0.5));

	Size size_pai = gridSp->getContentSize();
	scale = (float)visibleSize.width * 0.8 / (float)size_pai.width;
	gridSp->setScale(scale, scale);
	this->addChild(gridSp);

	for(int i=0;i<MINE_GRID_SIZE;i++)
		for(int j=0;j<MINE_GRID_SIZE;j++)
			if (app->MINE_GRID_GLOBAL[i][j] > 0)
			{
				Sprite* p = Sprite::create("p.png");
				p->setAnchorPoint(Point(0,0));
				p->setPosition(j*gridSp->getContentSize().width / MINE_GRID_SIZE, (9 - i)*gridSp->getContentSize().width / MINE_GRID_SIZE);
				gridSp->addChild(p);
			}

	for (int i = 0; i<MINE_GRID_SIZE; i++)
		for (int j = 0; j < MINE_GRID_SIZE; j++)
			if (app->AIBO_GRID_GLOBAL[i][j] ==0x10 ||
				app->AIBO_GRID_GLOBAL[i][j] == 0x11 ||
				app->AIBO_GRID_GLOBAL[i][j] == 0x12 ||
				app->AIBO_GRID_GLOBAL[i][j] == 0x21)
			{
				Sprite* p;
				if (app->AIBO_GRID_GLOBAL[i][j] == 0x10)
					p = Sprite::create("hit.png");
				else if (app->AIBO_GRID_GLOBAL[i][j] == 0x11)
					p = Sprite::create("hurt.png");
				else if (app->AIBO_GRID_GLOBAL[i][j] == 0x12)
					p = Sprite::create("dead.png");
				else if (app->AIBO_GRID_GLOBAL[i][j] == 0x21)
				{
					p = Sprite::create("dead.png");
					p->setColor(Color3B::RED);
				}

				p->setAnchorPoint(Point(0, 0));
				p->setPosition(j*gridSp->getContentSize().width / MINE_GRID_SIZE, (9 - i)*gridSp->getContentSize().width / MINE_GRID_SIZE);
				gridSp->addChild(p);
			}

	auto backbutton = Button::create("back.png", "back.png", "back.png");
	backbutton->setPosition(origin+Vec2(0.06f*visibleSize.width, 0.95f*visibleSize.height));
	backbutton->setScale(scale, scale);
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

	return true;
}