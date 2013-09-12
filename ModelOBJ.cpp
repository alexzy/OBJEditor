#include "ModelOBJ.h"

CModelOBJ::CModelOBJ()
{
	m_bWireframe = false;
	m_bShadow = false;
	m_bTexture = true;
	m_range = 0;
	m_radius = 0;
	m_selMode = GROUP_SEL;
	m_selCenter.x = 0;	m_selCenter.y = 0;	m_selCenter.z = 0;
	m_moveDelta.x = 0;	m_moveDelta.y = 0;	m_moveDelta.z = 0;	
}
CModelOBJ::~CModelOBJ()
{

}

bool CModelOBJ::LoadFile(string fileName)
{
	ifstream sfile(fileName.c_str(), ios::in);
	if(sfile.fail())
	{
		cout<<"打开"<<fileName<<"失败！"<<endl;
		return false;
	}
	//获取文件路径
	int slash = fileName.rfind("\\");
	m_filePath = fileName.substr(0,slash+1);
	string line;
	//初始化一个组
	GROUP group;
	group.selected = false;
	//记录顶点范围
	float xMax, yMax, zMax;
	float xMin, yMin, zMin;
	bool bFirstVert = true;
	//读每行
	while(getline(sfile, line))
	{
		istringstream sline(line);
		string head;
		sline >> head;
		if(head == "mtllib")	//材质路径
		{
			sline >> m_mtlPath;
			m_mtlPath = m_filePath + m_mtlPath;
		}
		if(head == "g")	//组
		{
			if(group.name == "")
			{
				sline >> group.name;
			}
			else
			{
				string tmpGn;
				sline >> tmpGn;
				//将group加入模型数组，并重置
				if(group.name != tmpGn)
				{
					m_model.push_back(group);
					group.name = tmpGn;
					group.faces.clear();
				}
			}
		}
		if(head == "v")			//顶点坐标
		{
			VERTEX tmpV;
			sline >> tmpV.x >> tmpV.y >> tmpV.z;
			tmpV.selected = false;
			m_vertices.push_back(tmpV);
			//记录包围球
			if(bFirstVert)
			{
				xMax = tmpV.x;
				xMin = tmpV.x;
				yMax = tmpV.y;
				yMin = tmpV.y;
				zMax = tmpV.z;
				zMin = tmpV.z;
				bFirstVert = false;
			}
			else
			{
				if(tmpV.x > xMax)
					xMax = tmpV.x;
				if(tmpV.x < xMin)
					xMin = tmpV.x;
				if(tmpV.y > yMax)
					yMax = tmpV.y;
				if(tmpV.y < yMin)
					yMin = tmpV.y;
				if(tmpV.z > zMax)
					zMax = tmpV.z;
				if(tmpV.z < zMin)
					zMin = tmpV.z;
			}
			//顶点所占网格的最大范围
			float rx = fabs(tmpV.x);
			float ry = fabs(tmpV.y);
			float rz = fabs(tmpV.z);
			if(rx > m_range)
				m_range = rx;
			if(ry > m_range)
				m_range = ry;
			if(rz > m_range)
				m_range = rz;
		}
		if(head == "vt")		//纹理坐标
		{
			VEC_2 tmpVt;
			sline >> tmpVt.x >> tmpVt.y;
			m_texCoords.push_back(tmpVt);
		}
		if(head == "vn")		//法向
		{
			VEC_3 tmpVn;
			sline >> tmpVn.x >> tmpVn.y >> tmpVn.z;
			m_normals.push_back(tmpVn);
		}
		if(head == "usemtl")	//使用纹理名
		{
			sline >> group.mtlName;	
		}
		if(head == "f")			//面
		{
			string tmpFv;
			vector<VERTEX_INDEX> face;
			VERTEX_INDEX vertIndex;
			while(sline >> tmpFv)
			{
				int slash = tmpFv.find("/");
				int vIndex=0, vtIndex=0, vnIndex=0;
				if(slash == -1)				//"顶点"
				{
					vIndex = atoi(tmpFv.c_str());
				}
				else						//"顶点/？"
				{
					vIndex = atoi(tmpFv.substr(0,slash).c_str());
					tmpFv = tmpFv.substr(slash+1);
					slash = tmpFv.find("/");
					if(slash == -1)			//"顶点/纹理"
					{
						vtIndex = atoi(tmpFv.c_str());
					}
					else if(slash == 0)		//"顶点//法向"
					{
						vnIndex = atoi(tmpFv.substr(1).c_str());
					}
					else					//"顶点/纹理/法向"
					{
						vtIndex = atoi(tmpFv.substr(0,slash).c_str());
						vnIndex = atoi(tmpFv.substr(slash+1).c_str());
					}
				}
				//根据索引创建完整的顶点信息
				vIndex--;	vtIndex--;	vnIndex--;
				int vMax = m_vertices.size()-1;
				int vtMax = m_texCoords.size()-1;
				int vnMax = m_normals.size()-1;
				if(vIndex<0 || vIndex>vMax || vtIndex<0 || vtIndex>vtMax || vnIndex<0 || vnIndex>vnMax)
				{
					cout<<"面索引越界或暂不支持此OBJ格式！"<<endl;
					return false;
				}
				vertIndex.vi = vIndex;
				vertIndex.ti = vtIndex;
				vertIndex.ni = vnIndex;
				face.push_back(vertIndex);
			}
			group.faces.push_back(face);
		}//endif
	}//endwhile
	m_model.push_back(group);
	
	//计算包围球半径
	float width = (xMax-xMin)/2;
	float height = (yMax-yMin)/2;
	float depth = (zMax-zMin)/2;
	m_radius = (width>height) ? ((width>depth)?width:depth) : ((height>depth)?height:depth);

	//初始化选择表
	int selsize = (m_vertices.size()+10)*4;
	for(int i=0; i!=selsize; ++i)
	{
		GLuint tmp;
		m_selBuff.push_back(tmp);
	}
	return true;
}

