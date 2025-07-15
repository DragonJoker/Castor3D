#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDerivativeValue.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"

#include <ShaderWriter/BaseTypes/CombinedImage.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

CU_ImplementSmartPtr( c3d, PassComponent )
CU_ImplementSmartPtr( c3d, PassComponentPlugin )

namespace c3d
{
	//*********************************************************************************************

	namespace shader
	{
		PassComponentID PassComponentsShader::getId()const
		{
			return m_plugin.getId();
		}

		sdw::Vec4 PassComponentsShader::sampleMap( sdw::CombinedImage2DRgba32 const & map
			, sdw::Vec3 const & texCoords
			, shader::BlendComponents const & components )const
		{
			return map.sample( texCoords.xy() );
		}

		sdw::Vec4 PassComponentsShader::sampleMap( sdw::CombinedImage2DRgba32 const & map
			, shader::DerivTex const & texCoords
			, shader::BlendComponents const & components )const
		{
			return map.grad( texCoords.value()
				, texCoords.dPdx()
				, texCoords.dPdy() );
		}
	}

	//*********************************************************************************************

	PassComponent::PassComponent( Pass & pass
		, String type
		, StringArray deps )
		: OwnedBy< Pass >{ pass }
		, m_type{ c3d::move( type ) }
		, m_dependencies{ c3d::move( deps ) }
		, m_id{ pass.getComponentId( m_type ) }
		, m_plugin{ pass.getComponentPlugin( m_id ) }
		, m_dirty{ pass.m_dirty }
		, m_materialShader{ pass.getMaterialShader( m_type ) }
	{
	}

	PassComponentUPtr PassComponent::clone( Pass & pass )const
	{
		return doClone( pass );
	}

	bool PassComponent::writeText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return doWriteText( tabs, folder, subfolder, file );
	}

	void PassComponent::fillBuffer( PassBuffer & buffer )const
	{
		if ( m_materialShader )
		{
			doFillBuffer( buffer );
		}
	}

	void PassComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )const
	{
		getPlugin().fillTextureConfiguration( configuration, mask );
	}

	//************************************************************************************************
}
