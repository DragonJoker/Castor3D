#include "PassRenderer.hpp"
#include "Camera.hpp"
#include "SceneNode.hpp"
#include "FrameVariable.hpp"
#include "OneFrameVariable.hpp"
#include "PointFrameVariable.hpp"
#include "FrameVariableBuffer.hpp"
#include "ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< class Type >
		std::shared_ptr< Type > RetrieveVariable( std::weak_ptr< Type > & p_pMemberVariable, String const & p_strName, ShaderProgramBaseWPtr p_pMemberProgram, ShaderProgramBaseSPtr p_pParameterProgram, FrameVariableBufferSPtr p_pFrameVariablesBuffer )
		{
			std::shared_ptr< Type > l_pReturn;

			if ( p_pMemberVariable.expired() || p_pParameterProgram != p_pMemberProgram.lock() )
			{
				p_pFrameVariablesBuffer->GetVariable( p_strName, l_pReturn );
				p_pMemberVariable = l_pReturn;
			}
			else
			{
				l_pReturn = p_pMemberVariable.lock();
			}

			return l_pReturn;
		}
	}

	PassRenderer::~PassRenderer()
	{
		Cleanup();
	}

	void PassRenderer::Cleanup()
	{
	}

	void PassRenderer::Render()
	{
		DoFillShader();
	}

	void PassRenderer::Render2D()
	{
		DoFillShader();
	}

	PassRenderer::PassRenderer( RenderSystem * p_pRenderSystem )
		:	Renderer<Pass, PassRenderer>( p_pRenderSystem )
	{
	}

	void PassRenderer::DoFillShader()
	{
		ShaderProgramBaseSPtr l_pProgram = m_target->GetShader< ShaderProgramBase >();

		if ( l_pProgram )
		{
			FrameVariableBufferSPtr l_pPassBuffer = m_target->GetPassBuffer();
			FrameVariableBufferSPtr l_pSceneBuffer = m_target->GetSceneBuffer();
			Point4fFrameVariableSPtr l_pAmbient = RetrieveVariable( m_pAmbient, ShaderProgramBase::MatAmbient, m_pProgram, l_pProgram, l_pPassBuffer );
			Point4fFrameVariableSPtr l_pDiffuse = RetrieveVariable( m_pDiffuse, ShaderProgramBase::MatDiffuse, m_pProgram, l_pProgram, l_pPassBuffer );
			Point4fFrameVariableSPtr l_pSpecular = RetrieveVariable( m_pSpecular, ShaderProgramBase::MatSpecular, m_pProgram, l_pProgram, l_pPassBuffer );
			Point4fFrameVariableSPtr l_pEmissive = RetrieveVariable( m_pEmissive, ShaderProgramBase::MatEmissive, m_pProgram, l_pProgram, l_pPassBuffer );
			OneFloatFrameVariableSPtr l_pShininess = RetrieveVariable( m_pShininess, ShaderProgramBase::MatShininess, m_pProgram, l_pProgram, l_pPassBuffer );
			OneFloatFrameVariableSPtr l_pOpacity = RetrieveVariable( m_pOpacity, ShaderProgramBase::MatOpacity, m_pProgram, l_pProgram, l_pPassBuffer );
			Point3rFrameVariableSPtr l_pCameraPos = RetrieveVariable( m_pCameraPos, ShaderProgramBase::CameraPos, m_pProgram, l_pProgram, l_pSceneBuffer );

			if ( l_pProgram != m_pProgram.lock() )
			{
				m_pProgram = l_pProgram;
			}

			m_target->GetAmbient().to_rgba( m_ptAmbient );
			m_target->GetDiffuse().to_rgba( m_ptDiffuse );
			m_target->GetSpecular().to_rgba( m_ptSpecular );
			m_target->GetEmissive().to_rgba( m_ptEmissive );

			if ( l_pAmbient )
			{
				l_pAmbient->SetValue( m_ptAmbient );
			}

			if ( l_pDiffuse )
			{
				l_pDiffuse->SetValue( m_ptDiffuse );
			}

			if ( l_pSpecular )
			{
				l_pSpecular->SetValue( m_ptSpecular );
			}

			if ( l_pEmissive )
			{
				l_pEmissive->SetValue( m_ptEmissive );
			}

			if ( l_pShininess )
			{
				l_pShininess->SetValue( m_target->GetShininess() );
			}

			if ( l_pOpacity )
			{
				l_pOpacity->SetValue( m_target->GetAlpha() );
			}

			if ( l_pCameraPos && m_pRenderSystem && m_pRenderSystem->GetCurrentCamera() )
			{
				Point3r l_position = m_pRenderSystem->GetCurrentCamera()->GetParent()->GetDerivedPosition();
				l_pCameraPos->SetValue( l_position );
			}
		}
	}
}
