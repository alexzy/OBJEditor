#pragma once
#include "stdafx.h"

#include "Type.h"
#include "Texture.h"
#include <map>

//选择模式
enum SEL_MODE
{
	GROUP_SEL,
	VERTEX_SEL
};
// 带选择标识的顶点
struct VERTEX : public VEC_3
{
	bool selected;
};
// 面中的顶点索引
struct VERTEX_INDEX
{
	unsigned vi;
	unsigned ti;
	unsigned ni;
};
// 组
struct GROUP
{
	string name;
	string mtlName;
	vector<vector<VERTEX_INDEX>> faces;
	bool selected;
};
// 材质与纹理ID
struct MATERIAL
{
	MATERIAL()
	{
		bKd = bKa = bKs = bNs = false;
	}
	bool bKd;
	bool bKa;
	bool bKs;
	bool bNs;
	float kdiff[4];
	float kambi[4];
	float kspec[4];
	float nshinness[1];
	unsigned mapKd;
};

class CModelOBJ
{
public:
	CModelOBJ();
	~CModelOBJ();

	bool LoadFile(string fileName);
	bool LoadMaterial();
	void Render(GLenum mode=GL_RENDER);
	void InitSelect();
	void ProcessSelect(int hits, bool bOneSel);
	void InitDrag();
	void DragSelect(float dist, int ctrl);
	void EndDrag();

	void EnWireFrame(const bool bWF)	{	m_bWireframe = bWF;			}
	void SwitchShadow()					{	m_bShadow = !m_bShadow;		}
	void EnTexture(const bool bTex)		{	m_bTexture = bTex;			}
	void SwitchTexture()				{	m_bTexture = !m_bTexture;	}
	void SetSelect(const SEL_MODE mode)	{	m_selMode = mode;			}
	VEC_3 GetSelectCenter()const		{	return m_selCenter;			}
	float GetRadius()const				{	return m_radius;			}
	float GetRange()const				{	return m_range;				}
private:
	string m_filePath;						//文件路径
	vector<VERTEX> m_vertices;				//顶点坐标
	vector<VEC_2> m_texCoords;				//纹理坐标
	vector<VEC_3> m_normals;				//法向坐标
	vector<GROUP> m_model;					//模型
	map<string, MATERIAL> m_materials;		//纹理映射
	string m_mtlPath;						//纹理路径

	bool m_bWireframe;						//是否线框模式
	bool m_bShadow;							//是否有阴影
	bool m_bTexture;						//是否绘制纹理

	float m_range;							//所占网格的最大范围
	float m_radius;							//模型包围球半径
	vector<GLuint> m_selBuff;				//选择buffer
	SEL_MODE m_selMode;						//选择组还是顶点
	VEC_3 m_selCenter;						//选择的中心点

	VEC_3 m_moveDelta;						//移动增量
};