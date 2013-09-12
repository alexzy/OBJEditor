//一个较为通用的OpenGL纹理读取类 by zhengyue
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <memory>
#include <list>
#include <string>
using namespace std;

#include <windows.h>
#include <GL\gl.h>
#include <GL\glu.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

class CTexture
{
public:
	CTexture(){}
	CTexture(const char *name){	m_name = name;	}
	CTexture(unsigned int id, const char *name){	m_id = id;	m_name = name;	}
	~CTexture(){}//glDeleteTextures(1, &m_id);	m_name.erase();

	friend class CTextureManager;
	unsigned int GetId()const{	return m_id;	}
	const char *GetName()const{	return m_name.c_str();	}
private:
	unsigned int m_id;			//纹理ID
	string m_name;				//纹理名
};

class CTextureManager
{
private:
	static auto_ptr<CTextureManager> m_pInst;
protected:
	CTextureManager(){	Initialize();	}
public:
	~CTextureManager(){}//	CleanAllTextures();	

	static CTextureManager* Instance()
	{
		if( !m_pInst.get() )
		{
			m_pInst = auto_ptr<CTextureManager>( new CTextureManager() );
		}
		return m_pInst.get();
	}
	
	unsigned int LoadTexture(const char *fileName);
	void DeleteTexture(unsigned int id);	
private:
	void Initialize();
	void CleanAllTextures();

	list<CTexture*> m_texlist;	//纹理列表
};
#define TEXMGR CTextureManager::Instance()
#endif