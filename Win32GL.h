#pragma once

#ifndef DEBUG
#define DEBUG
#endif

#ifndef DEBUG
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif
#include <iostream>
using namespace std;

//window
#include <windows.h>
#include <memory>
#include <time.h>
#define WINDOW_CLASS_NAME		"OPENGL_WINDOW"

//opengl
#include <GL\gl.h>
#include <GL\glu.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

typedef void (*pRFunc)();
typedef void (*pWFunc)(int w, int h);
typedef void (*pMFunc)(UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef void (*pKFunc)(UINT key);
class CWin32GL
{
private:
	static auto_ptr<CWin32GL> m_pInst;	
protected:
	CWin32GL(void);
public:
	~CWin32GL(void);

	static CWin32GL* Instance()
	{
		if( !m_pInst.get() )
		{
			m_pInst = auto_ptr<CWin32GL>( new CWin32GL() );
		}
		return m_pInst.get();
	}

	static LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int InitWindow(char *title, int w, int h);
	void ResizeWindow(int w, int h);
	void MainLoop();
	void KillWindow();

	void SetRender(pRFunc prf=NULL, int fps=48);
	void SetReshape(pWFunc pwf=NULL);
	void SetMouseEvent(pMFunc pef=NULL);
	void SetKeyEvent(pKFunc pkf=NULL);
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HDC m_hDC;
	HGLRC m_hRC;

	pRFunc m_pRenderFunc;
	pWFunc m_pReshapeFunc;
	pMFunc m_pMouseFunc;
	pKFunc m_pKeyFunc;
	DWORD m_frametime;
	DWORD m_millispf;
};
#define WIN32GL CWin32GL::Instance()