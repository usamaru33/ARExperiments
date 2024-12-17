/*---------------------------------------------------------------------------
               MQO�`����3D���f����ǂݍ���Ń}�[�J�̏�ɕ\��
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
//�ȉ���4�̕ϐ��́C�e�X��ARToolKit�f�B�e�N�g����u�����ꏊ�ɍ��킹�ăt�@�C���p�X��K�؂ɏ��������邱��
*/
// �O���[�o���ϐ�
const char* vconf_name = "C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/WDM_camera_flipV.xml";	// �r�f�I�f�o�C�X�̐ݒ�t�@�C��
const char* cparam_name = "C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/camera_para.dat";		// �J�����p�����[�^�t�@�C��
const char* patt_name[3] = { "C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/patt.hiro"
, "C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/patt.calib" ,
"C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/patt.kanji" };			// C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/
const char* mqo_name[3] = {
	"C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/ninja.mqo" ,
	"C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/car.mqo"	,
	"C:/Users/fuusu/Downloads/ARToolKit-2.7.1-bin-book/ARToolKit/bin/Data/camera.mqo" };				// MQO�t�@�C��

int		patt_id[3];						// �p�^�[����ID
double	patt_trans[3][4];				// ���W�ϊ��s��
double	patt_center[2] = { 0.0, 0.0 };	// �p�^�[���̒��S���W
double	patt_width = 80.0;			// �p�^�[���̃T�C�Y�i�P�ʁFmm�j
int		thresh = 100;			// 2�l����臒l
int X = 0;
bool flag[3] = { true, true, true };
int visible[3] = { 0, 0, 0 };
int count[3] = { 0,0,0 };
std::chrono::system_clock::time_point  start, end;

// ���f��
MQO_MODEL   modelList[3];

// �v���g�^�C�v�錾
void MainLoop(void);
void DrawObject(int);
void MouseEvent(int button, int state, int x, int y);
void KeyEvent(unsigned char key, int x, int y);
void Cleanup(void);
void mySetLight(void);


//===========================================================================
// main�֐�
//===========================================================================
int main(int argc, char** argv)
{
	ARParam	cparam;			// �J�����p�����[�^
	ARParam	wparam;			// �J�����p�����[�^�i��Ɨp�ϐ��j
	int		xsize, ysize;	// �摜�T�C�Y

	// GLUT�̏�����
	glutInit(&argc, argv);

	// �r�f�I�f�o�C�X�̐ݒ�
	if (arVideoOpen(const_cast<char*>(vconf_name)) < 0) {
		printf("�r�f�I�f�o�C�X�̃G���[");
		return -1;
	}

	// �J�����p�����[�^�̐ݒ�
	if (arVideoInqSize(&xsize, &ysize) < 0) {
		printf("�摜�T�C�Y���擾�ł��܂���ł���\n");
		return -1;
	}
	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		printf("�J�����p�����[�^�̓ǂݍ��݂Ɏ��s���܂���\n");
		return -1;
	}
	arParamChangeSize(&wparam, xsize, ysize, &cparam);
	arInitCparam(&cparam);

	// �p�^�[���t�@�C���̃��[�h
	for (int i = 0; i < 3; i++) {


		if ((patt_id[i] = arLoadPatt(patt_name[i])) < 0) {
			printf("�p�^�[���t�@�C���̓ǂݍ��݂Ɏ��s���܂���\n");
			return -1;
		}
	}

	// �E�B���h�E�̐ݒ�
	argInit(&cparam, 1.0, 0, 0, 0, 0);

	// GLMetaseq�̏�����
	mqoInit();

	// ���f���̓ǂݍ���
	for (int i = 0; i < 3; i++) {
		modelList[i] = mqoCreateModel(mqo_name[i], 1.0);

		if (modelList[i] == NULL) {
			printf("���f���̓ǂݍ��݂Ɏ��s���܂���\n");
			return -1;
		}
	}



	// �r�f�I�L���v�`���̊J�n
	arVideoCapStart();

	// ���C�����[�v�̊J�n
	argMainLoop(MouseEvent, KeyEvent, MainLoop);

	return 0;
}


