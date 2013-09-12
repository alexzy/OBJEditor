#include "stdafx.h"
#include "Win32GL.h"
#include "Camera.h"
#include "ModelOBJ.h"
#include "Guides.h"

string g_modelFileName;
CModelOBJ g_modelObj;
//函数声明
void init();
void render();
void reshape(int w, int h);
void onMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
void onKeyEvent(UINT key);

int main(int argc, char **argv)
{
	if( argc < 2 )
	{
		MessageBox( NULL, "请把一个OBJ文件全路径当做参数传给程序（拖进来）", "Error", MB_OK | MB_ICONWARNING );
		return 0;
	}
	else
	{
		g_modelFileName = argv[1];
	}
	cout<<"****************** OBJ Editor by 郑越 ******************"<<endl;
	cout<<"** 【1】网格 【2】切换是否显示纹理贴图 【3】切换灯光 ***"<<endl;
	cout<<"** 【按下鼠标中键移动】从不同角度查看模型 **************"<<endl;
	cout<<"** 【滚动滚轮】后退或拉近相机 **************************"<<endl;
	cout<<"** 【按下鼠标左键点击或框选】选择以组为单位的模型网格 **"<<endl;
	cout<<"** 【按下鼠标右键拖动控制杆】把选中网格按拖动轴向平移 **"<<endl;
	cout<<"********************************************************"<<endl;
	cout<<"读取文件："<<g_modelFileName<<endl;

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
	//OpenGL初始化
	glClearColor(0.3f, 0.5f, 0.8f, 0.0f);					//背景色
	glShadeModel(GL_SMOOTH);								//启用阴影平滑
	glClearDepth(1.0f);										//设置深度缓存
	glEnable(GL_DEPTH_TEST);								//启用深度测试
	glDepthFunc(GL_LESS);									//所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		//真正精细的透视修正

	//光照
	float light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	float light_ambient [] = { 1.0, 1.0, 1.0, 1.0 };
	float light_diffuse [] = { 1.0, 1.0, 1.0, 1.0 };
	float light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);		//创建点光源
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);		//创建环境光
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);		//创建漫反射光
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);		//创建镜面反射光
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//材质（测试用）
	//float mat_specular[] = { 1.0, 1.0, 0.0, 1.0 };		//材料反射光的颜色
	//float mat_shininess[] = { 50.0 };						//高光指数，0-128.0，值越大高光点越小越亮
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//线框平滑（测试用）
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//剔除（测试用）
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
	//相机位置
	CAMERA->Render();
	//绘制模型
	g_modelObj.Render();
	//绘制辅助线
	GUIDES->Render();
}
void reshape(int w, int h)
{
	//防止被0除
	if( h == 0 )
		h = 1;
	glViewport( 0,0,w,h );
	GUIDES->SetAspect(w, h);
	//投影变换
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 60.0f, (float)w/(float)h, 0.1f, 1000.0f );
	//模型变换
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
	case WM_MOUSEWHEEL:			//滚轮
		if(g_buttonType == 0)
		{
			if((short)HIWORD(wParam) > 0)
				CAMERA->Zoom(-2.0f);
			else
				CAMERA->Zoom(2.0f);
		}
		break;
	case WM_LBUTTONDOWN:		//左键按下
		if(g_buttonType == 0)
		{
			g_buttonType = WM_LBUTTONDOWN;
			g_oldPos.x = LOWORD(lParam);
			g_oldPos.y = HIWORD(lParam);
			GUIDES->InitFrame(g_oldPos.x, g_oldPos.y);
		}
		break;
	case WM_MBUTTONDOWN:		//中键按下
		if(g_buttonType == 0)
		{
			g_buttonType = WM_MBUTTONDOWN;
			g_oldPos.x = LOWORD(lParam);
			g_oldPos.y = HIWORD(lParam);
			CAMERA->PreRotate();
		}
		break;
	case WM_RBUTTONDOWN:		//右键按下
		if(g_buttonType == 0)
		{
			g_buttonType = WM_RBUTTONDOWN;
			g_oldPos.x = LOWORD(lParam);
			g_oldPos.y = HIWORD(lParam);
			//选择模式
			GUIDES->InitSelect();
			glRenderMode(GL_SELECT);
			glInitNames();
			glPushMatrix();			//model push
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			//设置选择视景体
			glGetIntegerv(GL_VIEWPORT, viewport);
			gluPickMatrix( (GLdouble)(LOWORD(lParam)), (GLdouble)(viewport[3]-HIWORD(lParam)), 10, 10, viewport);
			//渲染选择模式
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluPerspective( 60.0f, (float)viewport[2]/(float)viewport[3], 0.1f, 1000.0f );
			CAMERA->Render();
			GUIDES->SelectControl();
			//恢复
			glPopMatrix();			//model pop
			hits = glRenderMode(GL_RENDER);
			GUIDES->ProcessSelect(hits);
			reshape(viewport[2], viewport[3]);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			//拖动初始化
			g_modelObj.InitDrag();
		}
		break;
	case WM_MOUSEMOVE:			//移动
		//按下左键移动
		if(g_buttonType == WM_LBUTTONDOWN)
		{
			GUIDES->DragFrame(LOWORD(lParam), HIWORD(lParam));
		}
		//按下中键移动
		if(g_buttonType == WM_MBUTTONDOWN)
		{
			CAMERA->Rotate(LOWORD(lParam)-g_oldPos.x, HIWORD(lParam)-g_oldPos.y);
		}
		//按下右键移动
		if(g_buttonType == WM_RBUTTONDOWN)
		{
			float dist = 0;
			//glGetIntegerv(GL_VIEWPORT, viewport);
			if(GUIDES->GetControl() == SEL_CX)	//X轴
			{
				dist = (float)(LOWORD(lParam)-g_oldPos.x)/viewport[2];
				if(fabs(CAMERA->GetYaw())>90)
					dist = 0-dist;
			}
			if(GUIDES->GetControl() == SEL_CY)	//Y轴
			{
				dist = (float)(g_oldPos.y-HIWORD(lParam))/viewport[3];
			}
			if(GUIDES->GetControl() == SEL_CZ)	//Z轴
			{
				dist = (float)(LOWORD(lParam)-g_oldPos.x)/viewport[2];
				if(CAMERA->GetYaw()<0)
					dist = 0-dist;
			}

			g_modelObj.DragSelect(dist, GUIDES->GetControl());
		}
		break;
	case WM_LBUTTONUP:			//左键弹起
		g_buttonType = 0;
		GUIDES->EndFrame();
		GUIDES->EndControl();
		//选择模式
		g_modelObj.InitSelect();
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushMatrix();			//model push
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//设置选择视景体
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
		//渲染选择模式
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluPerspective( 60.0f, (float)viewport[2]/(float)viewport[3], 0.1f, 1000.0f );
		CAMERA->Render();
		g_modelObj.Render(GL_SELECT);
		//恢复
		glPopMatrix();			//model pop
		hits = glRenderMode(GL_RENDER);
		g_modelObj.ProcessSelect(hits, bOneSel);
		if(hits > 0)
			GUIDES->InitControl(g_modelObj.GetSelectCenter().x, g_modelObj.GetSelectCenter().y, g_modelObj.GetSelectCenter().z);
		reshape(viewport[2], viewport[3]);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		break;
	case WM_MBUTTONUP:			//中键弹起
		g_buttonType = 0;
		break;
	case WM_RBUTTONUP:			//右键弹起
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