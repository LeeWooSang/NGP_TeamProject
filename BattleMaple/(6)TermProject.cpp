#include <windows.h>
#include <stdlib.h>
#include <atlImage.h>
#include <mmstream.h>

#define CLIENT_WIDTH   1024
#define CLIENT_HEIGHT   740
#define OBSTACLES_NUM   100
#define SKILL_NUM      10

#define PREDEATH 11
#define DEATH 10
#define RUN 1
#define ATTACK 2
#define JUMP 3
#define SLIDE 4                           //mode 변수에 쓰일 매크로. ex) mode = RUN;
#define ALIVE 5
#define ALERT 6
#define HIT   7
#define BOOMSKILL 20
#define SPEARSKILL 21

HINSTANCE g_hInst;
TCHAR lpszClass[] = TEXT("Bitmap");

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DIALOG1Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{

	HWND hwnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	if (!hPrevInstance) {
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hInstance = hInstance;
		WndClass.lpfnWndProc = (WNDPROC)WndProc;
		WndClass.lpszClassName = lpszClass;
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&WndClass);
	}

	RECT rcWindow{};
	GetWindowRect(GetDesktopWindow(), &rcWindow);
	RECT rcClient{ 0,0,CLIENT_WIDTH,CLIENT_HEIGHT };
	AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, false);

	rcClient.right -= rcClient.left;
	rcClient.bottom -= rcClient.top;
	rcClient.left = rcClient.top = 0;

	POINT Start{ (rcWindow.right - rcClient.right) / 2
		,(rcWindow.bottom - rcClient.bottom) / 2 };

	hwnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		Start.x, Start.y, rcClient.right, rcClient.bottom, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}


enum Level { Easy = 5, Hard = 10 };
enum ObsType { Hero = 0, Mon, Boom, Stone, Drink, Skill, Boss };

struct BACKGROUND {
	int x;         //오브젝트의 x축 위치
	int y;         //오브젝트의 y축 위치
	int w;         //오브젝트 이미지의 가로길이
	int h;         //오브젝트 이미지의 세로길이
};

struct POS {
	int x;         //오브젝트의 x축 위치
	int y;         //오브젝트의 y축 위치
	int w;         //오브젝트 이미지의 가로길이
	int h;         //오브젝트 이미지의 세로길이
	int mode;      //오브젝트의 상태(죽음, 타격, 공격 등)   //장애물의 경우 DEATH와 ALIVE, HIT사용
	int type;      // 오브젝트 종류
};

struct SKILL {
	int x;
	int y;
	int w;
	int h;
	int on;
};

BOOL InRect(const POS& m1, const POS& m2, const int type)
{
	int cns = 0;
	if (type == ObsType::Boom || ObsType::Mon) cns = 15;
	else if (type == ObsType::Stone) cns = 10;
	else cns = 0;
	if (m1.x + m1.w >= m2.x + cns && m1.y + m1.h >= m2.y
		&& m1.x <= m2.x + m2.w - cns && m1.y <= m2.y + m2.h)
		return TRUE;
	else
		return FALSE;
}

