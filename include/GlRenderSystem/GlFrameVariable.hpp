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
#ifndef ___Gl_FrameVariable___
#define ___Gl_FrameVariable___

#include "Module_GlRender.hpp"

#include "GlShaderProgram.hpp"
#include "GlBuffer.hpp"
#include "OpenGl.hpp"

#include <Castor3D/FrameVariable.hpp>

namespace GlRender
{
	struct GlVariableApplyerBase
	{
		virtual void operator()( OpenGl & p_gl, uint32_t p_uiIndex, Castor3D::FrameVariableSPtr p_pVariable )=0;
	};
	template< typename Type > struct OneVariableApplyer;
	template< typename Type, uint32_t Count > struct PointVariableApplyer;
	template< typename Type, uint32_t Rows, uint32_t Columns > struct MatrixVariableApplyer;
	
	class UboVariableInfos
	{
	public:
		uint32_t	m_uiIndex;
		int			m_iOffset;
		int			m_iSize;

		UboVariableInfos()
			:	m_uiIndex	( 0	)
			,	m_iOffset	( 0	)
			,	m_iSize		( 0	)
		{
		}
		
		~UboVariableInfos()
		{
		}
	};
	
	DECLARE_SMART_PTR( GlVariableApplyerBase );
	DECLARE_SMART_PTR( UboVariableInfos );
	DECLARE_MAP( Castor3D::FrameVariableSPtr, UboVariableInfosSPtr, UboVariableInfos );
	DECLARE_MAP( Castor3D::FrameVariableSPtr, GlVariableApplyerBaseSPtr, VariableApplyer );

	class GlFrameVariableBuffer : public Castor3D::FrameVariableBuffer
	{
	private:
		GlBufferBase< uint8_t >	m_glBuffer;
		int						m_iUniformBlockIndex;
		int						m_iUniformBlockSize;
		OpenGl &				m_gl;

	public:
		GlFrameVariableBuffer( OpenGl & p_gl, Castor::String const & p_strName, GlRenderSystem * p_pRenderSystem );
		virtual ~GlFrameVariableBuffer();

	private:
		virtual Castor3D::FrameVariableSPtr DoCreateVariable( Castor3D::ShaderProgramBase * p_pProgram, Castor3D::eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc=1 );
		virtual bool DoInitialise( Castor3D::ShaderProgramBase * p_pProgram );
		virtual void DoCleanup();
		virtual bool DoBind();
		virtual void DoUnbind();
	};

	class GlFrameVariableBase : CuNonCopyable
	{
	protected:
		uint32_t	m_iGlIndex;
		uint32_t *	m_uiParentProgram;
		bool		m_bPresentInProgram;
		OpenGl &	m_gl;

	public:
		GlFrameVariableBase( OpenGl & p_gl, uint32_t * p_uiProgram );
		virtual ~GlFrameVariableBase();
		virtual void Apply()=0;

		inline uint32_t GetGlIndex() const { return m_iGlIndex; }

	protected:
		void GetVariableLocation( char const * p_pVarName);
		template <typename Type> void DoApply( Type const * p_pValue, uint32_t p_uiOcc);
		template <typename Type, int Count> void DoApply( Type const * p_pValue, uint32_t p_uiOcc);
		template <typename Type, int Rows, int Columns> void DoApply( Type const * p_pValue, uint32_t p_uiOcc);
	};

	template< typename T >
	class GlOneFrameVariable : public Castor3D::OneFrameVariable< T >, public GlFrameVariableBase
	{
	public:
		GlOneFrameVariable( OpenGl & p_gl, uint32_t p_uiOcc, GlShaderProgram * p_pProgram=NULL );
		GlOneFrameVariable( OpenGl & p_gl, Castor3D::OneFrameVariable< T > * p_pVariable);
		virtual ~GlOneFrameVariable();
		
		virtual bool Initialise();
		virtual void Cleanup();
		virtual void Bind(){}
		virtual void Apply();
	};

	template <typename T, uint32_t Count>
	class GlPointFrameVariable : public Castor3D::PointFrameVariable<T, Count>, public GlFrameVariableBase
	{
	public:
		GlPointFrameVariable( OpenGl & p_gl, uint32_t p_uiOcc, GlShaderProgram * p_pProgram=NULL );
		GlPointFrameVariable( OpenGl & p_gl, Castor3D::PointFrameVariable<T, Count> * p_pVariable);
		virtual ~GlPointFrameVariable();
		
		virtual bool Initialise();
		virtual void Cleanup();
		virtual void Bind(){}
		virtual void Apply();
	};

	template <typename T, uint32_t Rows, uint32_t Columns>
	class GlMatrixFrameVariable : public Castor3D::MatrixFrameVariable<T, Rows, Columns>, public GlFrameVariableBase
	{
	public:
		GlMatrixFrameVariable( OpenGl & p_gl, uint32_t p_uiOcc, GlShaderProgram * p_pProgram=NULL );
		GlMatrixFrameVariable( OpenGl & p_gl, Castor3D::MatrixFrameVariable<T, Rows, Columns> * p_pVariable);
		virtual ~GlMatrixFrameVariable();
		
		virtual bool Initialise();
		virtual void Cleanup();
		virtual void Bind(){}
		virtual void Apply();
	};
#include "GlFrameVariable.inl"
}


#endif
