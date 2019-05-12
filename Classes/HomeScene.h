#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;
USING_NS_CC;

class HomeScene : public cocos2d::Layer, TableViewDataSource, TableViewDelegate
{
	float _cellwidth;
	float _cellheight;

public:
	EventListenerCustom * _listener;
	EventListenerCustom * _listener_ch;
	float scale;

	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(HomeScene);

public:
	virtual Size cellSizeForTable(TableView *table);
	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(TableView *table);

public:
	virtual void tableCellTouched(TableView* table, TableViewCell* cell);

	void checkHeart(float dt);

};