#include<Dxlib.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE); //Window表示にする  640*480

	if (DxLib_Init() == -1)    return -1;    //DXライブラリ初期位置

	printfDx("Hallo World!\n");
	WaitKey();                               //キーorマウス入力待ち

	DxLib_End();
	return 0;                                //ライブラリ終了処置
}