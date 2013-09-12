#pragma once
#include "stdafx.h"
//#include "Type.h"
#include <memory>

class CCamera
{
private:
	static auto_ptr<CCamera> m_pInst;
protected:
	CCamera();
public:
	~CCamera();

	static CCamera* Instance()
	{
		if( !m_pInst.get() )
		{
			m_pInst = auto_ptr<CCamera>( new CCamera() );
		}
		return m_pInst.get();
	}

	void Init(float dist);
	void Zoom(float z);
	void PreRotate();
	void Rotate(int dx, int dy);
	void Render();

	float GetDist()const	{	return m_distance;		}
	float GetYaw()const		{	return m_yawAngle;		}
	float GetPitch()const	{	return m_pitchAngle;	}
private:
	float m_distance;
	float m_yawAngle;
	float m_pitchAngle;

	float m_oldYaw;
	float m_oldPitch;

	//VEC_3 m_pos;
	//VEC_3 m_look;
	//VEC_3 m_up;
};
#define CAMERA CCamera::Instance()