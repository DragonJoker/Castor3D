#include "DirectionalLight.hpp"


using namespace Castor;

namespace Castor3D
{
	bool DirectionalLight::TextLoader::operator()( DirectionalLight const & p_light, TextFile & p_file )
	{
		bool l_return = LightCategory::TextLoader()( p_light, p_file );

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\n\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	DirectionalLight::DirectionalLight()
		: LightCategory( eLIGHT_TYPE_DIRECTIONAL )
	{
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	LightCategorySPtr DirectionalLight::Create()
	{
		return std::make_shared< DirectionalLight >();
	}

	void DirectionalLight::Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const
	{
		int l_offset = 0;
		DoBindComponent( GetColour(), p_index, l_offset, p_texture );
		DoBindComponent( GetIntensity(), p_index, l_offset, p_texture );
		Point4f l_posType = GetPositionType();
		DoBindComponent( Point4f( l_posType[0], l_posType[1], -l_posType[2], l_posType[3] ), p_index, l_offset, p_texture );
	}

	void DirectionalLight::SetDirection( Castor::Point3f const & p_position )
	{
		LightCategory::SetPositionType( Castor::Point4f( p_position[0], p_position[1], p_position[2], 0.0f ) );
	}

	Castor::Point3f DirectionalLight::GetDirection()const
	{
		Point4f const & l_position = LightCategory::GetPositionType();
		return Point3f( l_position[0], l_position[1], l_position[2] );
	}
}
