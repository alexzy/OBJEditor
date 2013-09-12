#include "Camera.h"
auto_ptr<CCamera> CCamera::m_pInst = auto_ptr<CCamera>();

CCamera::CCamera(void)
{
	//初始化相机参数
	//m_pos.x = 0.0f;
	//m_pos.y = 0.0f;
	//m_pos.z = -150.0f;
	//m_look.x = 0.0f;
	//m_look.y = 1.1f;
	//m_look.z = -100.0f;
	//m_up.x = 0.0f;
	//m_up.y = 1.0f;
	//m_up.z = 0.0f;
	m_distance = 50.0f;
	m_yawAngle = -45.0f;
	m_pitchAngle = 20.0f;
	m_oldYaw = 0.0f;
	m_oldPitch = 0.0f;

	//gluLookAt(m_pos.x, m_pos.y, m_pos.z, m_look.x, m_look.y, m_look.z, m_up.x, m_up.y, m_up.z);
}

CCamera::~CCamera(void)
{
}

void CCamera::Init(float dist)
{
	float tmpd = dist * 2;
	m_distance = (tmpd>50) ? tmpd : 50.0f;
}

void CCamera::Zoom(float z)
{
	m_distance += z;
	//限制范围
	if(m_distance<1)
	{
		m_distance = 1.0f;
	}
}

void CCamera::PreRotate()
{
	m_oldYaw = m_yawAngle;
	m_oldPitch = m_pitchAngle;
}

void CCamera::Rotate(int dx, int dy)
{
	m_yawAngle = m_oldYaw + (float)dx/5.0f;
	m_pitchAngle = m_oldPitch + (float)dy/5.0f;
	//限制范围
	if(m_pitchAngle>89)
		m_pitchAngle = 89.0f;
	if(m_pitchAngle<-89)
		m_pitchAngle = -89.0f;
	if(m_yawAngle<-180)
		m_yawAngle += 360.0f;
	if(m_yawAngle>180)
		m_yawAngle -= 360.0f;
}

void CCamera::Render()
{
	//平移后旋转
	glTranslatef(0.0f, 0.0f, 0.0f-m_distance);
	glRotatef(m_pitchAngle, 1.0f, 0.0f, 0.0f);
	glRotatef(m_yawAngle, 0.0f, 1.0f, 0.0f);
}