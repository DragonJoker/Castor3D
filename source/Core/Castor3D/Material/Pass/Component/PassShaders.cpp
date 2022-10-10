#include "Castor3D/Material/Pass/Component/PassShaders.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/Writer.hpp>

namespace castor3d::shader
{
	//************************************************************************************************

	PassShaders::PassShaders( PassComponentRegister const & compRegister
		, TextureFlags const & texturesFlags
		, ComponentModeFlags filter
		, Utils & utils )
		: m_utils{ utils }
		, m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getComponentsShaders( texturesFlags, filter, m_updateComponents ) }
		, m_filter{ filter }
	{
	}

	PassShaders::PassShaders( PassComponentRegister const & compRegister
		, PipelineFlags const & flags
		, ComponentModeFlags filter
		, Utils & utils )
		: m_utils{ utils }
		, m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getComponentsShaders( flags, filter, m_updateComponents ) }
		, m_filter{ filter }
	{
	}

	void PassShaders::fillMaterial( sdw::type::BaseStruct & material
		, sdw::expr::ExprList & inits )const
	{
		if ( material.empty() )
		{
			m_compRegister.fillMaterial( material, inits, 0u );
		}
	}

	void PassShaders::fillComponents( sdw::type::BaseStruct & components
		, Materials const & materials
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponents( components, materials, nullptr );
			shader->fillComponentsInits( components, materials, nullptr, nullptr, inits );
		}
	}

	void PassShaders::fillComponents( sdw::type::BaseStruct & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponents( components, materials, &surface );
			shader->fillComponentsInits( components, materials, &material, &surface, inits );
		}
	}

	void PassShaders::fillComponentsInits( sdw::type::BaseStruct const & components
		, Materials const & materials
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponentsInits( components, materials, nullptr, nullptr, inits );
		}
	}

	void PassShaders::fillComponentsInits( sdw::type::BaseStruct const & components
		, Materials const & materials
		, Material const & material
		, sdw::StructInstance const & surface
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->fillComponentsInits( components, materials, &material, &surface, inits );
		}
	}

	void PassShaders::applyComponents( TextureFlags const & texturesFlags
		, TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, BlendComponents const & components )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->applyComponents( texturesFlags, nullptr, config, sampled, components );
		}
	}

	void PassShaders::applyComponents( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, BlendComponents const & components )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->applyComponents( flags.m_texturesFlags, &flags, config, sampled, components );
		}
	}

	void PassShaders::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, BlendComponents const & res
		, BlendComponents const & src )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->blendComponents( materials, passMultiplier, res, src );
		}
	}

	void PassShaders::updateMaterial( sdw::Vec3 const & albedo
		, sdw::Vec4 const & spcRgh
		, sdw::Vec4 const & colMtl
		, Material & material )const
	{
		for ( auto & shader : m_compRegister.getMaterialShaders() )
		{
			shader->updateMaterial( albedo
				, spcRgh
				, colMtl
				, material );
		}
	}

	void PassShaders::updateOutputs( Material const & material
		, SurfaceBase const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl )const
	{
		for ( auto & shader : m_compRegister.getMaterialShaders() )
		{
			shader->updateOutputs( material
				, surface
				, spcRgh
				, colMtl );
		}
	}

	void PassShaders::updateOutputs( BlendComponents const & components
		, SurfaceBase const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl )const
	{
		for ( auto & shader : m_shaders )
		{
			shader->updateOutputs( components
				, surface
				, spcRgh
				, colMtl );
		}
	}

	void PassShaders::updateComponents( TextureFlags const & texturesFlags
		, BlendComponents const & components )const
	{
		for ( auto & updateComponents : m_updateComponents )
		{
			updateComponents( texturesFlags, components );
		}
	}

	void PassShaders::updateComponents( PipelineFlags const & flags
		, BlendComponents const & components )const
	{
		for ( auto & updateComponents : m_updateComponents )
		{
			updateComponents( flags.m_texturesFlags, components );
		}
	}
}
