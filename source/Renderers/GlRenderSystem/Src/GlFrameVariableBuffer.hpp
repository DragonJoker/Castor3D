/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GL_FRAME_VARIABLE_BUFFER_H___
#define ___GL_FRAME_VARIABLE_BUFFER_H___

#include <FrameVariableBuffer.hpp>

#include "GlRenderSystemPrerequisites.hpp"
#include "GlBufferBase.hpp"

namespace GlRender
{
	struct GlVariableApplyerBase
	{
		virtual void operator()( OpenGl & p_gl, uint32_t p_uiIndex, Castor3D::FrameVariableSPtr p_pVariable ) = 0;
	};

	class UboVariableInfos
	{
	public:
		UboVariableInfos()
			:	m_uiIndex( 0	)
			,	m_iOffset( 0	)
			,	m_iSize( 0	)
		{
		}

		~UboVariableInfos()
		{
		}

		uint32_t m_uiIndex;
		int m_iOffset;
		int m_iSize;
	};

	DECLARE_SMART_PTR( UboVariableInfos );
	DECLARE_MAP( Castor3D::FrameVariableSPtr, UboVariableInfosSPtr, UboVariableInfos );
	DECLARE_MAP( Castor3D::FrameVariableSPtr, GlVariableApplyerBaseSPtr, VariableApplyer );

	class GlFrameVariableBuffer
		:	public Castor3D::FrameVariableBuffer
	{
	public:
		GlFrameVariableBuffer( OpenGl & p_gl, Castor::String const & p_strName, GlRenderSystem * p_pRenderSystem );
		virtual ~GlFrameVariableBuffer();

	private:
		virtual Castor3D::FrameVariableSPtr DoCreateVariable( Castor3D::ShaderProgramBase * p_pProgram, Castor3D::eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc = 1 );
		virtual bool DoInitialise( Castor3D::ShaderProgramBase * p_pProgram );
		virtual void DoCleanup();
		virtual bool DoBind();
		virtual void DoUnbind();

	private:
		GlBufferBase< uint8_t > m_glBuffer;
		int m_iUniformBlockIndex;
		int m_iUniformBlockSize;
		OpenGl & m_gl;
	};
}

#endif
