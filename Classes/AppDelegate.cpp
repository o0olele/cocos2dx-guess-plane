/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "AppDelegate.h"
#include "Global.h"
#include "HelloWorldScene.h"
#include "LoginScene.h"

// #define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(320, 480);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

void AppDelegate::filetest() {
	std::string writablePath = FileUtils::getInstance()->getWritablePath();
	std::string fileName = "writeStringTest.txt";

	// writeTest
	std::string writeDataStr = "10.60.102.252|20896";
	std::string fullPath = writablePath + fileName;

	bool isExist = false;

	auto sharedFileUtils = FileUtils::getInstance();
	isExist = sharedFileUtils->isFileExist(fullPath);

	if (!isExist)
	{
		if (FileUtils::getInstance()->writeStringToFile(writeDataStr, fullPath.c_str()))
		{
			log("see the plist file at %s", fullPath.c_str());
		}
		else
		{
			log("write plist file failed");
		}
	}

	// readTest
	std::string readDataStr = FileUtils::getInstance()->getStringFromFile(fullPath);
	int _left = readDataStr.find('|');
	_ip = readDataStr.substr(0, _left);
	_port = std::stoi(readDataStr.substr(_left + 1, readDataStr.length() - _left));
	log("----------------IP: %s, port:%d", _ip.c_str(), _port);
	//log("%s", readDataStr);
}

void AppDelegate::setPort(int p)
{
	_port = p;
}

void AppDelegate::setIp(std::string i)
{
	_ip = i;
}

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("angame", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("angame");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();

    // if the frame's height is larger than the height of medium size.
    if (frameSize.height >= mediumResolutionSize.height)
    {
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }
    register_all_packages();

    try{
        initConnect();
    }
    catch(char *str){

    }

    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}

void AppDelegate::loginEventDispatcher(u_int type)
{
	int a = type;
	_warn_type = GetAllUserIng;
	if (a == 2)
	{
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, LoginScene::createScene()));
	}
	else
	{
		EventCustom event("user_login_event");
		event.setUserData(static_cast<void*>(&a));
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
	}
}

