#include "PlaneSprite.h"
#include "Global.h"
#include "AppDelegate.h"
USING_NS_CC;

PlaneSprite::PlaneSprite(void)
{
}

PlaneSprite::~PlaneSprite(void)
{
}

Rect PlaneSprite::getRect()
{
	auto s = getTexture()->getContentSize();
	return Rect(-s.width / 2, -s.height / 2, s.width, s.height);
}

PlaneSprite* PlaneSprite::createWithTexture(Texture2D* aTexture)
{
	PlaneSprite* pPlaneSprite = new (std::nothrow) PlaneSprite();
	pPlaneSprite->initWithTexture(aTexture);
	pPlaneSprite->autorelease();

	return pPlaneSprite;
}

void PlaneSprite::setStartScale(float in)
{
	_start_scale = in;
}

void PlaneSprite::setStartPos(float x, float y)
{
	_pos_x = x;
	_pos_y = y;
}

void PlaneSprite::setGridPos(float x, float y, float s)
{
	_grid_x = x;
	_grid_y = y;
	_grid_size = s;
}

int PlaneSprite::getI()
{
	return _i;
}

int PlaneSprite::getJ()
{
	return _j;
}

int PlaneSprite::getItail()
{
	if (_direct == 0)
		return _i + 3;
	else if (_direct == 1)
		return _i;
	else if (_direct == 2)
		return _i - 3;
	else
		return _i;

	return 0;
}

int PlaneSprite::getJtail()
{
	if (_direct == 0)
		return _j;
	else if (_direct == 1)
		return _j - 3;
	else if (_direct == 2)
		return _j;
	else
		return _j + 3;

	return 0;
}

void PlaneSprite::setDirect(int d)
{
	_direct = d;
}

int PlaneSprite::getDirect()
{
	return _direct;
}

bool PlaneSprite::initWithTexture(Texture2D* aTexture)
{
	if (Sprite::initWithTexture(aTexture))
	{
		_state = kPaddleStateUngrabbed;
	}

	return true;
}

void PlaneSprite::onEnter()
{
	Sprite::onEnter();

	// Register Touch Event
	listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(PlaneSprite::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(PlaneSprite::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(PlaneSprite::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void PlaneSprite::onExit()
{
	//    auto director = Director::getInstance();
	//    director->getTouchDispatcher()->removeDelegate(this);
	Sprite::onExit();
}

bool PlaneSprite::containsTouchLocation(Touch* touch)
{
	return getRect().containsPoint(convertTouchToNodeSpaceAR(touch));
}

bool PlaneSprite::onTouchBegan(Touch* touch, Event* event)
{
	AppDelegate* app = (AppDelegate *)Application::getInstance();

	if (_state != kPaddleStateUngrabbed) return false;
	if (!containsTouchLocation(touch)) return false;
	if (_i >= 0 && _j >= 0)
	{
		app->setGlobalGrid(_i, _j, _direct, 0);
		_i = 0; _j = 0;
	}

	
	_state = kPaddleStateGrabbed;
	//CCLOG("return true");
	return true;
}

void PlaneSprite::onTouchMoved(Touch* touch, Event* event)
{
	CCASSERT(_state == kPaddleStateGrabbed, "Paddle - Unexpected state!");

	if (_move_cnt == 0)
	{
		setScale(_start_scale, _start_scale);
	}

	_move_cnt++;

	auto touchPoint = touch->getLocation();
	setPosition(Vec2(touchPoint.x, touchPoint.y));
}

PlaneSprite* PlaneSprite::clone() const
{
	PlaneSprite* ret = PlaneSprite::createWithTexture(_texture);
	ret->_state = _state;
	ret->setPosition(getPosition());
	ret->setAnchorPoint(getAnchorPoint());
	return ret;
}

void PlaneSprite::onTouchEnded(Touch* touch, Event* event)
{
	CCASSERT(_state == kPaddleStateGrabbed, "Paddle - Unexpected state!");

	if (_move_cnt > 0)
	{
		AppDelegate* app = (AppDelegate *)Application::getInstance();
		int cnt_x = (int)(getPosition().x - _grid_x) / (_grid_size / 10.0f);
		int cnt_y = (int)(getPosition().y - _grid_y) / (_grid_size / 10.0f);

		_i = _direct == 0 ? (MINE_GRID_SIZE - cnt_y) - 2 :
			_direct == 1 ? MINE_GRID_SIZE - cnt_y :
			_direct == 2 ? (MINE_GRID_SIZE - cnt_y) + 1 :
			(MINE_GRID_SIZE - cnt_y) - 1;
		_j = _direct == 0 ? (cnt_x - 1) + 1 :
			_direct == 1 ? (cnt_x - 1) + 2 :
			_direct == 2 ? (cnt_x - 1) :
			(cnt_x - 1) - 1;

		if (getBoundingBox().getMinX() > _grid_x &&
			getBoundingBox().getMaxX()<_grid_x + _grid_size + 20 &&
			getBoundingBox().getMinY()>_grid_y&&
			getBoundingBox().getMaxY() < _grid_y + _grid_size + 20&&
			app->isGridGlobalNull(_i,_j,_direct))
		{
			app->setGlobalGrid(_i, _j, _direct, 1);
			setPosition(Vec2(_grid_x+cnt_x* _grid_size / 10.0f, _grid_y + cnt_y * _grid_size / 10.0f));

			int a = getTag()*10+1;
			EventCustom event("game_custom_event");
			event.setUserData(static_cast<void*>(&a));
			Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
		}
		else
		{
			_i = 0; _j = 0;
			setPosition(Vec2(_pos_x, _pos_y));
			setScale(_start_scale / 2, _start_scale / 2);

			int a = getTag()*10+0;
			EventCustom event("game_custom_event");
			event.setUserData(static_cast<void*>(&a));
			Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
		}
		_move_cnt = 0;
	}

	_state = kPaddleStateUngrabbed;
}
