#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor3d
{
	//*********************************************************************************************

	void NormalComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eGeometry ) )
		{
			return;
		}

		if ( !components.hasMember( "normal" ) )
		{
			components.declMember( "normal", sdw::type::Kind::eVec3F );
			components.declMember( "tangent", sdw::type::Kind::eVec3F );
			components.declMember( "bitangent", sdw::type::Kind::eVec3F );
		}
	}

	void NormalComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::expr::ExprList & inits )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eGeometry ) )
		{
			return;
		}

		if ( surface )
		{
			inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "normal", vec3( 0.0_f ) ) ) );
			inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "tangent", vec3( 0.0_f ) ) ) );
			inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "bitangent", vec3( 0.0_f ) ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		}
	}

	void NormalComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "normal" ) )
		{
			res.getMember< sdw::Vec3 >( "normal" ) += src.getMember< sdw::Vec3 >( "normal" ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "tangent" ) += src.getMember< sdw::Vec3 >( "tangent" ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "bitangent" ) += src.getMember< sdw::Vec3 >( "bitangent" ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	castor::String const NormalComponent::TypeName = C3D_MakePassBaseComponentName( "normal" );

	NormalComponent::NormalComponent( Pass & pass )
		: PassComponent{ pass, TypeName }
	{
	}

	PassComponentUPtr NormalComponent::doClone( Pass & pass )const
	{
		return std::make_unique< NormalComponent >( pass );
	}

	//*********************************************************************************************
}
