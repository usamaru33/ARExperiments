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

const char* vconf_name = "E:/ARToolKit/bin/Data/WDM_camera_flipV.xml";
const char* cparam_name = "E:/ARToolKit/bin/Data/camera_para.dat";
const char* patt_name[] = { "E:/ARToolKit/bin/Data/patt.hiro"  };//hiro=start�ީ`��
const char* mqo_name[] = { "E:/ARToolKit/bin/Data/ninja.mqo", "E:/ARToolKit/bin/Data/camera.mqo", "E:/ARToolKit/bin/Data/car.mqo" };
int		patt_id[2];
double	patt_trans[3][4];
double	patt_center[2] = { 0.0, 0.0 };
double	patt_width = 80.0;
int		thresh = 100;
int     modenum = 0;
int     game_started = 0; // ���`��״�B������1 ���`���_ʼ���ʾ
MQO_MODEL	model;

void MainLoop(void);
void DrawObject(void);
void MouseEvent(int button, int state, int x, int y);
void KeyEvent(unsigned char key, int x, int y);
void Cleanup(void);

int main(int argc, char** argv)
{
	ARParam	cparam;
	ARParam	wparam;
	int		xsize, ysize;

	glutInit(&argc, argv);

	if (arVideoOpen(const_cast<char*>(vconf_name)) < 0) {
		printf("�ӥǥ����ȥ�`����_���ޤ���\n");//�ӥǥ����ȥ�`�ब�_���ʤ�
		return -1;
	}

	if (arVideoInqSize(&xsize, &ysize) < 0) {
		printf("�ӥǥ���������ȡ�äǤ��ޤ���\n");//�ӥǥ��Υ�������ȡ���ʤ�
		return -1;
	}
	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		printf("�����ѥ��`���ե�������i���z��ޤ���\n");//�����ѥ��`���`�ե�������`�ɤǤ��ʤ�
		return -1;
	}
	arParamChangeSize(&wparam, xsize, ysize, &cparam);
	arInitCparam(&cparam);

	if ((patt_id[0] = arLoadPatt(patt_name[0])) < 0) {
		printf("HIRO�ީ`���`���i���z��ޤ���\n");//hiro(start)�ީ`�����`�ɤǤ��ʤ�
		return -1;
	}

	argInit(&cparam, 1.0, 0, 0, 0, 0);

	arVideoCapStart();

	argMainLoop(MouseEvent, KeyEvent, MainLoop);

	return 0;
}

void MainLoop(void)
{
	ARUint8* image;
	ARMarkerInfo* marker_info;
	int marker_num;
	int hiro_index = -1;
	bool checkp_index[3] = { false,false,false};

	// ��ȡ����ͷͼ��
	if ((image = arVideoGetImage()) == NULL) {
		arUtilSleep(2);
		return;
	}

	// ��ʾ����ͷͼ��
	argDrawMode2D();
	argDispImage(image, 0, 0);

	// ����־
	if (arDetectMarker(image, thresh, &marker_info, &marker_num) < 0) {
		Cleanup();
		exit(0);
	}

	// hiro�Υީ`����Ҋ�Ĥ��ޤ���
	for (int i = 0; i < marker_num; i++) {
		if (marker_info[i].id == patt_id[0]) {
			if (hiro_index == -1 || marker_info[hiro_index].cf < marker_info[i].cf) {
				hiro_index = i;
			}
		}
	}
	// ��������ͷͼ��
	arVideoCapNext();

	// ��Ϸ�߼�
	if (hiro_index != -1 ) {
		//hiro�ީ`���������ʳ�����ޤ���
		if (!game_started) {
			game_started = 1;
			printf("���`���_ʼ��\n");//���`�ॹ���`��
		}
	}
	else if (hiro_index == -1 && checkp_index[0] == true && checkp_index[1] == true && checkp_index[2] == true) {//hiro�ީ`���`�J�R�Ǥ��ʤ���ͬ�rȫ�ƤΥ����å��ݥ���Ȥ�ͨ�^���ޤ���
		if (game_started) {
			game_started = 0;
			printf("���`��K��\n");//���`�२���
		}
	}

	argSwapBuffers();
}

void DrawObject(void)
{
	double gl_para[16];

	argDrawMode3D();
	argDraw3dCamera(0, 0);

	argConvGlpara(patt_trans, gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(gl_para);

	glTranslatef(0.0, 0.0, 20.0);
	glColor3f(0.0, 1.0, 0.0);
	glLineWidth(3.0);
	glutWireCube(40.0);
}

void MouseEvent(int button, int state, int x, int y)
{
	printf("�ޥ������٥��: �ܥ���:%d ״�B:%d ����:(x,y)=(%d,%d)\n", button, state, x, y);
}

void KeyEvent(unsigned char key, int x, int y)
{
	if (key == 0x1b) {
		Cleanup();
		exit(0);
	}
}

void Cleanup(void)
{
	arVideoCapStop();
	arVideoClose();
	argCleanup();

}
