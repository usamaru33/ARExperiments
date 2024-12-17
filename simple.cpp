/*---------------------------------------------------------------------------
	マーカ「Hiro」を認識してワイヤフレームの立方体を表示させるプログラム
---------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdio>
#include <cstdint>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <AR/ar.h>
#include <AR/param.h>
#include <AR/video.h>
#include <AR/gsub.h>


/*
//以下の3つの変数は，各々がARToolKitディテクトリを置いた場所に合わせてファイルパスを適切に書き換えること
*/
// グローバル変数
const char* vconf_name = "C:/ARToolKit/bin/Data/WDM_camera_flipV.xml";	// ビデオデバイスの設定ファイル
const char* cparam_name = "C:/ARToolKit/bin/Data/camera_para.dat";		// カメラパラメータファイル
const char* patt_name = "C:/ARToolKit/bin/Data/patt.hiro";				// パターンファイル


int		patt_id;						// パターンのID
double	patt_trans[3][4];				// 座標変換行列
double	patt_center[2] = { 0.0, 0.0 };	// パターンの中心座標
double	patt_width = 80.0;			// パターンのサイズ（単位：mm）
int		thresh = 100;			// 2値化の閾値
double x_trans = 0;
double y_trans = 0;
//以下追加した変数
int start_time = 0;//タイマー開始判定
int run_time = 0;//タイマー動作の判定
int stop_time = 0;//タイマーストップの判定
double start, end;//開始時間とサイクルごとの時間の記録
double total = 0.0, set = 10.0;//経過時間と制限時間
char message[] = "時間です！";//コマンドプロンプトに投げるメッセージ


// プロトタイプ宣言
void MainLoop(void);
void DrawObject(void);
void MouseEvent(int button, int state, int x, int y);
void KeyEvent(unsigned char key, int x, int y);
void Cleanup(void);


//===========================================================================
// main関数
//===========================================================================
int main(int argc, char** argv)
{
	ARParam	cparam;			// カメラパラメータ
	ARParam	wparam;			// カメラパラメータ（作業用変数）
	int		xsize, ysize;	// 画像サイズ

	// GLUTの初期化
	glutInit(&argc, argv);

	// ビデオデバイスの設定
	if (arVideoOpen(const_cast<char *>(vconf_name)) < 0) {
		printf("ビデオデバイスのエラー");
		return -1;
	}

	// カメラパラメータの設定
	if (arVideoInqSize(&xsize, &ysize) < 0) {
		printf("画像サイズを取得できませんでした\n");
		return -1;
	}
	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		printf("カメラパラメータの読み込みに失敗しました\n");
		return -1;
	}
	arParamChangeSize(&wparam, xsize, ysize, &cparam);
	arInitCparam(&cparam);

	// パターンファイルのロード
	if ((patt_id = arLoadPatt(patt_name)) < 0) {
		printf("パターンファイルの読み込みに失敗しました\n");
		return -1;
	}



	// ウィンドウの設定
	argInit(&cparam, 1.0, 0, 0, 0, 0);

	// ビデオキャプチャの開始
	arVideoCapStart();

	// メインループの開始
	argMainLoop(MouseEvent, KeyEvent, MainLoop);



	return 0;
}


//===========================================================================
// メインループ関数
//===========================================================================
void MainLoop(void)
{
	ARUint8* image;
	ARMarkerInfo* marker_info;
	int				marker_num;
	int				j, k;

	// カメラ画像の取得
	if ((image = arVideoGetImage()) == NULL) {
		arUtilSleep(2);
		return;
	}

	// カメラ画像の描画
	argDrawMode2D();
	argDispImage(image, 0, 0);

	// マーカの検出と認識
	if (arDetectMarker(image, thresh, &marker_info, &marker_num) < 0) {
		Cleanup();
		exit(0);
	}

	// 次の画像のキャプチャ指示
	arVideoCapNext();

	// マーカの信頼度の比較
	k = -1;
	for (j = 0; j < marker_num; j++) {
		if (patt_id == marker_info[j].id) {
			if (k == -1) k = j;
			else if (marker_info[k].cf < marker_info[j].cf) k = j;
		}
	}

	if (k != -1) {
		// マーカの位置・姿勢（座標変換行列）の計算
		arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);

		// 3Dオブジェクトの描画
		DrawObject();
	}

	// バッファの内容を画面に表示
	argSwapBuffers();

	if (start_time == 1) {//タイマースタートの処理
		start = clock() / CLOCKS_PER_SEC;
		start_time = 0;
		run_time = 1;
		printf("test");
	}

	if (run_time == 1) {//制限時間超過の処理　
		end = clock() / CLOCKS_PER_SEC;
		total = end - start;
		if (total > set) {
			printf("\n\n%s\n", message);
			run_time = 0;
			set = 10.0;
		}
	}
	if (stop_time == 1 && run_time==1) {//タイマーストップの処理
		printf("%.0f秒です。\n", (double)(end - start));
		stop_time = 0;
		run_time = 0;
		set = 10.0;
	}

}


//===========================================================================
// 3Dオブジェクトの描画を行う関数
//===========================================================================
void DrawObject(void)
{
	double	gl_para[16];

	// 3Dオブジェクトを描画するための準備
	argDrawMode3D();
	argDraw3dCamera(0, 0);

	// 座標変換行列の適用
	argConvGlpara(patt_trans, gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(gl_para);

	// 3Dオブジェクトの描画
	glTranslatef(x_trans, y_trans, 20.0);
	glColor3f(0.0, 1.0, 0.0);
	glLineWidth(3.0);
	glutWireCube(40.0);
}


//===========================================================================
// マウス入力処理関数
//===========================================================================
void MouseEvent(int button, int state, int x, int y)
{
	// 入力状態を表示
	printf("ボタン:%d 状態:%d 座標:(x,y)=(%d,%d) \n", button, state, x, y);
}


//===========================================================================
// キーボード入力処理関数
//===========================================================================
void KeyEvent(unsigned char key, int x, int y)
{
	// ESCキーを入力したらアプリケーション終了
	if (key == 0x1b) {
		Cleanup();
		exit(0);
	}
	else if (key == 'w' || key== 'W') {
		y_trans += 10;
	}
	else if (key == 'a' || key == 'A') {
		x_trans -= 10;
	}
	else if (key == 's' || key == 'S') {
		y_trans -= 10;
	}
	else if (key == 'd' || key == 'D') {
		x_trans += 10;
	}
	else if (key == 'c' || key == 'C') {//タイマーストップ処理
		if (run_time == 1) {
			stop_time = 1;
		}
	}
	else if (key == 'v' || key == 'V') {//タイマースタート処理
		if (run_time == 0) {
			start_time = 1;
		}
	}
	else if (key == 'm' || key == 'M') {//制限時間の延長
		set += 5.0;
	}
}


//===========================================================================
// 終了処理関数
//===========================================================================
void Cleanup(void)
{
	arVideoCapStop();	// ビデオキャプチャの停止
	arVideoClose();		// ビデオデバイスの終了
	argCleanup();		// グラフィック処理の終了
}
