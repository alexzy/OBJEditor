#include "stdafx.h"
#include "Win32GL.h"
#include "Camera.h"
#include "ModelOBJ.h"
#include "Guides.h"

string g_modelFileName;
CModelOBJ g_modelObj;
//��������
void init();
void render();
void reshape(int w, int h);
void onMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
void onKeyEvent(UINT key);

int main(int argc, char **argv)
{
	if( argc < 2 )
	{
		MessageBox( NULL, "���һ��OBJ�ļ�ȫ·�������������������Ͻ�����", "Error", MB_OK | MB_ICONWARNING );
		return 0;
	}
	else
	{
		g_modelFileName = argv[1];
	}
	cout<<"****************** OBJ Editor by ֣Խ ******************"<<endl;
	cout<<"** ��1������ ��2���л��Ƿ���ʾ������ͼ ��3���л��ƹ� ***"<<endl;
	cout<<"** ����������м��ƶ����Ӳ�ͬ�ǶȲ鿴ģ�� **************"<<endl;
	cout<<"** ���������֡����˻�������� **************************"<<endl;
	cout<<"** ������������������ѡ��ѡ������Ϊ��λ��ģ������ **"<<endl;
	cout<<"** ����������Ҽ��϶����Ƹˡ���ѡ�������϶�����ƽ�� **"<<endl;
	cout<<"********************************************************"<<endl;
	cout<<"��ȡ�ļ���"<<g_modelFileName<<endl;

	if( !WIN32GL->InitWindow( "OBJ Editor", 480, 480 ) )
		return 0;
	init();
	GUIDES->SetAspect(480, 480);
	WIN32GL->SetRender(render, 40);
	WIN32GL->SetReshape(reshape);
	WIN32GL->SetMouseEvent(onMouseEvent);
	WIN32GL->SetKeyEvent(onKeyEvent);
	WIN32GL->MainLoop();

	return 0;
}

void init()
{
	//OpenGL��ʼ��
	glClearColor(0.3f, 0.5f, 0.8f, 0.0f);					//����ɫ
	glShadeModel(GL_SMOOTH);								//������Ӱƽ��
	glClearDepth(1.0f);										//������Ȼ���
	glEnable(GL_DEPTH_TEST);								//������Ȳ���
	glDepthFunc(GL_LESS);									//������Ȳ��Ե�����
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		//������ϸ��͸������

	//����
	float light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	float light_ambient [] = { 1.0, 1.0, 1.0, 1.0 };
	float light_diffuse [] = { 1.0, 1.0, 1.0, 1.0 };
	float light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);		//�������Դ
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);		//����������
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);		//�����������
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);		//�������淴���
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//���ʣ������ã�
	//float mat_specular[] = { 1.0, 1.0, 0.0, 1.0 };		//���Ϸ�������ɫ
	//float mat_shininess[] = { 50.0 };						//�߹�ָ����0-128.0��ֵԽ��߹��ԽСԽ��
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//�߿�ƽ���������ã�
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//�޳��������ã�
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
	g_modelObj.LoadFile(g_modelFileName);
	g_modelObj.LoadMaterial();
	CAMERA->Init(g_modelObj.GetRange());
	GUIDES->Init(g_modelObj.GetRange(), g_modelObj.GetRadius());
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//���λ��
	CAMERA->Render();
	//����ģ��
	g_modelObj.Render();
	//���Ƹ�����
	GUIDES->Render();
}
void reshape(int w, int h)
{
	//��ֹ��0��
	if( h == 0 )
		h = 1;
	glViewport( 0,0,w,h );
	GUIDES->SetAspect(w, h);
	//ͶӰ�任
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 60.0f, (float)w/(float)h, 0.1f, 1000.0f );
	//ģ�ͱ任
	glMatrixMode(GL_MODELVIEW);
}

UINT g_buttonType = 0;
POINT g_oldPos = {0, 0};

void onMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int width, height, x, y, hits;
	static GLint viewport[4];
	bool bOneSel = false;
	switch(uMsg)
	{
	case WM_MOUSEWHEEL:			//����
		if(g_buttonType == 0)
		{
			if((short)HIWORD(wParam) > 0)
				CAMERA->Zoom(-2.0f);
			else
				CAMERA->Zoom(2.0f);
		}
		break;
	case WM_LBUTTONDOWN:		//�������
		if(g_buttonType == 0)
		{
			g_buttonType = WM_LBUTTONDOWN;
			g_oldPos.x = LOWORD(lParam);
			g_oldPos.y = HIWORD(lParam);
			GUIDES->InitFrame(g_oldPos.x, g_oldPos.y);
		}
		break;
	case WM_MBUTTONDOWN:		//�м�����
		if(g_buttonType == 0)
		{
			g_buttonType = WM_MBUTTONDOWN;
			g_oldPos.x = LOWORD(lParam);
			g_oldPos.y = HIWORD(lParam);
			CAMERA->PreRotate();
		}
		break;
	case WM_RBUTTONDOWN:		//�Ҽ�����
		if(g_buttonType == 0)
		{
			g_buttonType = WM_RBUTTONDOWN;
			g_oldPos.x = LOWORD(lParam);
			g_oldPos.y = HIWORD(lParam);
			//ѡ��ģʽ
			GUIDES->InitSelect();
			glRenderMode(GL_SELECT);
			glInitNames();
			glPushMatrix();			//model push
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			//����ѡ���Ӿ���
			glGetIntegerv(GL_VIEWPORT, viewport);
			gluPickMatrix( (GLdouble)(LOWORD(lParam)), (GLdouble)(viewport[3]-HIWORD(lParam)), 10, 10, viewport);
			//��Ⱦѡ��ģʽ
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluPerspective( 60.0f, (float)viewport[2]/(float)viewport[3], 0.1f, 1000.0f );
			CAMERA->Render();
			GUIDES->SelectControl();
			//�ָ�
			glPopMatrix();			//model pop
			hits = glRenderMode(GL_RENDER);
			GUIDES->ProcessSelect(hits);
			reshape(viewport[2], viewport[3]);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			//�϶���ʼ��
			g_modelObj.InitDrag();
		}
		break;
	case WM_MOUSEMOVE:			//�ƶ�
		//��������ƶ�
		if(g_buttonType == WM_LBUTTONDOWN)
		{
			GUIDES->DragFrame(LOWORD(lParam), HIWORD(lParam));
		}
		//�����м��ƶ�
		if(g_buttonType == WM_MBUTTONDOWN)
		{
			CAMERA->Rotate(LOWORD(lParam)-g_oldPos.x, HIWORD(lParam)-g_oldPos.y);
		}
		//�����Ҽ��ƶ�
		if(g_buttonType == WM_RBUTTONDOWN)
		{
			float dist = 0;
			//glGetIntegerv(GL_VIEWPORT, viewport);
			if(GUIDES->GetControl() == SEL_CX)	//X��
			{
				dist = (float)(LOWORD(lParam)-g_oldPos.x)/viewport[2];
				if(fabs(CAMERA->GetYaw())>90)
					dist = 0-dist;
			}
			if(GUIDES->GetControl() == SEL_CY)	//Y��
			{
				dist = (float)(g_oldPos.y-HIWORD(lParam))/viewport[3];
			}
			if(GUIDES->GetControl() == SEL_CZ)	//Z��
			{
				dist = (float)(LOWORD(lParam)-g_oldPos.x)/viewport[2];
				if(CAMERA->GetYaw()<0)
					dist = 0-dist;
			}

			g_modelObj.DragSelect(dist, GUIDES->GetControl());
		}
		break;
	case WM_LBUTTONUP:			//�������
		g_buttonType = 0;
		GUIDES->EndFrame();
		GUIDES->EndControl();
		//ѡ��ģʽ
		g_modelObj.InitSelect();
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushMatrix();			//model push
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//����ѡ���Ӿ���
		glGetIntegerv(GL_VIEWPORT, viewport);
		width = abs(LOWORD(lParam)-g_oldPos.x);
		height = abs(HIWORD(lParam)-g_oldPos.y);
		if(width<5 && height<5)
			bOneSel = true;
		width = (width<5) ? 5 : width;
		height = (height<5) ? 5 : height;
		x = g_oldPos.x + width/2;
		y = g_oldPos.y + height/2;
		gluPickMatrix( (GLdouble)x, (GLdouble)(viewport[3]-y), width, height, viewport);
		//��Ⱦѡ��ģʽ
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluPerspective( 60.0f, (float)viewport[2]/(float)viewport[3], 0.1f, 1000.0f );
		CAMERA->Render();
		g_modelObj.Render(GL_SELECT);
		//�ָ�
		glPopMatrix();			//model pop
		hits = glRenderMode(GL_RENDER);
		g_modelObj.ProcessSelect(hits, bOneSel);
		if(hits > 0)
			GUIDES->InitControl(g_modelObj.GetSelectCenter().x, g_modelObj.GetSelectCenter().y, g_modelObj.GetSelectCenter().z);
		reshape(viewport[2], viewport[3]);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		break;
	case WM_MBUTTONUP:			//�м�����
		g_buttonType = 0;
		break;
	case WM_RBUTTONUP:			//�Ҽ�����
		g_buttonType = 0;
		g_modelObj.EndDrag();
		break;
	}
}

void onKeyEvent(UINT key)
{
	switch(key)
	{
	case '1':
		g_modelObj.EnWireFrame(true);
		g_modelObj.EnTexture(false);
		break;
	case '2':
		g_modelObj.EnWireFrame(false);
		g_modelObj.SwitchTexture();
		break;
	case '3':
		g_modelObj.SwitchShadow();
		break;
	}
}