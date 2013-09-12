#pragma once
#include "stdafx.h"
#include <memory>

enum SEL_CTRL
{
	NONE,
	SEL_CX,
	SEL_CY,
	SEL_CZ
};

class CGuides
{
private:
	static auto_ptr<CGuides> m_pInst;
protected:
	CGuides();
public:
	~CGuides();

	static CGuides* Instance()
	{
		if( !m_pInst.get() )
		{
			m_pInst = auto_ptr<CGuides>( new CGuides() );
		}
		return m_pInst.get();
	}

	void Init(float range, float radius);
	void InitFrame(int l, int t);
	void DragFrame(int r, int b);
	void EndFrame(){	m_bFrame = false;	}
	void InitControl(float x, float y, float z);
	void EndControl(){	m_bControl = false;	}
	void Render();
	void InitSelect();
	void SelectControl();
	void ProcessSelect(int hits);

	void SetAspect(int w, int h){	m_aspect = (w==0) ? h*2.0f : h*2.0f/w;	}
	unsigned GetControl()const	{	return m_selCtrl;	}
private:
	float m_aspect;					//�ݺ��*2.0f
	//���
	bool m_bGirds;
	int m_scale;					//��߿̶ȴ�С
	int m_scaleNum;					//�̶ȸ���
	int m_scaleMid;					//���Ŀ̶�
	float m_gridOffset;				//���ƫ��
	//������
	bool m_bAxis;
	//ѡ���(Opengl����)
	bool m_bFrame;
	float m_fLeft;					//��
	float m_fTop;					//��
	float m_fRight;					//��
	float m_fButtom;				//��
	//���Ƹ�(Opengl����)
	bool m_bControl;
	float m_controlX;
	float m_controlY;
	float m_controlZ;
	float m_ctrlSize;				//���Ƹ˴�С
	//ѡ��
	GLuint m_selBuff[16];
	unsigned m_selCtrl;				//ѡ�������
};
#define GUIDES CGuides::Instance()