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
	float m_aspect;					//纵横比*2.0f
	//标尺
	bool m_bGirds;
	int m_scale;					//标尺刻度大小
	int m_scaleNum;					//刻度个数
	int m_scaleMid;					//中心刻度
	float m_gridOffset;				//标尺偏移
	//坐标轴
	bool m_bAxis;
	//选择框(Opengl坐标)
	bool m_bFrame;
	float m_fLeft;					//左
	float m_fTop;					//上
	float m_fRight;					//右
	float m_fButtom;				//下
	//控制杆(Opengl坐标)
	bool m_bControl;
	float m_controlX;
	float m_controlY;
	float m_controlZ;
	float m_ctrlSize;				//控制杆大小
	//选择
	GLuint m_selBuff[16];
	unsigned m_selCtrl;				//选择的轴向
};
#define GUIDES CGuides::Instance()