#include "Win32GL.h"
auto_ptr<CWin32GL> CWin32GL::m_pInst = auto_ptr<CWin32GL>();

CWin32GL::CWin32GL(void)
{
	m_hInstance = GetModuleHandle(NULL);
	m_hWnd = NULL;
	m_hDC = NULL;
	m_hRC = NULL;
	m_pRenderFunc = NULL;
	m_pReshapeFunc = NULL;
	m_pMouseFunc = NULL;
	m_pKeyFunc = NULL;
}

CWin32GL::~CWin32GL(void)
{
}

LRESULT CALLBACK CWin32GL::WinProc(	HWND hwnd,
							UINT uMsg,
							WPARAM wParam,
							LPARAM lParam	)
{
	switch(uMsg)
	{
	case WM_SIZE:
		if(WIN32GL->m_pReshapeFunc == NULL)
			WIN32GL->ResizeWindow( LOWORD(lParam), HIWORD(lParam) );
		else
			WIN32GL->m_pReshapeFunc( LOWORD(lParam), HIWORD(lParam) );
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSEMOVE:
		WIN32GL->m_pMouseFunc(uMsg, wParam, lParam);
	case WM_KEYDOWN:
		WIN32GL->m_pKeyFunc(wParam);
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

int CWin32GL::InitWindow(char *title, int w, int h)
{
	//窗口类
	WNDCLASSEX wc = {	sizeof(WNDCLASSEX),
						CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
						WinProc,
						0,
						0,
						m_hInstance,
						LoadIcon(NULL, IDI_WINLOGO),
						LoadCursor(NULL, IDC_ARROW),
						(HBRUSH)GetStockObject(BLACK_BRUSH),
						NULL,
						WINDOW_CLASS_NAME,
						NULL	};
	//注册窗口类
	if( !RegisterClassEx(&wc) )
	{
		cout << "窗口注册失败" << endl;
#ifdef DEBUG
		system("pause");
#endif
		return 0;
	}

	RECT windowRect = { (long)0, (long)0, (long)w, (long)h };
	AdjustWindowRectEx( &windowRect, WS_OVERLAPPEDWINDOW, FALSE, NULL );
	//创建窗口
	if( !(m_hWnd = CreateWindowEx(	NULL,
									WINDOW_CLASS_NAME,
									title,
									WS_OVERLAPPEDWINDOW,
									CW_USEDEFAULT, CW_USEDEFAULT,
									windowRect.right-windowRect.left,
									windowRect.bottom-windowRect.top,
									NULL,
									NULL,
									m_hInstance,
									NULL	)) )
	{
		cout << "创建窗口失败" << endl;
		KillWindow();
		return 0;
	}

	//获取DC
	if( !(m_hDC = GetDC(m_hWnd)) )
	{
		cout << "获取DC失败" << endl;
		KillWindow();
		return 0;
	}

	
    //初始化像素格式
	static PIXELFORMATDESCRIPTOR pfd =    
    {
		sizeof(PIXELFORMATDESCRIPTOR),    //上述格式描述符的大小
		1,                                //版本号
		PFD_DRAW_TO_WINDOW |              //格式支持窗口
		PFD_SUPPORT_OPENGL |              //格式必须支持OpenGL
		PFD_DOUBLEBUFFER,                 //必须支持双缓冲
		PFD_TYPE_RGBA,                    //申请RGBA格式
		24,                               //选定色彩深度
		0, 0, 0, 0, 0, 0, 0, 0,           //忽略RGBA
		0,                                //无累加缓存
		0, 0, 0, 0,                       //忽略聚集位
		32,                               //32位Z-缓存(深度缓存)
		0,                                //无蒙板缓存
		0,                                //无辅助缓存
		PFD_MAIN_PLANE,                   //主绘图层
		0,                                //Reserved
		0, 0, 0                           //忽略层遮罩
	};

	//选择一个最适合pfd描述的像素格式索引值
	int nPixelFormat;
	if( !(nPixelFormat = ChoosePixelFormat(m_hDC, &pfd)) )
	{
		cout << "无法找到像素格式" << endl;
		KillWindow();
		return 0;
	}
	
	//为DC设置像素格式
	if( !SetPixelFormat(m_hDC, nPixelFormat, &pfd) )
	{
		cout << "设置像素格式失败" << endl;
		KillWindow();
		return 0;
	}
	//创建RC并激活
	if( !(m_hRC = wglCreateContext(m_hDC)) )
	{
		cout << "创建RC失败" << endl;
		KillWindow();
		return 0;
	}
	if( !wglMakeCurrent(m_hDC, m_hRC) )
	{
		cout << "激活RC失败" << endl;
		KillWindow();
		return 0;
	}
	
	ShowWindow( m_hWnd, SW_SHOW );		//显示窗口
	SetForegroundWindow( m_hWnd );		//窗口置顶
	SetFocus( m_hWnd );					//设置键盘焦点

	return 1;
}

void CWin32GL::ResizeWindow(int w, int h)
{
	//防止被0除
	if( h == 0 )
		h = 1;
	glViewport( 0,0,w,h );
	//投影变换
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 60.0f, (float)w/(float)h, 0.1f, 1000.0f );
	//模型变换
	glMatrixMode(GL_MODELVIEW);
}

void CWin32GL::SetRender(pRFunc prf, int fps)
{
	m_pRenderFunc = prf;
	m_millispf = 1000/fps;
}
void CWin32GL::SetReshape(pWFunc pwf)
{
	m_pReshapeFunc = pwf;
}
void CWin32GL::SetMouseEvent(pMFunc pef)
{
	m_pMouseFunc = pef;
}
void CWin32GL::SetKeyEvent(pKFunc pkf)
{
	m_pKeyFunc = pkf;
}

void CWin32GL::MainLoop()
{
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );
	m_frametime = GetTickCount();
	while(1)
	{
			//消息循环
			if( PeekMessage(&msg, NULL, 0,0, PM_REMOVE) )
			{
				if( msg.message == WM_QUIT )
					break;
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			//控制帧数
			if( GetTickCount() - m_frametime > m_millispf )
			{
				//
				//在此使用OpenGL进行绘制
				//
				if(m_pRenderFunc != NULL)
				m_pRenderFunc();
				//交换缓冲区
				SwapBuffers(m_hDC);

				m_frametime = GetTickCount();
			}
			else
			{
				Sleep(1);
			}
	}
	KillWindow();
}

void CWin32GL::KillWindow()
{
	if(m_hRC)
	{
		if( !wglMakeCurrent(NULL, NULL) )
		{
			cout << "WGL释放DC和RC失败" << endl;
		}
		if( !wglDeleteContext(m_hRC) )
		{
			cout << "删除RC失败" << endl;
		}
		m_hRC = NULL;
	}
	if( m_hDC && !ReleaseDC(m_hWnd, m_hDC) )
	{
		cout << "释放DC失败" << endl;
		m_hDC = NULL;
	}
	if( m_hWnd && !DestroyWindow(m_hWnd) )
	{
		cout << "销毁HWND失败" << endl;
		m_hWnd = NULL;
	}
	if( !UnregisterClass(WINDOW_CLASS_NAME, m_hInstance) )
	{
		cout << "注销窗口类失败" << endl;
		m_hInstance = NULL;
	}
#ifdef DEBUG
		system("pause");
#endif
}