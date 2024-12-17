/*---------------------------------------------------------------------------
               MQO形式の3Dモデルを読み込んでマーカの上に表示
---------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <AR/ar.h>
#include <AR/param.h>
#include <AR/video.h>
#include <AR/gsub.h>
#include "GLMetaseq.h"

#include <chrono>
#include <iostream>

/*
//以下の4つの変数は，各々がARToolKitディテクトリを置いた場所に合わせてファイルパスを適切に書き換えること
*/
// グローバル変数
const char* vconf_name = "C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/WDM_camera_flipV.xml";	// ビデオデバイスの設定ファイル
const char* cparam_name = "C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/camera_para.dat";		// カメラパラメータファイル
const char* patt_name[3] = { "C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/patt.hiro"
, "C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/patt.calib" ,
"C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/patt.kanji" };			// C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/
const char* mqo_name[3] = {
	"C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/ninja.mqo" ,
	"C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/car.mqo"	,
	"C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/camera.mqo" };				// MQOファイル

int		patt_id[3];						// パターンのID
double	patt_trans[3][4];				// 座標変換行列
double	patt_center[2] = { 0.0, 0.0 };	// パターンの中心座標
double	patt_width = 80.0;			// パターンのサイズ（単位：mm）
int		thresh = 100;			// 2値化の閾値
int X = 0;
bool flag[3] = { true, true, true };
int visible[3] = { 0, 0, 0 };
int count[3] = { 0,0,0 };
std::chrono::system_clock::time_point  start, end;

// モデル
MQO_MODEL   modelList[3];

// プロトタイプ宣言
void MainLoop(void);
void DrawObject(int);
void MouseEvent(int button, int state, int x, int y);
void KeyEvent(unsigned char key, int x, int y);
void Cleanup(void);
void mySetLight(void);


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
	if (arVideoOpen(const_cast<char*>(vconf_name)) < 0) {
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
	for (int i = 0; i < 3; i++) {


		if ((patt_id[i] = arLoadPatt(patt_name[i])) < 0) {
			printf("パターンファイルの読み込みに失敗しました\n");
			return -1;
		}
	}

	// ウィンドウの設定
	argInit(&cparam, 1.0, 0, 0, 0, 0);

	// GLMetaseqの初期化
	mqoInit();

	// モデルの読み込み
	for (int i = 0; i < 3; i++) {
		modelList[i] = mqoCreateModel(mqo_name[i], 1.0);

		if (modelList[i] == NULL) {
			printf("モデルの読み込みに失敗しました\n");
			return -1;
		}
	}



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
	for (int i = 0; i <= 3; i++) {
		k = -1;
		for (j = 0; j < marker_num; j++) {
			if (patt_id[i] == marker_info[j].id) {
				if (k == -1) k = j;
				else if (marker_info[k].cf < marker_info[j].cf) k = j;
			}
		}



		if (k == -1) {
			if (i == 0) {
				count[0] += 1;
			}

			if (i == 1 && flag[0] == false) {
				count[1] += 1;
			}

			if (i == 2 && flag[1] == false) {
				count[2] += 1;
			}
			printf(" %d", count[1]);
			continue;
		}



		if (count[i] > 10) {

			//test
			//


			if (i == 0) {
				flag[0] = false;
			}

			if (i == 1 && flag[1] != false && flag[0] == false) {
				flag[1] = false;
			}

			if (i == 2 && flag[2] != false && flag[1] == false) {
				flag[2] = false;
			}
			continue;
		}

		if (count[1] > 10) {

			//test
			//


			

			if ( flag[1] != false && flag[0] == false) {
				flag[1] = false;
			}

			
			continue;
		}


		if (k != -1) {

			if (count[i] <= 10) {
				count[i] = 0;
			}

			// マーカの位置・姿勢（座標変換行列）の計算
			arGetTransMat(&marker_info[i], patt_center, patt_width, patt_trans);

			// 3Dオブジェクトの描画

		}



		if (flag[i] == true) {
			DrawObject(i);
		}

	}
	// バッファの内容を画面に表示
	argSwapBuffers();

	if (flag[0] == false && modelList[0] != NULL) {
		mqoDeleteModel(modelList[0]);
		modelList[0] = NULL;
		printf("model deleted : 0");
	}

	if (flag[1] == false && modelList[1] != NULL) {
		mqoDeleteModel(modelList[1]);
		modelList[1] = NULL;
		printf("model deleted : 1");
	}

	if (flag[2] == false && modelList[2] != NULL) {
		mqoDeleteModel(modelList[2]);
		modelList[2] = NULL;
		printf("model deleted : 2");
	}

}


//===========================================================================
// 光源の設定を行う関数
//===========================================================================
void mySetLight(void)
{
	GLfloat light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };	// 拡散反射光
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// 鏡面反射光
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 0.1 };	// 環境光
	GLfloat light_position[] = { 100.0, -200.0, 200.0, 0.0 };	// 位置と種類

	// 光源の設定
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);	 // 拡散反射光の設定
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); // 鏡面反射光の設定
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);	 // 環境光の設定
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // 位置と種類の設定

	//	glShadeModel( GL_SMOOTH );	// シェーディングの種類の設定
	glEnable(GL_LIGHT0);		// 光源の有効化
}


//===========================================================================
// 3Dオブジェクトの描画を行う関数
//===========================================================================
void DrawObject(int Y)
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
	glClear(GL_DEPTH_BUFFER_BIT);			// Zバッファの初期化
	glEnable(GL_DEPTH_TEST);				// 隠面処理の適用

	mySetLight();							// 光源の設定
	glEnable(GL_LIGHTING);				// 光源の適用

	glPushMatrix();
	glRotatef(90.0, 1.0, 0.0, 0.0);	// モデルを立たせる
	mqoCallModel(modelList[Y]);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);


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

	if (key == 'C' || key == 'c') {
		X = (X + 1) % 6;
	}

	if (key == 0x20) {

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
	// モデルの削除
	mqoCleanup();				// GLMetaseqの終了処理
}
