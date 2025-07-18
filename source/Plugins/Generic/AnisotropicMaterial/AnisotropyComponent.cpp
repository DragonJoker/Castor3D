#include "AnisotropyComponent.hpp"

#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/Component/Base/NormalComponent.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslDebugOutput.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/CameraUbo.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

CU_ImplementSmartPtr( anisotropy, AnisotropyComponent )

namespace anisotropy
{
	//*********************************************************************************************

	template< typename TypeT >
	class TextWriter
		: public c3d::TextWriterT< AnisotropyComponent >
	{
	public:
		explicit TextWriter( c3d::String const & tabs )
			: c3d::TextWriterT< AnisotropyComponent >{ tabs }
		{
		}

		bool operator()( AnisotropyComponent const & pass
			, c3d::StringStream & file )override
		{
			c3d::log::info << this->tabs() << cuT( "Writing Anisotropy data " ) << std::endl;
			return this->writeOpt( file, cuT( "anisotropy_strength" ), pass.getStrength(), AnisotropyComponent::DefaultStrength )
				&& this->writeOpt( file, cuT( "anisotropy_rotation" ), pass.getRotation().degrees(), AnisotropyComponent::DefaultRotation );
		}
	};

	//*********************************************************************************************

	namespace toonpass
	{
		static CU_ImplementAttributeParserBlock( parserPassAnisotropyStrength, c3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = c3d::getPassComponent< AnisotropyComponent >( *blockContext );
				component.setStrength( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassAnisotropyRotation, c3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = c3d::getPassComponent< AnisotropyComponent >( *blockContext );
				float v = params[0]->get< float >();
				component.setRotation( c3d::Angle::fromDegrees( v ) );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void AnisotropyComponent::ReflRefrShader::computeWithTransmission( c3ds::ReflectionModel & reflections
		, c3ds::BlendComponents & components
		, c3ds::LightSurface const & lightSurface
		, c3ds::BackgroundModel & backgroundModel
		, sdw::CombinedImage2DRgba32 const & mippedScene
		, c3ds::CameraData const & camera
		, c3ds::RenderData const & render
		, c3ds::DirectLighting & lighting
		, c3ds::IndirectLighting & indirect
		, sdw::Vec2 const & sceneUv
		, sdw::UInt const & envMapIndex
		, sdw::Vec3 const & incident
		, c3ds::ReflectionRefraction & output
		, c3ds::DebugOutputCategory const & debugOutput )const
	{
		if ( mippedScene.isEnabled() )
		{
			auto & writer = *components.getWriter();
			auto anisotropicT = components.getMember< sdw::Vec3 >( "anisotropicT" );
			auto anisotropicB = components.getMember< sdw::Vec3 >( "anisotropicB" );
			auto anisotropyStrength = components.getMember< sdw::Float >( "anisotropyStrength" );

			auto bentNormal = writer.declLocale( "bentNormal"
				, cross( anisotropicB, lightSurface.V().value() ) );
			bentNormal = normalize( cross( bentNormal, anisotropicB ) );
			// This heuristic can probably be improved upon
			auto a = writer.declLocale( "a"
				, pow( pow( 1.0_f - anisotropyStrength * ( 1.0_f - components.perceptualRoughness ), 2.0_f ), 2.0_f ) );
			bentNormal = normalize( mix( bentNormal, lightSurface.N().value(), vec3( a ) ) );

			reflections.computeWithTransmission( components
				, bentNormal
				, lightSurface.worldPosition().value().xyz()
				, lightSurface.V().value()
				, dot( bentNormal, lightSurface.V().value() )
				, backgroundModel
				, mippedScene
				, camera
				, sceneUv
				, envMapIndex
				, output
				, debugOutput );
		}
		else
		{
			computeWithoutTransmission( reflections
				, components
				, lightSurface
				, backgroundModel
				, camera
				, render
				, lighting
				, indirect
				, sceneUv
				, envMapIndex
				, incident
				, output
				, debugOutput );
		}
	}

	void AnisotropyComponent::ReflRefrShader::computeWithoutTransmission( c3ds::ReflectionModel & reflections
		, c3ds::BlendComponents & components
		, c3ds::LightSurface const & lightSurface
		, c3ds::BackgroundModel & backgroundModel
		, c3ds::CameraData const & camera
		, c3ds::RenderData const & render
		, c3ds::DirectLighting & lighting
		, c3ds::IndirectLighting & indirect
		, sdw::Vec2 const & sceneUv
		, sdw::UInt const & envMapIndex
		, sdw::Vec3 const & incident
		, c3ds::ReflectionRefraction & output
		, c3ds::DebugOutputCategory const & debugOutput )const
	{
		auto & writer = *components.getWriter();
		auto anisotropicT = components.getMember< sdw::Vec3 >( "anisotropicT" );
		auto anisotropicB = components.getMember< sdw::Vec3 >( "anisotropicB" );
		auto anisotropyStrength = components.getMember< sdw::Float >( "anisotropyStrength" );

		auto bentNormal = writer.declLocale( "bentNormal"
			, cross( anisotropicB, lightSurface.V().value() ) );
		bentNormal = normalize( cross( bentNormal, anisotropicB ) );
		// This heuristic can probably be improved upon
		auto a = writer.declLocale( "a"
			, pow( pow( 1.0_f - anisotropyStrength * ( 1.0_f - components.perceptualRoughness ), 2.0_f ), 2.0_f ) );
		bentNormal = normalize( mix( bentNormal, lightSurface.N().value(), vec3( a ) ) );

		reflections.computeWithoutTransmission( components
			, bentNormal
			, lightSurface.worldPosition().value().xyz()
			, lightSurface.V().value()
			, clamp( dot( bentNormal, lightSurface.V().value() ), 0.0_f, 1.0_f )
			, backgroundModel
			, envMapIndex
			, output
			, debugOutput );
	}

	//*********************************************************************************************

	AnisotropyComponent::MaterialShader::MaterialShader()
		: c3ds::PassMaterialShader{ 12u }
	{
	}

	void AnisotropyComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "anisotropyStrength" ) )
		{
			type.declMember( "anisotropyDirection", ast::type::Kind::eVec2F );
			type.declMember( "anisotropyStrength", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( vec2( 1.0_f, 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ AnisotropyComponent::DefaultStrength } ) );
		}
	}