//===========================================================================
// ���C�����[�v�֐�
//===========================================================================
void MainLoop(void)
{
	ARUint8* image;
	ARMarkerInfo* marker_info;
	int				marker_num;
	int				j, k;

	// �J�����摜�̎擾
	if ((image = arVideoGetImage()) == NULL) {
		arUtilSleep(2);
		return;
	}

	// �J�����摜�̕`��
	argDrawMode2D();
	argDispImage(image, 0, 0);

	// �}�[�J�̌��o�ƔF��
	if (arDetectMarker(image, thresh, &marker_info, &marker_num) < 0) {
		Cleanup();
		exit(0);
	}

	// ���̉摜�̃L���v�`���w��
	arVideoCapNext();

	// �}�[�J�̐M���x�̔�r
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

			// �}�[�J�̈ʒu�E�p���i���W�ϊ��s��j�̌v�Z
			arGetTransMat(&marker_info[i], patt_center, patt_width, patt_trans);

			// 3D�I�u�W�F�N�g�̕`��

		}



		if (flag[i] == true) {
			DrawObject(i);
		}

	}
	// �o�b�t�@�̓��e����ʂɕ\��
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
// �����̐ݒ���s���֐�
//===========================================================================
void mySetLight(void)
{
	GLfloat light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };	// �g�U���ˌ�
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// ���ʔ��ˌ�
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 0.1 };	// ����
	GLfloat light_position[] = { 100.0, -200.0, 200.0, 0.0 };	// �ʒu�Ǝ��

	// �����̐ݒ�
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);	 // �g�U���ˌ��̐ݒ�
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); // ���ʔ��ˌ��̐ݒ�
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);	 // �����̐ݒ�
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // �ʒu�Ǝ�ނ̐ݒ�

	//	glShadeModel( GL_SMOOTH );	// �V�F�[�f�B���O�̎�ނ̐ݒ�
	glEnable(GL_LIGHT0);		// �����̗L����
}


//===========================================================================
// 3D�I�u�W�F�N�g�̕`����s���֐�
//===========================================================================
void DrawObject(int Y)
{
	double	gl_para[16];

	// 3D�I�u�W�F�N�g��`�悷�邽�߂̏���
	argDrawMode3D();
	argDraw3dCamera(0, 0);

	// ���W�ϊ��s��̓K�p
	argConvGlpara(patt_trans, gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(gl_para);

	// 3D�I�u�W�F�N�g�̕`��
	glClear(GL_DEPTH_BUFFER_BIT);			// Z�o�b�t�@�̏�����
	glEnable(GL_DEPTH_TEST);				// �B�ʏ����̓K�p

	mySetLight();							// �����̐ݒ�
	glEnable(GL_LIGHTING);				// �����̓K�p

	glPushMatrix();
	glRotatef(90.0, 1.0, 0.0, 0.0);	// ���f���𗧂�����
	mqoCallModel(modelList[Y]);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);


}


//===========================================================================
// �}�E�X���͏����֐�
//===========================================================================
void MouseEvent(int button, int state, int x, int y)
{
	// ���͏�Ԃ�\��
	printf("�{�^��:%d ���:%d ���W:(x,y)=(%d,%d) \n", button, state, x, y);
}


//===========================================================================
// �L�[�{�[�h���͏����֐�
//===========================================================================
void KeyEvent(unsigned char key, int x, int y)
{
	// ESC�L�[����͂�����A�v���P�[�V�����I��
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
// �I�������֐�
//===========================================================================
void Cleanup(void)
{
	arVideoCapStop();	// �r�f�I�L���v�`���̒�~
	arVideoClose();		// �r�f�I�f�o�C�X�̏I��
	argCleanup();		// �O���t�B�b�N�����̏I��
	// ���f���̍폜
	mqoCleanup();				// GLMetaseq�̏I������
}
