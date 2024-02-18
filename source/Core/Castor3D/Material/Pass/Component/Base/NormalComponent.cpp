#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDerivativeValue.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, NormalComponent )

namespace castor3d
{
	//*********************************************************************************************

	void NormalComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eNormals )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eNormals ) )
		{
			return;
		}

		if ( !components.hasMember( "normal" ) )
		{
			if ( checkFlag( materials.getFilter(), ComponentModeFlag::eDerivTex ) )
			{
				components.declMember( "normal", shader::DerivVec3::makeType( components.getTypesCache() ) );
				components.declMember( "tangent", shader::DerivVec4::makeType( components.getTypesCache() ) );
				components.declMember( "bitangent", shader::DerivVec3::makeType( components.getTypesCache() ) );
			}
			else
			{
				components.declMember( "normal", sdw::type::Kind::eVec3F );
				components.declMember( "tangent", sdw::type::Kind::eVec4F );
				components.declMember( "bitangent", sdw::type::Kind::eVec3F );
			}
		}
	}

	void NormalComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "normal" ) )
		{
			return;
		}

		if ( surface )
		{
			if ( checkFlag( materials.getFilter(), ComponentModeFlag::eDerivTex ) )
			{
				inits.emplace_back( makeExpr( surface->getMember< shader::DerivVec3 >( "normal", shader::derivVec3( 0.0_f ) ) ) );
				inits.emplace_back( makeExpr( surface->getMember< shader::DerivVec4 >( "tangent", shader::derivVec4( 0.0_f ) ) ) );
				inits.emplace_back( makeExpr( surface->getMember< shader::DerivVec3 >( "bitangent", shader::derivVec3( 0.0_f ) ) ) );
			}
			else
			{
				inits.emplace_back( makeExpr( surface->getMember< sdw::Vec3 >( "normal", vec3( 0.0_f ) ) ) );
				inits.emplace_back( makeExpr( surface->getMember< sdw::Vec4 >( "tangent", vec4( 0.0_f ) ) ) );
				inits.emplace_back( makeExpr( surface->getMember< sdw::Vec3 >( "bitangent", vec3( 0.0_f ) ) ) );
			}
		}
		else
		{
			inits.emplace_back( makeExpr( vec3( 0.0_f ) ) );
			inits.emplace_back( makeExpr( vec4( 0.0_f ) ) );
			inits.emplace_back( makeExpr( vec3( 0.0_f ) ) );
		}
	}

	void NormalComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "normal" ) )
		{
			if ( res.usesDerivativeValues() )
			{
				using shader::operator*;
				res.getMember< shader::DerivVec3 >( "normal" ) += src.getMember< shader::DerivVec3 >( "normal" ) * passMultiplier;
				res.getMember< shader::DerivVec4 >( "tangent" ) += src.getMember< shader::DerivVec4 >( "tangent" ) * passMultiplier;
				res.getMember< shader::DerivVec3 >( "bitangent" ) += src.getMember< shader::DerivVec3 >( "bitangent" ) * passMultiplier;
			}
			else
			{
				res.getMember< sdw::Vec3 >( "normal" ) += src.getMember< sdw::Vec3 >( "normal" ) * passMultiplier;
				res.getMember< sdw::Vec4 >( "tangent" ) += src.getMember< sdw::Vec4 >( "tangent" ) * passMultiplier;
				res.getMember< sdw::Vec3 >( "bitangent" ) += src.getMember< sdw::Vec3 >( "bitangent" ) * passMultiplier;
			}
		}
	}

	//*********************************************************************************************

	castor::String const NormalComponent::TypeName = C3D_MakePassBaseComponentName( "normal" );

	NormalComponent::NormalComponent( Pass & pass )
		: PassComponent{ pass, TypeName }
	{
	}

	void NormalComponent::accept( ConfigurationVisitorBase & vis )
	{
	}

	PassComponentUPtr NormalComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< NormalComponent >( pass );
		return castor::ptrRefCast< PassComponent >( result );
	}

	//*********************************************************************************************
}
