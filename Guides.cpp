#include "Guides.h"
auto_ptr<CGuides> CGuides::m_pInst = auto_ptr<CGuides>();

CGuides::CGuides()
{
	m_aspect = 2.0f;

	m_bGirds = true;
	m_scale = 5;
	m_scaleNum = 21;
	m_scaleMid = 10;
	m_gridOffset = 50;

	m_bAxis = true;

	m_bFrame = false;
	m_fLeft = 0;
	m_fTop = 0;
	m_fRight = 0;
	m_fButtom = 0;

	m_bControl = false;
	m_controlX = 0;
	m_controlY = 0;
	m_controlZ = 0;
	m_ctrlSize = 5.0f;

	m_selCtrl = NONE;
}
CGuides::~CGuides()
{

}

void CGuides::Init(float range, float radius)
{
	//自适应大小
	if(range>50)
	{
		m_scale = 10;
		m_scaleNum = (int)(range*2/10);
		m_scaleNum = (m_scaleNum%2==0) ? m_scaleNum+1 : m_scaleNum+2;
		m_scaleMid = (m_scaleNum-1)/2;
		m_gridOffset = m_scaleMid*m_scale*1.0f;
	}
	if(radius > 10)
		m_ctrlSize = radius;
}

void CGuides::InitFrame(int l, int t)
{
	m_bFrame = true;
	//屏幕坐标转换为OpenGL坐标
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	m_fLeft = l*2.0f/(viewport[2]-viewport[0]) - 1.0f;
	m_fTop = 1.0f - t*2.0f/(viewport[3]-viewport[1]);
	m_fRight = m_fLeft;
	m_fButtom = m_fTop;
}
void CGuides::DragFrame(int r, int b)
{
	//屏幕坐标转换为OpenGL坐标
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	m_fRight = r*2.0f/(viewport[2]-viewport[0]) - 1.0f;
	m_fButtom =  1.0f - b*2.0f/(viewport[3]-viewport[1]);
}

void CGuides::InitControl(float x, float y, float z)
{
	m_bControl = true;
	m_controlX = x;
	m_controlY = y;
	m_controlZ = z;
}

void CGuides::Render()
{
	glDisable(GL_LIGHTING);		//不受光照影响
	glDisable(GL_TEXTURE_2D);	//不受纹理影响
	//网格
	if(m_bGirds)
	{
		for(int i=0; i<m_scaleNum; i++)
		{
			glLineWidth(1.0f);
			if(i == m_scaleMid)
			{
				glColor3f(0.0f, 0.0f, 0.0f);
			}
			else
			{
				glColor3f(1.0f, 1.0f, 1.0f);
			}
			float start = (float)((i-m_scaleMid) * m_scale);
			glBegin(GL_LINES);
			glVertex3f(start, 0.0f, 0.0f-m_gridOffset);
			glVertex3f(start, 0.0f, m_gridOffset);
			glVertex3f(0.0f-m_gridOffset, 0.0f, start);
			glVertex3f(m_gridOffset, 0.0f, start);
			glEnd();
		}
	}

	//坐标轴
	if(m_bAxis)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1.0, 1.0, -1.0, m_aspect-1.0, 0.0, 1000.0);
		//移至左下角
		glTranslatef(-0.8f, -0.8f, 0.0f);
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		//X轴
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.1f, 0.0f, 0.0f);
		//Y轴
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.1f, 0.0f);
		//Z轴
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.1f);
		glEnd();
		glPopMatrix();
	}
	//控制杆
	if(m_bControl)
	{
		//关闭深度缓存使控制杆不被模型遮挡
		glDisable(GL_DEPTH_TEST);
		glLineWidth(3.0f);
		
		//X轴
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(m_controlX, m_controlY, m_controlZ);
		glVertex3f(m_controlX+m_ctrlSize, m_controlY, m_controlZ);
		//Y轴
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(m_controlX, m_controlY, m_controlZ);
		glVertex3f(m_controlX, m_controlY+m_ctrlSize, m_controlZ);
		//Z轴
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(m_controlX, m_controlY, m_controlZ);
		glVertex3f(m_controlX, m_controlY, m_controlZ+m_ctrlSize);
		glEnd();


		glEnable(GL_DEPTH_TEST);
	}


	//GUI环境
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 1000.0);
	//选择框
	if(m_bFrame)
	{
		glLineWidth(2.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINE_STRIP);
		glVertex3f(m_fLeft, m_fTop, 0.0f);
		glVertex3f(m_fRight, m_fTop, 0.0f);
		glVertex3f(m_fRight, m_fButtom, 0.0f);
		glVertex3f(m_fLeft, m_fButtom, 0.0f);
		glVertex3f(m_fLeft, m_fTop, 0.0f);
		glEnd();
	}
	glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
	//endGUI
	
	
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
}

void CGuides::InitSelect()
{
	glSelectBuffer(16, m_selBuff);
}
void CGuides::SelectControl()
{
	//X轴
	glPushName(SEL_CX);
	glBegin(GL_LINES);
	glVertex3f(m_controlX, m_controlY, m_controlZ);
	glVertex3f(m_controlX+m_ctrlSize, m_controlY, m_controlZ);
	glEnd();
	glPopName();
	//Y轴
	glPushName(SEL_CY);
	glBegin(GL_LINES);
	glVertex3f(m_controlX, m_controlY, m_controlZ);
	glVertex3f(m_controlX, m_controlY+m_ctrlSize, m_controlZ);
	glEnd();
	glPopName();
	//Z轴
	glPushName(SEL_CZ);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(m_controlX, m_controlY, m_controlZ);
	glVertex3f(m_controlX, m_controlY, m_controlZ+m_ctrlSize);
	glEnd();
	glPopName();

}
void CGuides::ProcessSelect(int hits)
{
	m_selCtrl = NONE;
	if(hits > 0)
	{
		int n = 0;
		double minz = m_selBuff[1];
		for(int i=0; i<hits; i++)
		{
			if( m_selBuff[1+i*4] < minz ) 
			{
				n = i;
				minz = m_selBuff[1+i*4];
			}
		}
		m_selCtrl = m_selBuff[3+n*4];
	}
}