void AppDelegate::addEventDispatcher(std::string new_user)
{
	int pos = new_user.find(_user_name.c_str());
	if (pos < 0)
		_online_member.append(new_user.c_str(), new_user.length());

	EventCustom event("user_add_event");
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void AppDelegate::subEventDispatcher()
{
	EventCustom event("user_add_event");
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void AppDelegate::regEventDispatcher(u_int type)
{
	int a = type;
	EventCustom event("user_reg_event");
	event.setUserData(static_cast<void*>(&a));
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void AppDelegate::changeKeyEventDispatcher(u_int)
{
}

void AppDelegate::getChallengeEventDispatcher(std::string user)
{
	_duel_user = user;
	_warn_type = GetChallengeIng;

	EventCustom event("get_challenge_event");
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void AppDelegate::inviteResultEventDispatcher(u_int type)
{
	int a = type;
	EventCustom event("invite_result_event");
	event.setUserData(static_cast<void*>(&a));
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void AppDelegate::gameStartEventDispatcher(u_int type, u_int mine, u_int other)
{
	IsStart = true;

	if (type == 0)
		_myturn = true;
	else
		_myturn = false;

	_my_score = mine;
	_enemy_score = other;

	EventCustom event("duel_event");
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void AppDelegate::duelEventDispatcher(u_int type, u_int _single, int _double)
{
	if (_myturn)
	{
		_pres_result = type;
		if (type == 0x21)
		{
			_double_true_h = _single;
			_double_true_t = _double;
		}
	}
	else
	{
		if (_single % 16 >= 0 && _single % 16 < 10 && _single / 16 >= 0 && _single / 16 < 10)
		{
			AIBO_GRID_GLOBAL[_single % 16][_single / 16] = type;
			if (_double > -1)
				AIBO_GRID_GLOBAL[_double % 16][_double / 16] = type;
		}
	}
}

void AppDelegate::gameResultEventDispatcher(u_int type)
{
	_warn_type = GameResult;
	_game_result = type;

	EventCustom event("game_result_event");
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void AppDelegate::initConnect()
{

   _client = SocketClient::construct();
   _client->onRecv = CC_CALLBACK_2(AppDelegate::onRecv, this);
   _client->onDisconnect = CC_CALLBACK_0(AppDelegate::onDisconnect, this);
   if (!_client->connectServer(_ip.c_str(), _port))
   {
   	    log("Client connect error");
   }
}

/*do with meassages*/
void AppDelegate::onRecv(const char* data, int count)
{
	u_int PackLength;
	char heart[4]={0x1f,0x00,0x04,0x00};
	DataBuffer.append(data, count);

	for (;;)
	{
		if (DataBuffer.length() < 4)
			break;

		if ((PackLength = (u_char)DataBuffer[2] + ((u_char)DataBuffer[3]) * 256) > DataBuffer.length())
			break;

		switch ((u_char)DataBuffer[0])
		{
		case 0x71:
			if (DataBuffer[1] < 0x03 || DataBuffer[1] == 0x04)
				loginEventDispatcher((u_int)DataBuffer[1]);
			else
				addEventDispatcher(DataBuffer.substr(4, PackLength - 4));
			break;
		case 0x72:
			if (DataBuffer[1] < 0x03)
				regEventDispatcher((u_int)DataBuffer[1]);
			else
				changeKeyEventDispatcher((u_int)DataBuffer[1]);
			break;
		case 0x73:
			if (DataBuffer[1] == 0x00)
				updateOnlineUser(DataBuffer.substr(4, PackLength - 4), 1);
			break;
		case 0x74:
			if (DataBuffer[1] == 0x00)
				getChallengeEventDispatcher(DataBuffer.substr(4, PackLength - 4));
			else
				inviteResultEventDispatcher((u_int)DataBuffer[1]);
			break;
		case 0x76:
			gameStartEventDispatcher((u_int)DataBuffer[1], (u_int)DataBuffer[4], (u_int)DataBuffer[5]);
			break;
		case 0x77:
			if (DataBuffer[1] < 20)
				duelEventDispatcher((u_int)DataBuffer[1], (u_int)DataBuffer[4], -1);
			else
				duelEventDispatcher((u_int)DataBuffer[1], (u_int)DataBuffer[4], (int)DataBuffer[5]);
			break;
		case 0x78:
			gameResultEventDispatcher((u_int)DataBuffer[1]);
			break;
		case 0x7e:
			updateOnlineUser(DataBuffer.substr(4, 8), 2);
			subEventDispatcher();
			break;//user offline
		case 0x7f:
			IsHeart = true;
		     _client->sendMessage(heart,4);
		     break;
		default:
			break;
		}

		if (DataBuffer.length() == PackLength)
		{
			DataBuffer = "";
			break;
		}
        else
		    DataBuffer = DataBuffer.substr(PackLength);
	}

}

void AppDelegate::onDisconnect()
{
	log("Client disconnect");
}

void AppDelegate::updateOnlineUser(std::string all, int type)
{
	_online_member = "";
	if (type == 1)
	{
		for (u_int i = 0; i < all.length() / 12; i++)
		{
			std::string sub = all.substr(i * 12, 12);
			int pos = sub.find(_user_name.substr(0,8).c_str());

			if (pos >= 0)
				_user_name = sub;
			else
				_online_member.append(sub.c_str(),sub.length());

		}
	}
	else
	{
		int t = _online_member.find(all.c_str());
		if (t >= 0)
		{
			_online_member.erase(t, 12);
		}
	}

	_warn_type = GetAllUserEd;
}

void AppDelegate::setGlobalGrid(int i, int j, int direct, int num)
{
	if (direct == 0 && i >= 0 && i + 3 <= 9 && j - 2 >= 0 && j + 2 <= 9)
	{
		MINE_GRID_GLOBAL[i][j] = num ? num + 1 : num;
		MINE_GRID_GLOBAL[i + 1][j - 2] = num; MINE_GRID_GLOBAL[i + 1][j - 1] = num; MINE_GRID_GLOBAL[i + 1][j] = num; MINE_GRID_GLOBAL[i + 1][j + 1] = num; MINE_GRID_GLOBAL[i + 1][j + 2] = num;
		MINE_GRID_GLOBAL[i + 2][j] = num;
		MINE_GRID_GLOBAL[i + 3][j - 1] = num; MINE_GRID_GLOBAL[i + 3][j] = num; MINE_GRID_GLOBAL[i + 3][j + 1] = num;
	}
	else if (direct == 1 && i - 2 >= 0 && i + 2 <= 9 && j - 3 >= 0 && j <= 9)
	{
		MINE_GRID_GLOBAL[i][j] = num ? num + 1 : num;
		MINE_GRID_GLOBAL[i - 2][j - 1] = num; MINE_GRID_GLOBAL[i - 1][j - 1] = num; MINE_GRID_GLOBAL[i][j - 1] = num; MINE_GRID_GLOBAL[i + 1][j - 1] = num; MINE_GRID_GLOBAL[i + 2][j - 1] = num;
		MINE_GRID_GLOBAL[i][j - 2] = num;
		MINE_GRID_GLOBAL[i - 1][j - 3] = num; MINE_GRID_GLOBAL[i][j - 3] = num; MINE_GRID_GLOBAL[i + 1][j - 3] = num;
	}
	else if (direct == 2 && i - 3 >= 0 && i <= 9 && j - 2 >= 0 && j + 2 <= 9)
	{
		MINE_GRID_GLOBAL[i][j] = num ? num + 1 : num;
		MINE_GRID_GLOBAL[i - 1][j - 2] = num; MINE_GRID_GLOBAL[i - 1][j - 1] = num; MINE_GRID_GLOBAL[i - 1][j] = num; MINE_GRID_GLOBAL[i - 1][j + 1] = num; MINE_GRID_GLOBAL[i - 1][j + 2] = num;
		MINE_GRID_GLOBAL[i - 2][j] = num;
		MINE_GRID_GLOBAL[i - 3][j - 1] = num; MINE_GRID_GLOBAL[i - 3][j] = num; MINE_GRID_GLOBAL[i - 3][j + 1] = num;
	}
	else if (direct == 3 && i - 2 >= 0 && i + 2 <= 9 && j >= 0 && j + 3 <= 9)
	{
		MINE_GRID_GLOBAL[i][j] = num ? num + 1 : num;
		MINE_GRID_GLOBAL[i - 2][j + 1] = num; MINE_GRID_GLOBAL[i - 1][j + 1] = num; MINE_GRID_GLOBAL[i][j + 1] = num; MINE_GRID_GLOBAL[i + 1][j + 1] = num; MINE_GRID_GLOBAL[i + 2][j + 1] = num;
		MINE_GRID_GLOBAL[i][j + 2] = num;
		MINE_GRID_GLOBAL[i - 1][j + 3] = num; MINE_GRID_GLOBAL[i][j + 3] = num; MINE_GRID_GLOBAL[i + 1][j + 3] = num;
	}
}

bool AppDelegate::isGridGlobalNull(int i, int j, int direct)
{
	if (direct == 0)
	{
		if (MINE_GRID_GLOBAL[i][j] ||
			MINE_GRID_GLOBAL[i + 1][j - 2] || MINE_GRID_GLOBAL[i + 1][j - 1] || MINE_GRID_GLOBAL[i + 1][j] || MINE_GRID_GLOBAL[i + 1][j + 1] || MINE_GRID_GLOBAL[i + 1][j + 2] ||
			MINE_GRID_GLOBAL[i + 2][j] ||
			MINE_GRID_GLOBAL[i + 3][j - 1] || MINE_GRID_GLOBAL[i + 3][j] || MINE_GRID_GLOBAL[i + 3][j + 1])
			return false;
	}
	else if (direct == 1)
	{
		if (MINE_GRID_GLOBAL[i][j] ||
			MINE_GRID_GLOBAL[i - 2][j - 1] || MINE_GRID_GLOBAL[i - 1][j - 1] || MINE_GRID_GLOBAL[i][j - 1] || MINE_GRID_GLOBAL[i + 1][j - 1] || MINE_GRID_GLOBAL[i + 2][j - 1] ||
			MINE_GRID_GLOBAL[i][j - 2] ||
			MINE_GRID_GLOBAL[i - 1][j - 3] || MINE_GRID_GLOBAL[i][j - 3] || MINE_GRID_GLOBAL[i + 1][j - 3])
			return false;
	}
	else if (direct == 2)
	{
		if (MINE_GRID_GLOBAL[i][j] ||
			MINE_GRID_GLOBAL[i - 1][j - 2] || MINE_GRID_GLOBAL[i - 1][j - 1] || MINE_GRID_GLOBAL[i - 1][j] || MINE_GRID_GLOBAL[i - 1][j + 1] || MINE_GRID_GLOBAL[i - 1][j + 2] ||
			MINE_GRID_GLOBAL[i - 2][j] ||
			MINE_GRID_GLOBAL[i - 3][j - 1] || MINE_GRID_GLOBAL[i - 3][j] || MINE_GRID_GLOBAL[i - 3][j + 1])
			return false;
	}
	else if (direct == 3)
	{
		if (MINE_GRID_GLOBAL[i][j] ||
			MINE_GRID_GLOBAL[i - 2][j + 1] || MINE_GRID_GLOBAL[i - 1][j + 1] || MINE_GRID_GLOBAL[i][j + 1] || MINE_GRID_GLOBAL[i + 1][j + 1] || MINE_GRID_GLOBAL[i + 2][j + 1] ||
			MINE_GRID_GLOBAL[i][j + 2] ||
			MINE_GRID_GLOBAL[i - 1][j + 3] || MINE_GRID_GLOBAL[i][j + 3] || MINE_GRID_GLOBAL[i + 1][j + 3])
			return false;
	}
	return true;
}

void AppDelegate::setWarnType(WarnType type)
{
	_warn_type = type;
}

void AppDelegate::setUsername(std::string name)
{
	this->_user_name = name;
}

void AppDelegate::setDuelUser(std::string u)
{
	_duel_user = u;
}

void AppDelegate::setPresResult(int r)
{
	_pres_result = r;
}

std::string AppDelegate::getDuelUser()
{
	return _duel_user;
}

std::string AppDelegate::getUser()
{
	return _user_name;
}

std::string AppDelegate::getOnlineUser()
{
	return _online_member;
}

int AppDelegate::getWarnType()
{
	return _warn_type;
}

int AppDelegate::getMyScore()
{
	return _my_score;
}

int AppDelegate::getEnemyScore()
{
	return _enemy_score;;
}

int AppDelegate::getResult()
{
	return _pres_result;
}

int AppDelegate::getDoubleHead()
{
	return _double_true_h;
}

int AppDelegate::getDoubleTail()
{
	return _double_true_t;
}

int AppDelegate::getGameResult()
{
	return _game_result;
}

bool AppDelegate::isMyTurn()
{
	return _myturn;
}

void AppDelegate::CleanAll()
{
	_user_name = "";
	_online_member = "";
	_duel_user = "";
	_warn_type = InitWarn;

	_my_score = 0;
	_enemy_score = 0;
	_pres_result = 0;
	_double_true_h = 0;
	_double_true_t = 0;

	_myturn = false;

	for(int i=0;i<10;i++)
		for (int j = 0; j < 10; j++)
		{
			MINE_GRID_GLOBAL[i][j] = 0;
			AIBO_GRID_GLOBAL[i][j] = 0;
		}

	if (_client)
		_client->destroy();

	IsHeart = false;
	IsStart = false;
	DataBuffer = "";
}