bool CModelOBJ::LoadMaterial()
{
	ifstream sfile(m_mtlPath.c_str(), ios::in);
	if(sfile.fail())
	{
		cout<<"打开"<<m_mtlPath<<"失败！"<<endl;
		return false;
	}
	string line;
	string mtlName;
	while(getline(sfile, line))
	{
		istringstream sline(line);
		string head;
		sline >> head;
		if(head == "newmtl")
		{
			sline >> mtlName;
		}
		if(head == "Kd")
		{
			m_materials[mtlName].bKd = true;
			sline >> m_materials[mtlName].kdiff[0] >> m_materials[mtlName].kdiff[1] >> m_materials[mtlName].kdiff[2];
			m_materials[mtlName].kdiff[3] = 1.0f;
		}
		if(head == "Ka")
		{
			m_materials[mtlName].bKa = true;
			sline >> m_materials[mtlName].kambi[0] >> m_materials[mtlName].kambi[1] >> m_materials[mtlName].kambi[2];
			m_materials[mtlName].kambi[3] = 1.0f;
		}
		if(head == "Ks")
		{
			m_materials[mtlName].bKs = true;
			sline >> m_materials[mtlName].kspec[0] >> m_materials[mtlName].kspec[1] >> m_materials[mtlName].kspec[2];
			m_materials[mtlName].kspec[3] = 1.0f;
		}
		if(head == "Ns")
		{
			m_materials[mtlName].bNs = true;
			float tmpNs;
			sline >> tmpNs;
			tmpNs = tmpNs/1000.0f*128.0f;
			m_materials[mtlName].nshinness[0] = tmpNs;
		}
		if(head == "map_Kd")
		{
			string tmpTexKd;
			sline >> tmpTexKd;
			tmpTexKd = m_filePath + tmpTexKd;
			m_materials[mtlName].mapKd = TEXMGR->LoadTexture(tmpTexKd.c_str());
		}

	}//endwhile
	return true;
}

