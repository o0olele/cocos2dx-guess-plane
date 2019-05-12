#include "cocos2d.h"
#include "ui\CocosGUI.h"

using namespace cocos2d::ui;
USING_NS_CC;

typedef enum tagPlaneState
{
	kPaddleStateGrabbed,
	kPaddleStateUngrabbed
} PlaneState;

class PlaneSprite : public cocos2d::Sprite, public cocos2d::Clonable
{
	EventListenerTouchOneByOne *listener;

	PlaneState        _state;
	
	int				  _move_cnt = 0;

	float			  _pos_x;
	float			  _pos_y;
	
	float			  _grid_x;
	float			  _grid_y;
	float			  _grid_size;

	int				  _i;
	int				  _j;
	int				  _direct = 0;

public:
	float 			  _start_scale;
    Vec2 origin;
	PlaneSprite(void);
	virtual ~PlaneSprite(void);

	cocos2d::Rect getRect();
	bool initWithTexture(cocos2d::Texture2D* aTexture) override;
	virtual void onEnter() override;
	virtual void onExit() override;
	bool containsTouchLocation(cocos2d::Touch* touch);
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual PlaneSprite* clone() const override;

	static PlaneSprite* createWithTexture(cocos2d::Texture2D* aTexture);

	void setStartScale(float in);
	void setStartPos(float x, float y);
	void setGridPos(float x, float y, float s);

	int getI();
	int getJ();

	int getItail();
	int getJtail();

	void setDirect(int);
	int getDirect();
};