BOOL InRectBoss(const POS& c, const SKILL& m)
{
	if (m.x <= c.x && m.x + m.w >= c.x + c.w)
		return TRUE;
	else
		return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HDC memdc;
	HWND hDlg = NULL;
	static HBRUSH hBrush, oldBrush;
	static HPEN hPen, oldPen;
	static CImage png, png_view, png_ground, png_start, png_chat;
	static CImage png_walk[4], png_jump, png_slide, png_attack[3], png_alert[3], png_walk_b[4], png_jump_b, png_slide_b, png_attack_b[3], png_alert_b[3]
		, png_fireball[4], png_mon[6], png_stone, png_boom, png_death[11], png_hit_up, png_hit_down, png_drink, png_crush[6], png_gameover[10], png_clear[21], png_cleartext[10]
		, png_bossalert[5], png_bossdeath[7], png_bossboomskill[15], png_bossspearskill[15], png_boomskill[6], png_spearskill[8];
	static RECT RectView, EasyPortal, HardPortal, Help;
	static POS Hero, obs[OBSTACLES_NUM], Skill[SKILL_NUM];
	static SKILL FireBall, BossBoom[2], BossSpear;
	static BACKGROUND ViewPos, GroundPos, BackGroundPos;
	static int WalkCount, DeathCount, AttackCount, AlertCount, MonCount, HP, PSkillCount, CrushCount, GameOverCount, ClearCount, ClearTextCount
		, BossAlertCount, BossDeathCount, BossBoomSkillCount, BossSpearSkillCount, SkillBoomCount, SkillSpearCount, BossHP;
	static BOOL hitup, hitdown, back, crush;               //back은 캐릭터가 뒤돌았다는 신호, crush는 스킬이 몬스터에게 맞았다는 신호
	static BOOL isMain = FALSE;
	static BOOL isBoss = FALSE;
	static BOOL isMidair = FALSE, midair_ready = FALSE;
	static BOOL isCheat = FALSE;
	static BOOL DlgOn = FALSE;
	static BOOL clear = FALSE, cleartext = FALSE;
	static POINT m;
	static int BossSkill;

	static int jumpSpd = 47, slideSpd = 8, attackCount = 0, attackSpd = 50;
	static int level = Level::Easy;      // 난이도 Easy/Hard
	static int max_obsNum = 0;   // 최대 장애물 개수
	static FILE* fp;
	static bool loadFail = false;

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hwnd, &RectView);
		png.Create(RectView.right, RectView.bottom, 24);

		PlaySound("추억의 길.wav", NULL, SND_ASYNC | SND_LOOP);

		png_view.Load(TEXT("view.png"));
		png_ground.Load(TEXT("ground.png"));
		png_start.Load(TEXT("start.png"));
		png_chat.Load(TEXT("말풍선.png"));

		//캐릭터 애니메이션
		png_walk[0].Load(TEXT("walk-1-1.png"));
		png_walk[1].Load(TEXT("walk-2-1.png"));
		png_walk[2].Load(TEXT("walk-3-1.png"));
		png_walk[3].Load(TEXT("walk-4-1.png"));

		png_walk_b[0].Load(TEXT("walk-1-b.png"));
		png_walk_b[1].Load(TEXT("walk-2-b.png"));
		png_walk_b[2].Load(TEXT("walk-3-b.png"));
		png_walk_b[3].Load(TEXT("walk-4-b.png"));

		png_attack[0].Load(TEXT("attack-1.png"));
		png_attack[1].Load(TEXT("attack-2.png"));
		png_attack[2].Load(TEXT("attack-3.png"));

		png_attack_b[0].Load(TEXT("attack-1-b.png"));
		png_attack_b[1].Load(TEXT("attack-2-b.png"));
		png_attack_b[2].Load(TEXT("attack-3-b.png"));

		png_alert[0].Load(TEXT("alert-1.png"));
		png_alert[1].Load(TEXT("alert-2.png"));
		png_alert[2].Load(TEXT("alert-3.png"));

		png_alert_b[0].Load(TEXT("alert-1-b.png"));
		png_alert_b[1].Load(TEXT("alert-2-b.png"));
		png_alert_b[2].Load(TEXT("alert-3-b.png"));

		png_jump.Load(TEXT("jump-1-1.png"));
		png_jump_b.Load(TEXT("jump-1-b.png"));

		png_slide.Load(TEXT("slide-1-1.png"));
		png_slide_b.Load(TEXT("slide-1-b.png"));

		png_death[0].Load(TEXT("death-1.png"));
		png_death[1].Load(TEXT("death-2.png"));
		png_death[2].Load(TEXT("death-3.png"));
		png_death[3].Load(TEXT("death-4.png"));
		png_death[4].Load(TEXT("death-5.png"));
		png_death[5].Load(TEXT("death-6.png"));
		png_death[6].Load(TEXT("death-7.png"));
		png_death[7].Load(TEXT("death-8.png"));
		png_death[8].Load(TEXT("death-9.png"));
		png_death[9].Load(TEXT("death-10.png"));
		png_death[10].Load(TEXT("death-11.png"));

		//캐릭터 스킬
		png_fireball[0].Load(TEXT("fireball_01-1.png"));
		png_fireball[1].Load(TEXT("fireball_02-1.png"));
		png_fireball[2].Load(TEXT("fireball_03-1.png"));
		png_fireball[3].Load(TEXT("fireball_04-1.png"));

		//몬스터 애니메이션
		png_mon[0].Load(TEXT("mon_01.png"));
		png_mon[1].Load(TEXT("mon_02.png"));
		png_mon[2].Load(TEXT("mon_03.png"));
		png_mon[3].Load(TEXT("mon_04.png"));
		png_mon[4].Load(TEXT("mon_05.png"));
		png_mon[5].Load(TEXT("mon_06.png"));

		//돌무덤
		png_stone.Load(TEXT("stone.png"));

		//폭탄
		png_boom.Load(TEXT("boom.png"));

		//에너지드링크
		png_drink.Load(TEXT("redbull.png"));

		//HIT 이펙트
		png_hit_up.Load(TEXT("hit.png"));
		png_hit_down.Load(TEXT("hit-down.png"));

		//crush 이펙트
		png_crush[0].Load(TEXT("boom_01.png"));
		png_crush[1].Load(TEXT("boom_02.png"));
		png_crush[2].Load(TEXT("boom_03.png"));
		png_crush[3].Load(TEXT("boom_04.png"));
		png_crush[4].Load(TEXT("boom_05.png"));
		png_crush[5].Load(TEXT("boom_06.png"));

		//gameover 이펙트
		png_gameover[0].Load(TEXT("gameover_01.png"));
		png_gameover[1].Load(TEXT("gameover_02.png"));
		png_gameover[2].Load(TEXT("gameover_03.png"));
		png_gameover[3].Load(TEXT("gameover_04.png"));
		png_gameover[4].Load(TEXT("gameover_05.png"));
		png_gameover[5].Load(TEXT("gameover_06.png"));
		png_gameover[6].Load(TEXT("gameover_07.png"));
		png_gameover[7].Load(TEXT("gameover_08.png"));
		png_gameover[8].Load(TEXT("gameover_09.png"));
		png_gameover[9].Load(TEXT("gameover_10.png"));

		//clear이펙트
		png_clear[0].Load(TEXT("clear_01.png"));
		png_clear[1].Load(TEXT("clear_02.png"));
		png_clear[2].Load(TEXT("clear_03.png"));
		png_clear[3].Load(TEXT("clear_04.png"));
		png_clear[4].Load(TEXT("clear_05.png"));
		png_clear[5].Load(TEXT("clear_06.png"));
		png_clear[6].Load(TEXT("clear_07.png"));
		png_clear[7].Load(TEXT("clear_08.png"));
		png_clear[8].Load(TEXT("clear_09.png"));
		png_clear[9].Load(TEXT("clear_10.png"));
		png_clear[10].Load(TEXT("clear_11.png"));
		png_clear[11].Load(TEXT("clear_12.png"));
		png_clear[12].Load(TEXT("clear_13.png"));
		png_clear[13].Load(TEXT("clear_14.png"));
		png_clear[14].Load(TEXT("clear_15.png"));
		png_clear[15].Load(TEXT("clear_16.png"));
		png_clear[16].Load(TEXT("clear_17.png"));
		png_clear[17].Load(TEXT("clear_18.png"));
		png_clear[18].Load(TEXT("clear_19.png"));
		png_clear[19].Load(TEXT("clear_20.png"));
		png_clear[20].Load(TEXT("clear_21.png"));

		//boss
		png_bossalert[0].Load(TEXT("bossalert_01.png"));
		png_bossalert[1].Load(TEXT("bossalert_02.png"));
		png_bossalert[2].Load(TEXT("bossalert_03.png"));
		png_bossalert[3].Load(TEXT("bossalert_04.png"));
		png_bossalert[4].Load(TEXT("bossalert_05.png"));
		png_bossalert[5].Load(TEXT("bossalert_06.png"));

		png_bossdeath[0].Load(TEXT("bossdeath_01.png"));
		png_bossdeath[1].Load(TEXT("bossdeath_02.png"));
		png_bossdeath[2].Load(TEXT("bossdeath_03.png"));
		png_bossdeath[3].Load(TEXT("bossdeath_04.png"));
		png_bossdeath[4].Load(TEXT("bossdeath_05.png"));
		png_bossdeath[5].Load(TEXT("bossdeath_06.png"));
		png_bossdeath[6].Load(TEXT("bossdeath_07.png"));

		png_bossboomskill[0].Load(TEXT("bossboomskill_01.png"));
		png_bossboomskill[1].Load(TEXT("bossboomskill_02.png"));
		png_bossboomskill[2].Load(TEXT("bossboomskill_03.png"));
		png_bossboomskill[3].Load(TEXT("bossboomskill_04.png"));
		png_bossboomskill[4].Load(TEXT("bossboomskill_05.png"));
		png_bossboomskill[5].Load(TEXT("bossboomskill_06.png"));
		png_bossboomskill[6].Load(TEXT("bossboomskill_07.png"));
		png_bossboomskill[7].Load(TEXT("bossboomskill_08.png"));
		png_bossboomskill[8].Load(TEXT("bossboomskill_09.png"));
		png_bossboomskill[9].Load(TEXT("bossboomskill_10.png"));
		png_bossboomskill[10].Load(TEXT("bossboomskill_11.png"));
		png_bossboomskill[11].Load(TEXT("bossboomskill_12.png"));
		png_bossboomskill[12].Load(TEXT("bossboomskill_13.png"));
		png_bossboomskill[13].Load(TEXT("bossboomskill_14.png"));
		png_bossboomskill[14].Load(TEXT("bossboomskill_15.png"));

		png_boomskill[0].Load(TEXT("fire_01.png"));
		png_boomskill[1].Load(TEXT("fire_02.png"));
		png_boomskill[2].Load(TEXT("fire_03.png"));
		png_boomskill[3].Load(TEXT("fire_04.png"));
		png_boomskill[4].Load(TEXT("fire_05.png"));
		png_boomskill[5].Load(TEXT("fire_06.png"));

		png_bossspearskill[0].Load(TEXT("bossspearskill_01.png"));
		png_bossspearskill[1].Load(TEXT("bossspearskill_02.png"));
		png_bossspearskill[2].Load(TEXT("bossspearskill_03.png"));
		png_bossspearskill[3].Load(TEXT("bossspearskill_04.png"));
		png_bossspearskill[4].Load(TEXT("bossspearskill_05.png"));
		png_bossspearskill[5].Load(TEXT("bossspearskill_06.png"));
		png_bossspearskill[6].Load(TEXT("bossspearskill_07.png"));
		png_bossspearskill[7].Load(TEXT("bossspearskill_08.png"));
		png_bossspearskill[8].Load(TEXT("bossspearskill_09.png"));
		png_bossspearskill[9].Load(TEXT("bossspearskill_10.png"));
		png_bossspearskill[10].Load(TEXT("bossspearskill_11.png"));
		png_bossspearskill[11].Load(TEXT("bossspearskill_12.png"));
		png_bossspearskill[12].Load(TEXT("bossspearskill_13.png"));
		png_bossspearskill[13].Load(TEXT("bossspearskill_14.png"));
		png_bossspearskill[14].Load(TEXT("bossspearskill_15.png"));

		png_spearskill[0].Load(TEXT("spear_01.png"));
		png_spearskill[1].Load(TEXT("spear_02.png"));
		png_spearskill[2].Load(TEXT("spear_03.png"));
		png_spearskill[3].Load(TEXT("spear_04.png"));
		png_spearskill[4].Load(TEXT("spear_05.png"));
		png_spearskill[5].Load(TEXT("spear_06.png"));
		png_spearskill[6].Load(TEXT("spear_07.png"));
		png_spearskill[7].Load(TEXT("spear_08.png"));

		png_cleartext[0].Load(TEXT("cleartext_01.png"));
		png_cleartext[1].Load(TEXT("cleartext_02.png"));
		png_cleartext[2].Load(TEXT("cleartext_03.png"));
		png_cleartext[3].Load(TEXT("cleartext_04.png"));
		png_cleartext[4].Load(TEXT("cleartext_05.png"));
		png_cleartext[5].Load(TEXT("cleartext_06.png"));
		png_cleartext[6].Load(TEXT("cleartext_07.png"));
		png_cleartext[7].Load(TEXT("cleartext_08.png"));
		png_cleartext[8].Load(TEXT("cleartext_09.png"));
		png_cleartext[9].Load(TEXT("cleartext_10.png"));

		//배경 초기화
		ViewPos.x = 0;
		ViewPos.y = 0;
		ViewPos.w = 4400;
		ViewPos.h = 597;

		GroundPos.x = 0;
		GroundPos.y = 598;
		GroundPos.w = 4400;
		GroundPos.h = 160;
		BackGroundPos.x = 4401;
		BackGroundPos.y = 598;
		BackGroundPos.w = 4400;
		BackGroundPos.h = 160;

		// 캐릭터 초기화
		Hero.w = 39;
		Hero.h = 69;
		Hero.x = 100;
		Hero.y = GroundPos.y - Hero.h;
		Hero.mode = RUN;
		Hero.type = ObsType::Hero;

		FireBall.x = Hero.x + Hero.w + 10;
		FireBall.y = Hero.y;
		FireBall.w = 133;
		FireBall.h = 46;
		FireBall.on = FALSE;

		//스킬
		for (int i = 0; i < SKILL_NUM; ++i) {
			Skill[i].w = 133;
			Skill[i].h = 46;
			Skill[i].x = 100;
			Skill[i].y = GroundPos.y - Skill[i].h;
			Skill[i].mode = DEATH;
			Skill[i].type = ObsType::Skill;
		}

		EasyPortal.left = 670;
		EasyPortal.top = 460;
		EasyPortal.right = 790;
		EasyPortal.bottom = 600;

		HardPortal.left = 870;
		HardPortal.top = 460;
		HardPortal.right = 990;
		HardPortal.bottom = 600;

		Help.left = 0;
		Help.top = 615;
		Help.right = 160;
		Help.bottom = RectView.bottom;


		isMain = TRUE;
		Hero.mode = ALERT;
		SetTimer(hwnd, 2, 100, NULL);
		break;
	case WM_LBUTTONDOWN:
		m.x = LOWORD(lParam);
		m.y = HIWORD(lParam);

		if (Hero.mode == DEATH || cleartext)
		{
			png.Destroy();
			PostQuitMessage(0);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		}
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			//배경이동
			if (ViewPos.x >= -3376)
			{
				if (level == Level::Easy)
				{
					ViewPos.x -= 5;
					GroundPos.x -= 20;
					BackGroundPos.x -= 20;
					//장애물 이동
					for (int i = 0; i < max_obsNum; ++i) {
						obs[i].x -= 20;
					}
				}
				else if (level == Level::Hard)
				{
					ViewPos.x -= 6;
					GroundPos.x -= 25;
					BackGroundPos.x -= 25;
					//장애물 이동
					for (int i = 0; i < max_obsNum; ++i) {
						obs[i].x -= 25;
					}

				}
				if (BackGroundPos.x <= 0)
				{
					GroundPos.x = 0;
					BackGroundPos.x = 4401;
				}
				if (ViewPos.x <= -2100) {
					if (!isMidair && !isBoss) {
						midair_ready = TRUE;
						SetTimer(hwnd, 8, 10, NULL);
					}
					isMidair = TRUE;
					Hero.mode = JUMP;
				}
			}
			else
			{
				if (!isBoss) {
					SetTimer(hwnd, 7, 3000, NULL);
					Hero.y = GroundPos.y - Hero.h;
				}
				KillTimer(hwnd, 5);
				KillTimer(hwnd, 6);
				isBoss = TRUE;
				isMidair = FALSE;
			}

			// 캐릭터 충돌
			hitup = FALSE;
			hitdown = FALSE;
			for (int i = 0; i < max_obsNum; ++i) {
				if (InRect(Hero, obs[i], obs[i].type) && !isCheat&&obs[i].mode != DEATH) {
					// 에너지드링크
					if (obs[i].type == ObsType::Drink) {
						hitdown = TRUE;
						obs[i].mode = DEATH;
						HP -= 10;
						if (HP <= 0)
							HP = 0;
					}
					//장애물
					else
					{
						if ((obs[i].type == ObsType::Boom&& Hero.mode == SLIDE)
							|| (obs[i].mode == HIT&&obs[i].type == ObsType::Mon)) break;
						hitup = TRUE;
						HP += 5;
					}
					break;
				}
			}

			if (isBoss)
			{
				if (InRectBoss(Hero, BossBoom[0]) && !isCheat&&BossBoom[0].on)
				{
					hitup = TRUE;
					HP += 2;
				}

				if (InRectBoss(Hero, BossBoom[1]) && !isCheat&&BossBoom[1].on)
				{
					hitup = TRUE;
					HP += 2;
				}

				if (InRectBoss(Hero, BossSpear) && !isCheat&&BossSpear.on)
				{
					hitup = TRUE;
					HP += 2;
				}
			}

			// 캐릭터 점프
			if (!isMidair&&Hero.mode == JUMP) {
				if (jumpSpd > -47) {
					jumpSpd -= 9;
					Hero.y -= jumpSpd;
				}
				if (Hero.y + Hero.h >= 598) {
					Hero.y = 598 - Hero.h;
					Hero.mode = RUN;
					if (isBoss)
						Hero.mode = ALERT;
					jumpSpd = 47;
				}
			}


			// 캐릭터 공격
			if (!back && Hero.mode == ATTACK) {
				attackCount += 1;
				if (attackCount > 2) {
					attackCount = 0;
					Hero.mode = RUN;
				}
				if (attackCount == 1) {
					for (int i = 0; i < SKILL_NUM; ++i) {
						if (Skill[i].mode == DEATH) {
							Skill[i].mode = ALIVE;
							Skill[i].x = Hero.x - 100;
							break;
						}
					}
				}
			}

			//스킬 이동
			for (int i = 0; i < SKILL_NUM; ++i) {
				if (Skill[i].mode == ALIVE) {
					Skill[i].x += attackSpd;
					if (Skill[i].x >= RectView.right) {
						Skill[i].mode = DEATH;
					}
				}
			}

			//스킬 충돌
			for (int i = 0; i < max_obsNum; ++i) {
				for (int j = 0; j < SKILL_NUM; ++j) {
					if (InRect(Skill[j], obs[i], obs[i].type)
						&& obs[i].mode != DEATH
						&& Skill[j].mode == ALIVE)
					{
						if (obs[i].type == ObsType::Mon || obs[i].type == ObsType::Stone) {
							obs[i].mode = HIT;
							Skill[j].mode = DEATH;
						}
						if (obs[i].type == ObsType::Boss)
						{
							crush = TRUE;
							BossHP -= 3;
							if (BossHP <= 0)
							{
								clear = TRUE;
								BossHP = 0;
								KillTimer(hwnd, 7);
								BossBoom[0].on = FALSE;
								BossBoom[1].on = FALSE;
								BossSpear.on = FALSE;
								obs[i].mode = PREDEATH;
								Hero.mode = ALERT;
							}
						}
					}
				}
			}
			break;
		case 2:
			break;
		case 3:
			if (Hero.x + Hero.w >= RectView.right)
				Hero.x = RectView.right - Hero.w;
			Hero.x += 3;
			break;
		case 4:
			if (Hero.x <= 0)
				Hero.x = 0;
			Hero.x -= 3;
			break;
		case 5:
			if (Hero.y + Hero.h >= RectView.bottom)
				Hero.y = RectView.bottom - Hero.h;
			Hero.y += 5;
			break;
		case 6:
			if (Hero.y <= 0)
				Hero.y = 0;
			Hero.y -= 5;
			break;
		case 7:
			BossSkill = rand() % 2;
			if (BossSkill == 0)
			{
				BossBoom[0].on = TRUE;
				BossBoom[1].on = TRUE;
				if (level == Level::Easy)
				{
					obs[66].mode = BOOMSKILL;
					BossBoom[0].x = 0;
					BossBoom[0].y = GroundPos.y - BossBoom[0].h - 20;
					BossBoom[1].x = 467;
					BossBoom[1].y = GroundPos.y - BossBoom[1].h - 20;
				}
				else if (level == Level::Hard)
				{
					obs[61].mode = BOOMSKILL;
					BossBoom[0].x = 0;
					BossBoom[0].y = GroundPos.y - BossBoom[0].h - 20;
					BossBoom[1].x = 467;
					BossBoom[1].y = GroundPos.y - BossBoom[1].h - 20;
				}
			}
			else if (BossSkill == 1)
			{
				BossSpear.on = TRUE;
				if (level == Level::Easy)
				{
					obs[66].mode = SPEARSKILL;
					BossSpear.x = obs[66].x - BossSpear.w;
					BossSpear.y = GroundPos.y - BossSpear.h;
				}
				else if (level == Level::Hard)
				{
					obs[61].mode = SPEARSKILL;
					BossSpear.x = obs[61].x - BossSpear.w;
					BossSpear.y = GroundPos.y - BossSpear.h;
				}
			}
			break;
		case 8:
			if (Hero.y <= 300 && midair_ready == TRUE) {
				KillTimer(hwnd, 8);
				midair_ready = FALSE;
				break;
			}
			Hero.y -= 5;
			break;
		}

		if (HP >= 100)
		{
			HP = 100;
			hitup = FALSE;
			hitdown = FALSE;
			KillTimer(hwnd, 1);
			SetTimer(hwnd, 2, 100, NULL);
			Hero.mode = DEATH;
		}
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_RIGHT:
			if ((isBoss || isMain) && Hero.mode != JUMP && Hero.mode != SLIDE) {
				Hero.mode = RUN;
				back = FALSE;
				KillTimer(hwnd, 4);
				SetTimer(hwnd, 3, 10, NULL);
				if (Hero.x + Hero.w >= RectView.right) Hero.x = RectView.right - Hero.w;
			}
			break;
		case VK_LEFT:
			if ((isBoss || isMain) && Hero.mode != JUMP && Hero.mode != SLIDE) {
				Hero.mode = RUN;
				back = TRUE;
				KillTimer(hwnd, 3);
				SetTimer(hwnd, 4, 10, NULL);
				if (Hero.x <= RectView.left) Hero.x = RectView.left;
			}
			break;
		case VK_UP:
			if (isMain)
			{
				if (Hero.x >= EasyPortal.left && Hero.x + Hero.w <= EasyPortal.right)
				{
					level = Level::Easy;
					SetTimer(hwnd, 1, 100, NULL);
					BossHP = 200;

					// 오브젝트 초기화
					//  - 파일 읽기(장애물 타입, x좌표)
					fp = fopen("EasyData.txt", "r");

					if (fp == NULL) {
						loadFail = true;
						return 0;
					}
					for (int i = 0; i < OBSTACLES_NUM; ++i) {
						fscanf(fp, "%d %d", &obs[i].type, &obs[i].x);
						if (obs[i].type != 0)   max_obsNum++;
					}
					fclose(fp);
				}
				else if (Hero.x >= HardPortal.left && Hero.x + Hero.w <= HardPortal.right)
				{
					level = Level::Hard;
					SetTimer(hwnd, 1, 100, NULL);
					BossHP = 400;

					// 오브젝트 초기화
					//  - 파일 읽기(장애물 타입, x좌표)
					fp = fopen("HardData.txt", "r");

					if (fp == NULL) {
						loadFail = true;
						return 0;
					}
					for (int i = 0; i < OBSTACLES_NUM; ++i) {
						fscanf(fp, "%d %d", &obs[i].type, &obs[i].x);
						if (obs[i].type != 0)   max_obsNum++;
					}
					fclose(fp);
				}
				else
					break;
				KillTimer(hwnd, 2);
				KillTimer(hwnd, 3);
				KillTimer(hwnd, 4);
				isMain = FALSE;
				back = FALSE;

				EasyPortal.left = 0;
				EasyPortal.top = 0;
				EasyPortal.right = 0;
				EasyPortal.bottom = 0;
				HardPortal.left = 0;
				HardPortal.top = 0;
				HardPortal.right = 0;
				HardPortal.bottom = 0;
				Help.left = 0;
				Help.top = 0;
				Help.right = 0;
				Help.bottom = 0;

				Hero.w = 39;
				Hero.h = 69;
				Hero.x = 100;
				Hero.y = GroundPos.y - Hero.h;
				Hero.mode = RUN;
				Hero.type = ObsType::Hero;

				// - 나머지 초기화
				for (int i = 0; i < max_obsNum; ++i) {
					if (obs[i].type == Mon) {
						obs[i].w = 78;
						obs[i].h = 60;
						obs[i].y = GroundPos.y - obs[i].h;
					}
					else if (obs[i].type == Boom) {
						obs[i].w = 89;
						obs[i].h = 100;
						if (obs[i].x < 9000)
							obs[i].y = GroundPos.y - obs[i].h - 50;
					}
					else if (obs[i].type == Stone) {
						obs[i].w = 65;
						obs[i].h = 54;
						obs[i].y = GroundPos.y - obs[i].h;
					}
					else if (obs[i].type == Drink) {
						obs[i].w = 23;
						obs[i].h = 60;
						obs[i].y = GroundPos.y - obs[i].h;
					}
					obs[i].mode = ALIVE;
				}
				//MidAir 데이터 읽기
				fp = fopen("MidAirDataE.txt", "r");
				if (fp == NULL) {
					loadFail = true;
					return 0;
				}
				if (level == Level::Hard) {
					fclose(fp);
					fp = fopen("MidAirDataH.txt", "r");
					if (fp == NULL) {
						loadFail = true;
						return 0;
					}
				}
				for (int i = max_obsNum; i < OBSTACLES_NUM; ++i) {
					fscanf(fp, "%d %d %d", &obs[i].type, &obs[i].x, &obs[i].y);
					if (obs[i].type != 0)   max_obsNum++;
					obs[i].w = 89;
					obs[i].h = 100;
					if (obs[i].type == ObsType::Drink) {
						obs[i].w = 23;
						obs[i].h = 60;
					}
					obs[i].mode = ALIVE;
					//보스 초기화
					if (obs[i].type == ObsType::Boss) {
						obs[i].w = 247;
						obs[i].h = 293;
						obs[i].mode = ALIVE;

						BossBoom[0].w = 192;
						BossBoom[0].h = 233;

						BossBoom[1].w = 192;
						BossBoom[1].h = 233;
						
						BossSpear.w = 579;
						BossSpear.h = 181;
					}
				}
				fclose(fp);
			}
			else if (!isMain)
			{
				if (isMidair) {
					KillTimer(hwnd, 5);
					SetTimer(hwnd, 6, 10, NULL);
					if (Hero.y <= RectView.top) Hero.y = RectView.top;
				}
				else if (Hero.mode != SLIDE) {
					Hero.mode = JUMP;
				}
			}
			break;
		case VK_DOWN:
			if (!isMain)
			{
				if (isMidair) {
					KillTimer(hwnd, 6);
					SetTimer(hwnd, 5, 10, NULL);
					if (Hero.y + Hero.h >= GroundPos.y) Hero.y = GroundPos.y - Hero.h;
				}
				else if (Hero.mode != JUMP) {
					Hero.mode = SLIDE;
				}
			}
			break;
		case VK_SPACE:
			if (!isMidair && (Hero.mode == RUN || Hero.mode == ALERT) && !back)
				Hero.mode = ATTACK;
			break;

		}

		InvalidateRect(hwnd, NULL, FALSE);
		break;
	case WM_KEYUP:
		if (wParam == VK_DOWN && !isMidair) {
			Hero.y = GroundPos.y - Hero.h;
			Hero.mode = RUN;
		}

		if (isMain || isBoss)
		{
			KillTimer(hwnd, 3);
			KillTimer(hwnd, 4);
			if (Hero.mode == RUN || Hero.mode==SLIDE)
			Hero.mode = ALERT;
		}

		if (Hero.mode == JUMP && isMidair)
		{
			KillTimer(hwnd, 5);
			KillTimer(hwnd, 6);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		if (loadFail)   TextOut(hdc, 50, 50, TEXT("File Open Fail."), 15);
		else {
			memdc = png.GetDC();

			if (isMain)
			{
				png_start.Draw(memdc, 0, 0, 1024, 720);
				png_chat.Draw(memdc, Hero.x - 20, Hero.y - 80, 144, 67);

				if (back)
				{
					if (Hero.mode == RUN)
					{
						if (WalkCount >= 4)
							WalkCount = 0;
						png_walk_b[WalkCount].Draw(memdc, Hero.x, Hero.y, Hero.w, Hero.h);
						WalkCount++;
					}
					else if (Hero.mode == ALERT)
					{
						if (AlertCount >= 3)
							AlertCount = 0;
						png_alert_b[AlertCount].Draw(memdc, Hero.x, Hero.y, 39, 68);
						AlertCount++;
					}
				}
				else
				{
					if (Hero.mode == RUN)
					{
						if (WalkCount >= 4)
							WalkCount = 0;
						png_walk[WalkCount].Draw(memdc, Hero.x, Hero.y, Hero.w, Hero.h);
						WalkCount++;
					}
					else if (Hero.mode == ALERT)
					{
						if (AlertCount >= 3)
							AlertCount = 0;
						png_alert[AlertCount].Draw(memdc, Hero.x, Hero.y, 39, 68);
						AlertCount++;
					}
				}
			}
			else if (!isMain)
			{
				png_view.Draw(memdc, ViewPos.x, ViewPos.y, ViewPos.w, ViewPos.h);
				png_ground.Draw(memdc, GroundPos.x, GroundPos.y, GroundPos.w, GroundPos.h);
				png_ground.Draw(memdc, BackGroundPos.x, BackGroundPos.y, BackGroundPos.w, BackGroundPos.h);

				//장애물 및 몬스터 애니메이션
				for (int i = 0; i < max_obsNum; ++i)
				{
					if (obs[i].mode != DEATH)
					{
						if (obs[i].type == ObsType::Boom)
							png_boom.Draw(memdc, obs[i].x, obs[i].y, 89, 100);
						else if (obs[i].type == ObsType::Stone) {
							png_stone.Draw(memdc, obs[i].x, obs[i].y, 65, 53);
							if (obs[i].mode == HIT)
							{
								if (CrushCount >= 6)
								{
									obs[i].mode = ALIVE;
									CrushCount = 0;
								}
								png_crush[CrushCount].Draw(memdc, obs[i].x - 10, obs[i].y - 20, 100, 100);
								CrushCount++;
							}
						}
						else if (obs[i].type == ObsType::Mon)
						{
							if (MonCount >= 6)
								MonCount = 0;
							png_mon[MonCount].Draw(memdc, obs[i].x, obs[i].y, 78, 60);
							MonCount++;

							if (obs[i].mode == HIT)
							{
								if (CrushCount >= 6)
								{
									obs[i].mode = DEATH;
									CrushCount = 0;
								}
								png_crush[CrushCount].Draw(memdc, obs[i].x - 10, obs[i].y - 20, 100, 100);
								CrushCount++;
							}
						}
						else if (obs[i].type == ObsType::Drink)
							png_drink.Draw(memdc, obs[i].x, obs[i].y, 23, 60);
						else if (obs[i].type == ObsType::Boss) {
								if (obs[i].mode == ALIVE)
								{
									if (BossAlertCount >= 5)
										BossAlertCount = 0;
									png_bossalert[BossAlertCount].Draw(memdc, obs[i].x, obs[i].y, obs[i].w, obs[i].h);
									BossAlertCount++;
								}
								else if (obs[i].mode == PREDEATH)
								{
									if (BossDeathCount >= 7)
									{
										obs[i].mode = DEATH;
										BossDeathCount = 0;
									}
									png_bossdeath[BossDeathCount].Draw(memdc, obs[i].x, obs[i].y, 304, 300);
									BossDeathCount++;
								}
								else if (obs[i].mode == BOOMSKILL)
								{
									if (SkillBoomCount >= 6)
									{
										SkillBoomCount = 0;
										obs[i].mode = ALIVE;
										BossBoom[0].on = FALSE;
										BossBoom[1].on = FALSE;
										BossBoomSkillCount = 0;
									}
									png_bossboomskill[BossBoomSkillCount].Draw(memdc, obs[i].x, obs[i].y - 50, 264, 398);

									png_boomskill[SkillBoomCount].Draw(memdc, BossBoom[0].x, BossBoom[0].y, BossBoom[0].w, BossBoom[0].h);
									png_boomskill[SkillBoomCount].Draw(memdc, BossBoom[1].x, BossBoom[1].y, BossBoom[1].w, BossBoom[1].h);
									BossBoomSkillCount++;
									if (BossBoomSkillCount % 2 == 1)
										SkillBoomCount++;
								}
								else if (obs[i].mode == SPEARSKILL)
								{
									if (BossSpearSkillCount >= 15)
									{
										obs[i].mode = ALIVE;
										BossSpear.on = FALSE;
										BossSpearSkillCount = 0;
										SkillSpearCount = 0;
									}
									png_bossspearskill[BossSpearSkillCount].Draw(memdc, obs[i].x, obs[i].y, 320, 376);

									png_spearskill[SkillSpearCount].Draw(memdc, BossSpear.x, BossSpear.y, BossSpear.w, BossSpear.h);
									BossSpearSkillCount++;
									if(BossSpearSkillCount%2 == 1)
										SkillSpearCount++;
								}

								if (crush)
								{
									if (CrushCount >= 6)
									{
										crush = FALSE;
										CrushCount = 0;
									}
									png_crush[CrushCount].Draw(memdc, obs[i].x + 10, obs[i].y + 100, 100, 100);
									CrushCount++;
								}

								if (BossHP > 50)
								{
									hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
									oldPen = (HPEN)SelectObject(memdc, hPen);
									hBrush = CreateSolidBrush(RGB(255, 255, 0));
									oldBrush = (HBRUSH)SelectObject(memdc, hBrush);
								}
								else if (BossHP <= 50)
								{
									hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
									oldPen = (HPEN)SelectObject(memdc, hPen);
									hBrush = CreateSolidBrush(RGB(255, 0, 0));
									oldBrush = (HBRUSH)SelectObject(memdc, hBrush);
								}
								if(level==Level::Easy)
									Rectangle(memdc, obs[i].x + 30, obs[i].y - 40, obs[i].x + 30 + BossHP, obs[i].y - 20);
								else
									Rectangle(memdc, obs[i].x + 30, obs[i].y - 40, obs[i].x + 30 + BossHP/2, obs[i].y - 20);

								SelectObject(memdc, oldPen);
								DeleteObject(hPen);
								SelectObject(memdc, oldBrush);
								DeleteObject(hBrush);
						}
					}
				}

				if (clear)
				{
					if (ClearCount >= 21)
					{
						clear = FALSE;
						ClearCount = 0;
						cleartext = TRUE;
					}
					png_clear[ClearCount].Draw(memdc, Hero.x - 200, GroundPos.y - 350 + 55, 470, 350);
					ClearCount++;
				}

				if (cleartext)
				{
					if (ClearTextCount >= 10)
					{
						ClearTextCount = 9;
					}
					png_cleartext[ClearTextCount].Draw(memdc, 0, 300, 1000, 500);
					ClearTextCount++;
				}
				// 캐릭터 애니메이션
				if (back)
				{
					if (Hero.mode == RUN)
					{
						if (WalkCount >= 4)
							WalkCount = 0;
						png_walk_b[WalkCount].Draw(memdc, Hero.x, Hero.y, Hero.w, Hero.h);
						WalkCount++;
					}
					else if (Hero.mode == JUMP)
					{
						png_jump_b.Draw(memdc, Hero.x, Hero.y, Hero.w, 66);
					}
					else if (Hero.mode == SLIDE)
					{
						png_slide_b.Draw(memdc, Hero.x, Hero.y + 27, 63, 44);
					}
					else if (Hero.mode == DEATH)
					{
						if (DeathCount >= 11)
							DeathCount = 0;
						png_death[DeathCount].Draw(memdc, Hero.x, Hero.y, 50, 75);
						DeathCount++;

						if (GameOverCount >= 10)
							GameOverCount = 9;
						png_gameover[GameOverCount].Draw(memdc, 0, -100, 1000, 500);
						GameOverCount++;
					}
					else if (Hero.mode == ATTACK)
					{
						if (AttackCount >= 3)
							AttackCount = 0;
						png_attack_b[AttackCount].Draw(memdc, Hero.x, Hero.y, 69, 63);
						AttackCount++;
					}
					else if (Hero.mode == ALERT)
					{
						if (AlertCount >= 3)
							AlertCount = 0;
						png_alert_b[AlertCount].Draw(memdc, Hero.x, Hero.y, 39, 68);
						AlertCount++;
					}
				}
				else
				{
					if (Hero.mode == RUN)
					{
						if (WalkCount >= 4)
							WalkCount = 0;
						png_walk[WalkCount].Draw(memdc, Hero.x, Hero.y, Hero.w, Hero.h);
						WalkCount++;
					}
					else if (Hero.mode == JUMP)
					{
						png_jump.Draw(memdc, Hero.x, Hero.y, Hero.w, 66);
					}
					else if (Hero.mode == SLIDE)
					{
						png_slide.Draw(memdc, Hero.x, Hero.y + 27, 63, 44);
					}
					else if (Hero.mode == DEATH)
					{
						if (DeathCount >= 11)
							DeathCount = 0;
						png_death[DeathCount].Draw(memdc, Hero.x, Hero.y, 50, 75);
						DeathCount++;

						if (GameOverCount >= 10)
							GameOverCount = 9;
						png_gameover[GameOverCount].Draw(memdc, 0, -100, 1000, 500);
						GameOverCount++;
					}
					else if (Hero.mode == ATTACK)
					{
						if (AttackCount >= 3)
							AttackCount = 0;
						png_attack[AttackCount].Draw(memdc, Hero.x, Hero.y, 69, 63);
						AttackCount++;
					}
					else if (Hero.mode == ALERT)
					{
						if (AlertCount >= 3)
							AlertCount = 0;
						png_alert[AlertCount].Draw(memdc, Hero.x, Hero.y, 39, 68);
						AlertCount++;
					}
				}

				hPen = CreatePen(PS_SOLID, 2, BLACK_PEN);
				oldPen = (HPEN)SelectObject(memdc, hPen);
				MoveToEx(memdc, Hero.x - 30, Hero.y - 40, NULL);
				LineTo(memdc, Hero.x + 70, Hero.y - 40);
				MoveToEx(memdc, Hero.x - 30, Hero.y - 40, NULL);
				LineTo(memdc, Hero.x - 30, Hero.y - 20);
				MoveToEx(memdc, Hero.x + 70, Hero.y - 40, NULL);
				LineTo(memdc, Hero.x + 70, Hero.y - 20);
				MoveToEx(memdc, Hero.x - 30, Hero.y - 20, NULL);
				LineTo(memdc, Hero.x + 70, Hero.y - 20);
				SelectObject(memdc, oldPen);
				DeleteObject(hPen);


				if (HP < 80)
				{
					hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
					oldPen = (HPEN)SelectObject(memdc, hPen);
					hBrush = CreateSolidBrush(RGB(255, 255, 0));
					oldBrush = (HBRUSH)SelectObject(memdc, hBrush);
				}
				else if (HP >= 80)
				{
					hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
					oldPen = (HPEN)SelectObject(memdc, hPen);
					hBrush = CreateSolidBrush(RGB(255, 0, 0));
					oldBrush = (HBRUSH)SelectObject(memdc, hBrush);
				}

				Rectangle(memdc, Hero.x - 30, Hero.y - 40, Hero.x - 30 + HP, Hero.y - 20);
				SelectObject(memdc, oldPen);
				DeleteObject(hPen);
				SelectObject(memdc, oldBrush);
				DeleteObject(hBrush);

				for (int i = 0; i < SKILL_NUM; ++i) {
					if (Skill[i].mode == ALIVE) {
						if (PSkillCount >= 3)
							PSkillCount = 0;
						png_fireball[PSkillCount].Draw(memdc, Skill[i].x, Skill[i].y, 133, 46);
						PSkillCount++;
					}
				}

				if (hitup)
				{
					png_hit_up.Draw(memdc, Hero.x, Hero.y - 40, 100, 30);
				}

				if (hitdown)
				{
					png_hit_down.Draw(memdc, Hero.x, Hero.y - 40, 100, 30);
				}
			}
			png.Draw(hdc, 0, 0);
			png.ReleaseDC();
		}
		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY:
		png.Destroy();
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMessage, wParam, lParam));
}