void CModelOBJ::Render(GLenum mode)
{
	if(mode == GL_RENDER)
	{
		//设置显示方式
		if(m_bWireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_LIGHTING);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_LIGHTING);
		}
		if(m_bShadow)
		{
			glEnable(GL_LIGHT1);
			glDisable(GL_LIGHT0);
		}
		else
		{
			glEnable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
		}
		if(m_bTexture)
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);
		//遍历
		int groupNum = m_model.size();
		for(int i=0; i!=groupNum; ++i)
		{
			if(!m_bWireframe)
			{
				//设置材质
				if(m_materials[m_model[i].mtlName].bKa)
					glMaterialfv(GL_FRONT, GL_AMBIENT, m_materials[m_model[i].mtlName].kambi);
				if(m_materials[m_model[i].mtlName].bKd)
					glMaterialfv(GL_FRONT, GL_DIFFUSE, m_materials[m_model[i].mtlName].kdiff);
				if(m_materials[m_model[i].mtlName].bKs)
					glMaterialfv(GL_FRONT, GL_SPECULAR, m_materials[m_model[i].mtlName].kspec);
				if(m_materials[m_model[i].mtlName].bNs)
					glMaterialfv(GL_FRONT, GL_SHININESS, m_materials[m_model[i].mtlName].nshinness);
				//绑定纹理
				glBindTexture( GL_TEXTURE_2D, m_materials[m_model[i].mtlName].mapKd );
			}
			if(m_selMode == GROUP_SEL)
			{
				if(m_model[i].selected)
				{
					glColor3f(0.0f, 1.0f, 1.0f);
					glLineWidth(2.0f);
				}
				else if(m_bWireframe)
				{
					glColor3f(0.0f, 0.0f, 0.3f);
					glLineWidth(1.0f);
				}
			}
			int faceNum = m_model[i].faces.size();
			for(int j=0; j!=faceNum; ++j)
			{
				//选择绘制模式
				int vertNum = m_model[i].faces[j].size();
				if(vertNum == 3)
					glBegin(GL_TRIANGLES);
				if(vertNum == 4)
					glBegin(GL_QUADS);
				if(vertNum > 4)
					glBegin(GL_POLYGON);
				for(int k=0; k!=vertNum; ++k)
				{
					if(!m_bWireframe)
					{
						VEC_2 tmpVt = m_texCoords[m_model[i].faces[j][k].ti];
						VEC_3 tmpVn = m_normals[m_model[i].faces[j][k].ni];
						glTexCoord2f( tmpVt.x, tmpVt.y );
						glNormal3f( tmpVn.x, tmpVn.y, tmpVn.z );
					}
					VERTEX tmpV = m_vertices[m_model[i].faces[j][k].vi];
					if(tmpV.selected)
					{
						tmpV.x += m_moveDelta.x;
						tmpV.y += m_moveDelta.y;
						tmpV.z += m_moveDelta.z;
					}
					glVertex3f( tmpV.x, tmpV.y, tmpV.z );
				}
				glEnd();
				//选中且非线框模式，需要再画一遍框
				if(m_model[i].selected && !m_bWireframe)
				{
					glDisable(GL_TEXTURE_2D);
					glDisable(GL_LIGHTING);
					glBegin(GL_LINE_LOOP);
					for(int k=0; k!=vertNum; ++k)
					{
						VERTEX tmpV = m_vertices[m_model[i].faces[j][k].vi];
						if(tmpV.selected)
						{
							tmpV.x += m_moveDelta.x;
							tmpV.y += m_moveDelta.y;
							tmpV.z += m_moveDelta.z;
						}
						glVertex3f( tmpV.x, tmpV.y, tmpV.z );
					}
					glEnd();
					glEnable(GL_LIGHTING);
					if(m_bTexture)
						glEnable(GL_TEXTURE_2D);						
				}
			}//endfor face
		}//endfor group
		//将选中顶点绘制成红色（测试）
		//glDisable(GL_TEXTURE_2D);
		//glDisable(GL_LIGHTING);
		//glColor3f(1.0f, 0.0f, 0.0f);
		//glPointSize(5.0f);
		//glBegin(GL_POINTS);
		//int vNum = m_vertices.size();
		//for(int i=0; i!=vNum; ++i)
		//{
		//	if(m_vertices[i].selected)
		//		glVertex3f( m_vertices[i].x, m_vertices[i].y, m_vertices[i].z );
		//}
		//glEnd();
	}//endif render
	if(mode == GL_SELECT)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		int groupNum = m_model.size();
		for(int i=0; i!=groupNum; ++i)
		{
			if(m_selMode == GROUP_SEL)
				glPushName(i);
			int faceNum = m_model[i].faces.size();
			for(int j=0; j!=faceNum; ++j)
			{
				//选择绘制模式
				int vertNum = m_model[i].faces[j].size();
				if(vertNum == 3)
					glBegin(GL_TRIANGLES);
				if(vertNum == 4)
					glBegin(GL_QUADS);
				if(vertNum > 4)
					glBegin(GL_POLYGON);
				for(int k=0; k!=vertNum; ++k)
				{
					VEC_3 tmpV = m_vertices[m_model[i].faces[j][k].vi];
					glVertex3f( tmpV.x, tmpV.y, tmpV.z );		
				}
				glEnd();
			}
			if(m_selMode == GROUP_SEL)
				glPopName();
		}
	}//endif select

}

