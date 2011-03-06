#include "Gl2RenderSystem/PrecompiledHeader.h"

#include "Gl2RenderSystem/Gl2Buffer.h"
#include "Gl2RenderSystem/Gl2RenderSystem.h"

using namespace Castor3D;

//******************************************************************************

void GlVbIndexBuffer :: Activate()
{
}

void GlVbIndexBuffer :: Deactivate()
{
}

//******************************************************************************

void GlVbVertexBuffer :: Activate()
{
	static const unsigned int s_arraySize[] = {1, 2, 3, 4, 4};
	static const unsigned int s_arrayType[] = {GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE};

	for (BufferDeclaration::const_iterator l_it = m_bufferDeclaration.Begin() ; l_it != m_bufferDeclaration.End() ; ++l_it)
	{
		switch (( * l_it).m_eUsage)
		{
		case eUsagePosition:
			CheckGlError( glEnableClientState( GL_VERTEX_ARRAY), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glVertexPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]), CU_T( "GlVbVertexBuffer :: Activate"));
			break;

		case eUsageNormal:
			CheckGlError( glEnableClientState( GL_NORMAL_ARRAY), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glNormalPointer( s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]), CU_T( "GlVbVertexBuffer :: Activate"));
			break;

		case eUsageTangent:
			CheckGlError( glEnableClientState( GL_TANGENT_ARRAY_EXT), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glTangentPointerEXT( s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]), CU_T( "GlVbVertexBuffer :: Activate"));
			break;

		case eUsageDiffuse:
			CheckGlError( glEnableClientState( GL_COLOR_ARRAY), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glColorPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]), CU_T( "GlVbVertexBuffer :: Activate"));
			break;

		case eUsageTexCoords0:
			CheckGlError( glActiveTexture( GL_TEXTURE0), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glClientActiveTexture( GL_TEXTURE0), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glEnableClientState( GL_TEXTURE_COORD_ARRAY), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glTexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]), CU_T( "GlVbVertexBuffer :: Activate"));
			break;

		case eUsageTexCoords1:
			CheckGlError( glActiveTexture( GL_TEXTURE1), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glClientActiveTexture( GL_TEXTURE1), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glEnableClientState( GL_TEXTURE_COORD_ARRAY), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glTexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]), CU_T( "GlVbVertexBuffer :: Activate"));
			break;

		case eUsageTexCoords2:
			CheckGlError( glActiveTexture( GL_TEXTURE2), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glClientActiveTexture( GL_TEXTURE2), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glEnableClientState( GL_TEXTURE_COORD_ARRAY), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glTexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]), CU_T( "GlVbVertexBuffer :: Activate"));
			break;

		case eUsageTexCoords3:
			CheckGlError( glActiveTexture( GL_TEXTURE3), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glClientActiveTexture( GL_TEXTURE3), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glEnableClientState( GL_TEXTURE_COORD_ARRAY), CU_T( "GlVbVertexBuffer :: Activate"));
			CheckGlError( glTexCoordPointer( s_arraySize[( * l_it).m_eDataType], s_arrayType[( * l_it).m_eDataType], m_bufferDeclaration.GetStride(), & m_buffer[( * l_it).m_uiOffset]), CU_T( "GlVbVertexBuffer :: Activate"));
			break;
		}
	}
}

void GlVbVertexBuffer :: Deactivate()
{
	CheckGlError( glDisableClientState( GL_VERTEX_ARRAY), CU_T( "GlVbVertexBuffer :: DeactivateBufferObject - glDisableClientState GL_VERTEX_ARRAY"));
	CheckGlError( glDisableClientState( GL_NORMAL_ARRAY), CU_T( "GlVbVertexBuffer :: DeactivateBufferObject - glDisableClientState GL_NORMAL_ARRAY"));
	CheckGlError( glDisableClientState( GL_COLOR_ARRAY), CU_T( "GlVbVertexBuffer :: DeactivateBufferObject - glDisableClientState GL_COLOR_ARRAY"));

	for (int i = 0; i < 4; ++i)
	{
		CheckGlError( glActiveTexture( GL_TEXTURE0 + i), CU_T( "GlVbVertexBuffer :: DeactivateBufferObject - glActiveTexture"));
		CheckGlError( glDisableClientState( GL_TEXTURE_COORD_ARRAY), CU_T( "GlVbVertexBuffer :: DeactivateBufferObject - glDisableClientState GL_TEXTURE_COORD_ARRAY"));
	}
}

//******************************************************************************