	//*********************************************************************************************

	void AnisotropyComponent::ComponentsShader::fillComponents( c3d::ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, c3ds::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( ( !checkFlag( componentsMask, c3d::ComponentModeFlag::eSpecularLighting ) )
			|| ( !checkFlag( materials.getFilter(), c3d::ComponentModeFlag::eSpecularLighting ) ) )
		{
			return;
		}

		if ( !components.hasMember( "anisotropyStrength" ) )
		{
			components.declMember( "anisotropyDirection", sdw::type::Kind::eVec2F );
			components.declMember( "anisotropyStrength", sdw::type::Kind::eFloat );
			components.declMember( "anisotropicT", sdw::type::Kind::eVec3F );
			components.declMember( "anisotropicB", sdw::type::Kind::eVec3F );
		}
	}

	void AnisotropyComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, c3ds::Materials const & materials
		, c3ds::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "anisotropyStrength" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec2 >( "anisotropyDirection" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "anisotropyStrength" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec2( 1.0_f, 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ AnisotropyComponent::DefaultStrength } ) );
		}

		inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
	}

	void AnisotropyComponent::ComponentsShader::blendComponents( c3ds::Materials const & materials
		, sdw::Float const & passMultiplier
		, c3ds::BlendComponents & res
		, c3ds::BlendComponents const & src )const
	{
		if ( res.hasMember( "anisotropyStrength" ) )
		{
			res.getMember< sdw::Float >( "anisotropyStrength" ) += src.getMember< sdw::Float >( "anisotropyStrength" ) * passMultiplier;
			res.getMember< sdw::Vec2 >( "anisotropyDirection" ) += src.getMember< sdw::Vec2 >( "anisotropyDirection" ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "anisotropicT" ) += src.getMember< sdw::Vec3 >( "anisotropicT" ) * passMultiplier;
			res.getMember< sdw::Vec3 >( "anisotropicB" ) += src.getMember< sdw::Vec3 >( "anisotropicB" ) * passMultiplier;
		}
	}

	void AnisotropyComponent::ComponentsShader::updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3ds::Material const & material
		, c3ds::BlendComponents & components
		, bool isFrontCulled )const
	{
		if ( !components.hasMember( "anisotropyStrength" ) )
		{
			return;
		}

		auto normal = components.usesDerivativeValues()
			? components.getMember< c3ds::DerivVec3 >( "normal" ).value()
			: components.getMember< sdw::Vec3 >( "normal" );
		auto tangent = components.usesDerivativeValues()
			? components.getMember< c3ds::DerivVec4 >( "tangent" ).value()
			: components.getMember< sdw::Vec4 >( "tangent" );
		auto bitangent = components.usesDerivativeValues()
			? components.getMember< c3ds::DerivVec3 >( "bitangent" ).value()
			: components.getMember< sdw::Vec3 >( "bitangent" );
		auto tbn = c3ds::Utils::getTBN( normal, tangent.xyz(), bitangent );

		auto anisotropicT = components.getMember< sdw::Vec3 >( "anisotropicT" );
		auto anisotropicB = components.getMember< sdw::Vec3 >( "anisotropicB" );
		anisotropicT = normalize( tbn * vec3( components.getMember< sdw::Vec2 >( "anisotropyDirection" ), 0.0_f ) );
		anisotropicB = normalize( cross( normal, anisotropicT ) );
	}

	//*********************************************************************************************

	void AnisotropyComponent::Plugin::createParsers( c3d::AttributeParsers & parsers
		, c3d::ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, c3d::CSCNSection::ePass
			, cuT( "anisotropy_strength" )
			, toonpass::parserPassAnisotropyStrength
			, { c3d::makeParameter< c3d::ParameterType::eFloat >( c3d::makeRange( 0.0f, 1.0f ) ) } );
		c3d::addParserT( parsers
			, c3d::CSCNSection::ePass
			, cuT( "anisotropy_rotation" )
			, toonpass::parserPassAnisotropyRotation
			, { c3d::makeParameter< c3d::ParameterType::eFloat >( c3d::makeRange( 0.0f, 360.0f ) ) } );
	}

	void AnisotropyComponent::Plugin::zeroBuffer( c3d::Pass const & pass
		, c3ds::PassMaterialShader const & materialShader
		, c3d::PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), 1.0f, offset );
		offset += data.write( materialShader.getMaterialChunk(), 0.0f, offset );
		data.write( materialShader.getMaterialChunk(), AnisotropyComponent::DefaultStrength, offset );
	}

	bool AnisotropyComponent::Plugin::isComponentNeeded( c3d::TextureCombine const & textures
		, c3d::ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, c3d::ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	c3d::String const AnisotropyComponent::TypeName = C3D_PluginMakePassOtherComponentName( "anisotropy", "anisotropy" );

	AnisotropyComponent::AnisotropyComponent( c3d::Pass & pass )
		: BaseDataPassComponentT< AnisotropyData >{ pass, TypeName, { c3d::NormalComponent::TypeName }
			, AnisotropyComponent::DefaultStrength, AnisotropyComponent::DefaultRotation }
	{
	}

	void AnisotropyComponent::accept( c3d::ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Anisotropy" ) );
		vis.visit( cuT( "Strength" ), m_value.strength );
		vis.visit( cuT( "Rotation" ), m_value.rotation );
	}

	c3d::PassComponentUPtr AnisotropyComponent::doClone( c3d::Pass & pass )const
	{
		auto result = c3d::makeRawUnique< AnisotropyComponent >( pass );
		result->setData( getData() );
		return c3d::PassComponentUPtr{ result.release() };
	}

	bool AnisotropyComponent::doWriteText( c3d::String const & tabs
		, c3d::Path const & folder
		, c3d::String const & subfolder
		, c3d::StringStream & file )const
	{
		return TextWriter< AnisotropyComponent >{ tabs }( *this, file );
	}

	void AnisotropyComponent::doFillBuffer( c3d::PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk(), getRotation().cos(), offset );
		offset += data.write( m_materialShader->getMaterialChunk(), getRotation().sin(), offset );
		data.write( m_materialShader->getMaterialChunk(), getStrength(), offset );
	}

	//*********************************************************************************************
}