void CModelOBJ::InitSelect()
{
	glSelectBuffer(m_selBuff.size(), &m_selBuff[0]);
}
void CModelOBJ::ProcessSelect(int hits, bool bOneSel)
{
	//重置选择信息
	m_selCenter.x = 0;	m_selCenter.y = 0;	m_selCenter.z = 0;
	int vNum = m_vertices.size();
	for(int i=0; i!=vNum; ++i)
	{
		m_vertices[i].selected = false;
	}
	//当hits为1选择有最小z的，当hits不为1选择所有
	if(m_selMode == GROUP_SEL)
	{
		vector<int> selGroups;
		if(hits > 0)
		{
			int ming = 0;
			double minz = m_selBuff[1];
			for(int i=0; i<hits; i++)
			{
				if( m_selBuff[1+i*4] < minz ) 
				{
					ming = i;
					minz = m_selBuff[1+i*4];
				}
				if(!bOneSel)
				{
					selGroups.push_back(m_selBuff[3+i*4]);
				}
			}
			if(bOneSel)
			{
				selGroups.push_back(m_selBuff[3+ming*4]);
			}
		}
		//计算影响的组和顶点
		int groupNum = m_model.size();
		for(int i=0; i!=groupNum; ++i)
		{
			m_model[i].selected = false;
			for(int s=0; s!=selGroups.size(); ++s)
			{
				if(i == selGroups[s])
					m_model[i].selected = true;
			}
			if(m_model[i].selected == true)
			{
				int faceNum = m_model[i].faces.size();
				for(int j=0; j!=faceNum; ++j)
				{
					int vertNum = m_model[i].faces[j].size();
					for(int k=0; k!=vertNum; ++k)
					{
						m_vertices[m_model[i].faces[j][k].vi].selected = true;
					}
				}//endfor face
			}//endif selected
		}//endfor group
		if(hits > 0)
		{
			//计算选择中心点
			int selVertNum = 0;
			for(int i=0; i!=vNum; ++i)
			{
				if(m_vertices[i].selected)
				{
					selVertNum ++;
					m_selCenter.x += m_vertices[i].x;
					m_selCenter.y += m_vertices[i].y;
					m_selCenter.z += m_vertices[i].z;
				}
			}
			m_selCenter.x /= selVertNum;
			m_selCenter.y /= selVertNum;
			m_selCenter.z /= selVertNum;
			//float xMax, yMax, zMax;
			//float xMin, yMin, zMin;
			//bool bFirstSel = true;
			//for(int i=1; i<vNum; ++i)
			//{
			//	if(m_vertices[i].selected)
			//	{
			//		if(bFirstSel)
			//		{
			//			xMax = m_vertices[i].x;
			//			xMin = m_vertices[i].x;
			//			yMax = m_vertices[i].y;
			//			yMin = m_vertices[i].y;
			//			zMax = m_vertices[i].z;
			//			zMin = m_vertices[i].z;
			//			bFirstSel = false;
			//		}
			//		else
			//		{
			//			if(m_vertices[i].x > xMax)
			//				xMax = m_vertices[i].x;
			//			if(m_vertices[i].x < xMin)
			//				xMin = m_vertices[i].x;
			//			if(m_vertices[i].y > yMax)
			//				yMax = m_vertices[i].y;
			//			if(m_vertices[i].y < yMin)
			//				yMin = m_vertices[i].y;
			//			if(m_vertices[i].z > zMax)
			//				zMax = m_vertices[i].z;
			//			if(m_vertices[i].z < zMin)
			//				zMin = m_vertices[i].z;
			//		}
			//	}//endif selected
			//}//endfor
			//float width = xMax-xMin;
			//float height = yMax-yMin;
			//float depth = zMax-zMin;
			//m_selCenter.x = xMin + width/2;
			//m_selCenter.y = yMin + height/2;
			//m_selCenter.z = zMin + depth/2;
		}//endif hits>0
	}//endif sel group
}
void CModelOBJ::InitDrag()
{
	m_moveDelta.x = 0;	m_moveDelta.y = 0;	m_moveDelta.z = 0;
}
void CModelOBJ::DragSelect(float dist, int ctrl)
{
	dist = dist*m_radius*4;
	if(ctrl == 1)
		m_moveDelta.x = dist;
	if(ctrl == 2)
		m_moveDelta.y = dist;
	if(ctrl == 3)
		m_moveDelta.z = dist;
}
void CModelOBJ::EndDrag()
{
	int vNum = m_vertices.size();
	for(int i=0; i!=vNum; ++i)
	{
		if(m_vertices[i].selected)
		{
			m_vertices[i].x += m_moveDelta.x;
			m_vertices[i].y += m_moveDelta.y;
			m_vertices[i].z += m_moveDelta.z;
		}
	}
	m_moveDelta.x = 0;	m_moveDelta.y = 0;	m_moveDelta.z = 0;
}