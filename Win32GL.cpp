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
	//������
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
	//ע�ᴰ����
	if( !RegisterClassEx(&wc) )
	{
		cout << "����ע��ʧ��" << endl;
#ifdef DEBUG
		system("pause");
#endif
		return 0;
	}

	RECT windowRect = { (long)0, (long)0, (long)w, (long)h };
	AdjustWindowRectEx( &windowRect, WS_OVERLAPPEDWINDOW, FALSE, NULL );
	//��������
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
		cout << "��������ʧ��" << endl;
		KillWindow();
		return 0;
	}

	//��ȡDC
	if( !(m_hDC = GetDC(m_hWnd)) )
	{
		cout << "��ȡDCʧ��" << endl;
		KillWindow();
		return 0;
	}

	
    //��ʼ�����ظ�ʽ
	static PIXELFORMATDESCRIPTOR pfd =    
    {
		sizeof(PIXELFORMATDESCRIPTOR),    //������ʽ�������Ĵ�С
		1,                                //�汾��
		PFD_DRAW_TO_WINDOW |              //��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |              //��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER,                 //����֧��˫����
		PFD_TYPE_RGBA,                    //����RGBA��ʽ
		24,                               //ѡ��ɫ�����
		0, 0, 0, 0, 0, 0, 0, 0,           //����RGBA
		0,                                //���ۼӻ���
		0, 0, 0, 0,                       //���Ծۼ�λ
		32,                               //32λZ-����(��Ȼ���)
		0,                                //���ɰ建��
		0,                                //�޸�������
		PFD_MAIN_PLANE,                   //����ͼ��
		0,                                //Reserved
		0, 0, 0                           //���Բ�����
	};

	//ѡ��һ�����ʺ�pfd���������ظ�ʽ����ֵ
	int nPixelFormat;
	if( !(nPixelFormat = ChoosePixelFormat(m_hDC, &pfd)) )
	{
		cout << "�޷��ҵ����ظ�ʽ" << endl;
		KillWindow();
		return 0;
	}
	
	//ΪDC�������ظ�ʽ
	if( !SetPixelFormat(m_hDC, nPixelFormat, &pfd) )
	{
		cout << "�������ظ�ʽʧ��" << endl;
		KillWindow();
		return 0;
	}
	//����RC������
	if( !(m_hRC = wglCreateContext(m_hDC)) )
	{
		cout << "����RCʧ��" << endl;
		KillWindow();
		return 0;
	}
	if( !wglMakeCurrent(m_hDC, m_hRC) )
	{
		cout << "����RCʧ��" << endl;
		KillWindow();
		return 0;
	}
	
	ShowWindow( m_hWnd, SW_SHOW );		//��ʾ����
	SetForegroundWindow( m_hWnd );		//�����ö�
	SetFocus( m_hWnd );					//���ü��̽���

	return 1;
}

void CWin32GL::ResizeWindow(int w, int h)
{
	//��ֹ��0��
	if( h == 0 )
		h = 1;
	glViewport( 0,0,w,h );
	//ͶӰ�任
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 60.0f, (float)w/(float)h, 0.1f, 1000.0f );
	//ģ�ͱ任
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
			//��Ϣѭ��
			if( PeekMessage(&msg, NULL, 0,0, PM_REMOVE) )
			{
				if( msg.message == WM_QUIT )
					break;
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			//����֡��
			if( GetTickCount() - m_frametime > m_millispf )
			{
				//
				//�ڴ�ʹ��OpenGL���л���
				//
				if(m_pRenderFunc != NULL)
				m_pRenderFunc();
				//����������
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
			cout << "WGL�ͷ�DC��RCʧ��" << endl;
		}
		if( !wglDeleteContext(m_hRC) )
		{
			cout << "ɾ��RCʧ��" << endl;
		}
		m_hRC = NULL;
	}
	if( m_hDC && !ReleaseDC(m_hWnd, m_hDC) )
	{
		cout << "�ͷ�DCʧ��" << endl;
		m_hDC = NULL;
	}
	if( m_hWnd && !DestroyWindow(m_hWnd) )
	{
		cout << "����HWNDʧ��" << endl;
		m_hWnd = NULL;
	}
	if( !UnregisterClass(WINDOW_CLASS_NAME, m_hInstance) )
	{
		cout << "ע��������ʧ��" << endl;
		m_hInstance = NULL;
	}
#ifdef DEBUG
		system("pause");
#endif
}