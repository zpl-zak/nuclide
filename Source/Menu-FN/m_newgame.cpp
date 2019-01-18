/***
*
*	Copyright (c) 2016-2019 Marco 'eukara' Hladik. All rights reserved.
*
*	See the file LICENSE attached with the sources for usage details.
*
****/

CWidget fn_newgame;
CMainButton ng_btnEasy;
CMainButton ng_btnMedium;
CMainButton ng_btnDifficult;
CMainButton ng_btnCancel;

/* Button Callbacks */
void ng_btneasy_start(void)
{
	static void ng_btneasy_end(void) {
		g_menupage = PAGE_MAIN;
		localcmd("maxplayers 1\n");
		localcmd(sprintf("map %s\n", games[gameinfo_current].startmap));
	}
	localsound("../media/launch_upmenu1.wav");
	header.SetStartEndPos(45,45,70,208);
	header.SetStartEndSize(460,80,156,26);
	header.m_lerp = 0.0f;
	header.m_visible = TRUE;
	header.SetHeader(HEAD_NEWGAME);
	header.SetExecute(ng_btneasy_end);
}

void ng_btncancel_start(void)
{
	static void ng_btncancel_end(void) {
		g_menupage = PAGE_MAIN;
	}
	localsound("../media/launch_dnmenu1.wav");
	header.SetStartEndPos(45,45,70,208);
	header.SetStartEndSize(460,80,156,26);
	header.m_lerp = 0.0f;
	header.m_visible = TRUE;
	header.SetHeader(HEAD_NEWGAME);
	header.SetExecute(ng_btncancel_end);
}

/* Init */
void menu_newgame_init(void)
{
	fn_newgame = spawn(CWidget);
	ng_btnEasy = spawn(CMainButton);
	ng_btnMedium = spawn(CMainButton);
	ng_btnDifficult = spawn(CMainButton);
	ng_btnCancel = spawn(CMainButton);
	
	ng_btnEasy.SetPos(50,140);
	ng_btnMedium.SetPos(50,172);
	ng_btnDifficult.SetPos(50,204);
	ng_btnCancel.SetPos(50,236);
	
	ng_btnEasy.SetImage(BTN_EASY);
	ng_btnEasy.SetExecute(ng_btneasy_start);
	ng_btnMedium.SetImage(BTN_MEDIUM);
	ng_btnMedium.SetExecute(ng_btneasy_start);
	ng_btnDifficult.SetImage(BTN_DIFFICULT);
	ng_btnDifficult.SetExecute(ng_btneasy_start);
	ng_btnCancel.SetImage(BTN_CANCEL);
	ng_btnCancel.SetExecute(ng_btncancel_start);
	
	Widget_Add(fn_newgame, ng_btnEasy);
	Widget_Add(fn_newgame, ng_btnMedium);
	Widget_Add(fn_newgame, ng_btnDifficult);
	Widget_Add(fn_newgame, ng_btnCancel);
}

/* Drawing */
void menu_newgame_draw(void)
{
	Widget_Draw(fn_newgame);
	drawpic([g_menuofs[0]+45,g_menuofs[1]+45], g_bmp[HEAD_NEWGAME],[460,80], [1,1,1], 1.0f, 1);

	WLabel_Static(215, 148, m_reslbl[IDS_NEWGAME_EASYHELP], 10, 10, col_help,
					1.0f, 0, font_label);
	WLabel_Static(215, 180, m_reslbl[IDS_NEWGAME_MEDIUMHELP], 10, 10, col_help,
					1.0f, 0, font_label);
	WLabel_Static(215, 212, m_reslbl[IDS_NEWGAME_DIFFICULTHELP], 10, 10, col_help,
					1.0f, 0, font_label);
	WLabel_Static(215, 244, m_reslbl[IDS_NEWGAME_RETURNHELP], 10, 10, col_help,
					1.0f, 0, font_label);
}

void menu_newgame_input(float evtype, float scanx, float chary, float devid)
{
	Widget_Input(fn_newgame, evtype, scanx, chary, devid);
}
