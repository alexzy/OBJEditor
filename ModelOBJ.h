#pragma once
#include "stdafx.h"

#include "Type.h"
#include "Texture.h"
#include <map>

//ѡ��ģʽ
enum SEL_MODE
{
	GROUP_SEL,
	VERTEX_SEL
};
// ��ѡ���ʶ�Ķ���
struct VERTEX : public VEC_3
{
	bool selected;
};
// ���еĶ�������
struct VERTEX_INDEX
{
	unsigned vi;
	unsigned ti;
	unsigned ni;
};
// ��
struct GROUP
{
	string name;
	string mtlName;
	vector<vector<VERTEX_INDEX>> faces;
	bool selected;
};
// ����������ID
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
	string m_filePath;						//�ļ�·��
	vector<VERTEX> m_vertices;				//��������
	vector<VEC_2> m_texCoords;				//��������
	vector<VEC_3> m_normals;				//��������
	vector<GROUP> m_model;					//ģ��
	map<string, MATERIAL> m_materials;		//����ӳ��
	string m_mtlPath;						//����·��

	bool m_bWireframe;						//�Ƿ��߿�ģʽ
	bool m_bShadow;							//�Ƿ�����Ӱ
	bool m_bTexture;						//�Ƿ��������

	float m_range;							//��ռ��������Χ
	float m_radius;							//ģ�Ͱ�Χ��뾶
	vector<GLuint> m_selBuff;				//ѡ��buffer
	SEL_MODE m_selMode;						//ѡ���黹�Ƕ���
	VEC_3 m_selCenter;						//ѡ������ĵ�

	VEC_3 m_moveDelta;						//�ƶ�����
};