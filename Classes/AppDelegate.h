
#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"
#include "SocketClient.h"

typedef enum tagWarnType
{
	InitWarn,
	GetAllUserIng,
	GetAllUserEd,
	ChooseUserIng,
	ChooseUserEd,
	WaitAcceptIng,
	WaitAcceptEd,
	InviteRefused,
	GetChallengeIng,
	GetChallengeEd,
	DecideFirstIng,
	DecideFirstEd,
	Surrender,
	GameResult
} WarnType;


/**
@brief    The cocos2d Application.

Private inheritance here hides part of interface from Director.
*/
class  AppDelegate : private cocos2d::Application
{
private:
	std::string _ip = "10.60.102.252";
	int _port = 20896;
	std::string _user_name = "";
	std::string _online_member = "";
	std::string _duel_user = "";
	WarnType _warn_type = InitWarn;

	int _my_score = 0;
	int _enemy_score = 0;
	int _pres_result = 0;
	int _double_true_h = 0;
	int _double_true_t = 0;

	bool _myturn = false;

	int _game_result;

public:
	int MINE_GRID_GLOBAL[10][10] = { 0 };
	int AIBO_GRID_GLOBAL[10][10] = { 0 };

    SocketClient * _client;
	bool IsHeart = false;
	bool IsStart = false;
	std::string DataBuffer = "";

    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

	/*
	setter & getter
	*/
	void setWarnType(WarnType type);
	void setUsername(std::string );
	void setDuelUser(std::string u="");
	void setPresResult(int);
	std::string getDuelUser();
	std::string getUser();
	std::string getOnlineUser();
	int getWarnType();
	int getMyScore();
	int getEnemyScore();
	int getResult();
	int getDoubleHead();
	int getDoubleTail();
	int getGameResult();
	bool isMyTurn();

	void CleanAll();

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  Called when the application moves to the background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  Called when the application reenters the foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();

	/*
	@brief  init socket connect
	*/
    void initConnect();

	/*
	@brief  deal with login messages
	@param  types of login messages
	*/
    void loginEventDispatcher(u_int);
    void addEventDispatcher(std::string);
	void subEventDispatcher();
	void regEventDispatcher(u_int);
	void changeKeyEventDispatcher(u_int);
	void getChallengeEventDispatcher(std::string);
	void inviteResultEventDispatcher(u_int);
	void gameStartEventDispatcher(u_int, u_int, u_int);
	void duelEventDispatcher(u_int, u_int, int);
	void gameResultEventDispatcher(u_int);

    void onRecv(const char* data, int count);
    void onDisconnect();

	/**
	@bref  update all online user info
	@para  info from socket message, update type:1->add;2->delete
	*/
	void updateOnlineUser(std::string all, int type);

	void setGlobalGrid(int i, int y, int direct, int num);
	bool isGridGlobalNull(int i, int y, int direct);

	void filetest();
	void setPort(int);
	void setIp(std::string);
};

#endif // _APP_DELEGATE_H_

