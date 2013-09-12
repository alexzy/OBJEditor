#include "Texture.h"
#include "bmp.h"	//暂时只支持bmp读取

auto_ptr<CTextureManager> CTextureManager::m_pInst = auto_ptr<CTextureManager>();

void CTextureManager::Initialize()
{
	//创建默认的棋盘格纹理
	if( m_texlist.begin() == m_texlist.end() )
	{
		CTexture *tex = new CTexture( "default" );
		// create and initialize the texture
		glGenTextures( 1, &tex->m_id );
		glBindTexture( GL_TEXTURE_2D, tex->m_id );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

		m_texlist.push_back( tex );

		// create a checker for the default texture
		int i, j, c;				// temporary variable
		unsigned char *checker;		// texture data

		checker = new unsigned char[ 64 * 64 * 4 ];
		for( i = 0; i < 64; i++ )
		{
			for( j = 0; j < 64; j++ )
			{
				c = ( !(i & 8) ^ !(j & 8)) * 255;
				checker[ (i * 256) + (j * 4) + 0 ] = (unsigned char)c;
				checker[ (i * 256) + (j * 4) + 1 ] = (unsigned char)c;
				checker[ (i * 256) + (j * 4) + 2 ] = (unsigned char)c;
				checker[ (i * 256) + (j * 4) + 3 ] = (unsigned char)255;
			}
		}
		glTexImage2D( GL_TEXTURE_2D, 0, 4, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, checker );
		delete [] checker;
	}
}
void CTextureManager::CleanAllTextures()
{
	for(list<CTexture*>::iterator itor = m_texlist.begin(); itor!=m_texlist.end(); ++itor)
	{
		delete (*itor);
		itor = m_texlist.erase(itor);
	}
}

unsigned int CTextureManager::LoadTexture(const char *fileName)
{
	unsigned int id = 0;
	unsigned char *texels = 0;
	int width=0, height=0, success=0;
	for(list<CTexture*>::iterator itor = m_texlist.begin(); itor!=m_texlist.end(); ++itor)
	{
		if( strcmp((*itor)->GetName(), fileName) == 0 )
			return (*itor)->GetId();
	}
	if( strstr(fileName, ".bmp") || strstr(fileName, ".BMP") )
		success = LoadFileBMP( fileName, &texels, &width, &height, true );
	if(success > 0)
	{
		// create and initialize new texture
		glGenTextures( 1, &id );
		glBindTexture( GL_TEXTURE_2D, id );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

		gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texels );
		// create a new CTexture object and push it at the end of the linked list
		CTexture *tex = new CTexture( id, fileName );
		m_texlist.push_back( tex );
	}
	else
	{
		// can't load the texture, use default texture
		id = (*m_texlist.begin())->GetId();
	}
	if(texels)
		delete[] texels;
	return id;
}