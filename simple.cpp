/*---------------------------------------------------------------------------
	�}�[�J�uHiro�v��F�����ă��C���t���[���̗����̂�\��������v���O����
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
//�ȉ���3�̕ϐ��́C�e�X��ARToolKit�f�B�e�N�g����u�����ꏊ�ɍ��킹�ăt�@�C���p�X��K�؂ɏ��������邱��
*/
// �O���[�o���ϐ�
const char* vconf_name = "C:/ARToolKit/bin/Data/WDM_camera_flipV.xml";	// �r�f�I�f�o�C�X�̐ݒ�t�@�C��
const char* cparam_name = "C:/ARToolKit/bin/Data/camera_para.dat";		// �J�����p�����[�^�t�@�C��
const char* patt_name = "C:/ARToolKit/bin/Data/patt.hiro";				// �p�^�[���t�@�C��


int		patt_id;						// �p�^�[����ID
double	patt_trans[3][4];				// ���W�ϊ��s��
double	patt_center[2] = { 0.0, 0.0 };	// �p�^�[���̒��S���W
double	patt_width = 80.0;			// �p�^�[���̃T�C�Y�i�P�ʁFmm�j
int		thresh = 100;			// 2�l����臒l
double x_trans = 0;
double y_trans = 0;
int start_time = 0;
int run_time = 0;
int stop_time = 0;
double start, end;
double total = 0.0, set = 10.0;
char message[] = "���Ԃł��I";


// �v���g�^�C�v�錾
void MainLoop(void);
void DrawObject(void);
void MouseEvent(int button, int state, int x, int y);
void KeyEvent(unsigned char key, int x, int y);
void Cleanup(void);


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
	if (arVideoOpen(const_cast<char *>(vconf_name)) < 0) {
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
	if ((patt_id = arLoadPatt(patt_name)) < 0) {
		printf("�p�^�[���t�@�C���̓ǂݍ��݂Ɏ��s���܂���\n");
		return -1;
	}



	// �E�B���h�E�̐ݒ�
	argInit(&cparam, 1.0, 0, 0, 0, 0);

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
	k = -1;
	for (j = 0; j < marker_num; j++) {
		if (patt_id == marker_info[j].id) {
			if (k == -1) k = j;
			else if (marker_info[k].cf < marker_info[j].cf) k = j;
		}
	}

	if (k != -1) {
		// �}�[�J�̈ʒu�E�p���i���W�ϊ��s��j�̌v�Z
		arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);

		// 3D�I�u�W�F�N�g�̕`��
		DrawObject();
	}

	// �o�b�t�@�̓��e����ʂɕ\��
	argSwapBuffers();

	if (start_time == 1) {
		start = clock() / CLOCKS_PER_SEC;
		start_time = 0;
		run_time = 1;
		printf("test");
	}

	if (run_time == 1) {
		end = clock() / CLOCKS_PER_SEC;
		total = end - start;
		if (total > set) {
			printf("\n\n%s\n", message);
			run_time = 0;
			set = 10.0;
		}
	}
	if (stop_time == 1 && run_time==1) {
		printf("%.0f�b�ł��B\n", (double)(end - start));
		stop_time = 0;
		run_time = 0;
		set = 10.0;
	}

}


//===========================================================================
// 3D�I�u�W�F�N�g�̕`����s���֐�
//===========================================================================
void DrawObject(void)
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
	glTranslatef(x_trans, y_trans, 20.0);
	glColor3f(0.0, 1.0, 0.0);
	glLineWidth(3.0);
	glutWireCube(40.0);
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
	else if (key == 'c' || key == 'C') {
		if (run_time == 1) {
			stop_time = 1;
		}
	}
	else if (key == 'v' || key == 'V') {
		if (run_time == 0) {
			start_time = 1;
		}
	}
	else if (key == 'm' || key == 'M') {
		set += 5.0;
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
}
