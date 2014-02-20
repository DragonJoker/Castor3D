#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/DirectionalLight.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

bool DirectionalLight::TextLoader :: operator ()( DirectionalLight const & p_light, TextFile & p_file )
{
	bool l_bReturn = LightCategory::TextLoader()( p_light, p_file);

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "\n}\n" ) ) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

DirectionalLight :: DirectionalLight()
	:	LightCategory	( eLIGHT_TYPE_DIRECTIONAL	)
{
}

DirectionalLight :: ~DirectionalLight()
{
}

LightCategorySPtr DirectionalLight :: Create()
{
	return std::make_shared< DirectionalLight >();
}

void DirectionalLight :: Render( LightRendererSPtr p_pRenderer )
{
	if( p_pRenderer )
	{
		p_pRenderer->Enable();
		p_pRenderer->ApplyPosition();
		p_pRenderer->ApplyAmbient();
		p_pRenderer->ApplyDiffuse();
		p_pRenderer->ApplySpecular();
		p_pRenderer->Bind();
	}
}

void DirectionalLight :: Render( LightRendererSPtr p_pRenderer, ShaderProgramBase * p_pProgram )
{
	if( p_pRenderer )
	{
		p_pRenderer->ApplyPositionShader();
		p_pRenderer->ApplyAmbientShader();
		p_pRenderer->ApplyDiffuseShader();
		p_pRenderer->ApplySpecularShader();
		p_pRenderer->EnableShader( p_pProgram );
	}
}

void DirectionalLight :: DoNormalisePosition()
{
	Point4f l_ptPosType = GetPositionType();
	Point3f l_ptDirection( l_ptPosType[0], l_ptPosType[1], l_ptPosType[2] );
	LightCategory::SetPosition( point::get_normalised( l_ptDirection ) );
}

//*************************************************************************************************
