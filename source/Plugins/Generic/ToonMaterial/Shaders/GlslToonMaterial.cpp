#include "ToonMaterial/Shaders/GlslToonMaterial.hpp"

#include "ToonMaterial/ToonPass.hpp"

#include <Castor3D/Limits.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/ShaderBuffer.hpp>

#include <ShaderWriter/Source.hpp>

namespace toon::shader
{
	//***********************************************************************************************

	namespace lighting
	{
		static sdw::Float interpolate( sdw::Float const & lhs
			, sdw::Float const & rhs
			, sdw::Float const & weight )
		{
			return lhs * ( 1.0_f - weight ) + rhs * weight;
		}

		static sdw::Vec4 interpolate( sdw::Vec4 const & lhs
			, sdw::Vec4 const & rhs
			, sdw::Float const & weight )
		{
			return lhs * vec4( 1.0_f - weight ) + rhs * vec4( weight );
		}
	}

	//*********************************************************************************************

	ToonProfiles::ToonProfiles( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: BufferT< ToonProfile >{ "ToonProfilesBuffer"
			, "toonProfiles"
			, writer
			, binding
			, set
			, enable }
	{
	}

	void ToonProfiles::update( castor3d::ShaderBuffer & buffer
		, castor3d::Pass const & pass )
	{
		static auto const toonPhongID = buffer.getDevice().renderSystem.getEngine()->getPassFactory().getNameId( ToonPhongPass::Type );
		static auto const toonBlinnPhongID = buffer.getDevice().renderSystem.getEngine()->getPassFactory().getNameId( ToonBlinnPhongPass::Type );
		static auto const toonPbrID = buffer.getDevice().renderSystem.getEngine()->getPassFactory().getNameId( ToonPbrPass::Type );
		auto profiles = castor::makeArrayView( reinterpret_cast< ToonProfileData * >( buffer.getPtr() ), castor3d::MaxMaterialsCount );
		auto & data = profiles[pass.getId() - 1u];

		if ( pass.getTypeID() == toonPhongID )
		{
			auto & toonPass = static_cast< ToonPhongPass const & >( pass );
			toonPass.fillData( data );
		}
		else if ( pass.getTypeID() == toonBlinnPhongID )
		{
			auto & toonPass = static_cast< ToonBlinnPhongPass const & >( pass );
			toonPass.fillData( data );
		}
		else if ( pass.getTypeID() == toonPbrID )
		{
			auto & toonPass = static_cast< ToonPbrPass const & >( pass );
			toonPass.fillData( data );
		}
	}

	castor3d::ShaderBufferUPtr ToonProfiles::create( castor3d::RenderDevice const & device )
	{
		return castor::makeUnique< castor3d::ShaderBuffer >( *device.renderSystem.getEngine()
			, device
			, castor3d::MaxMaterialsCount * sizeof( ToonProfileData )
			, cuT( "ToonProfilesBuffer" ) );
	}

	c3d::BufferBaseUPtr ToonProfiles::declare( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set )
	{
		return castor::makeUniqueDerived< c3d::BufferBase, ToonProfiles >( writer
			, binding
			, set
			, true );
	}

	//***********************************************************************************************

	ToonPhongLightMaterial::ToonPhongLightMaterial( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: c3d::LightMaterial{ writer, std::move( expr ), enabled }
		, ambient{ albDiv }
		, shininess{ spcDiv }
		, edgeWidth{ getMember< sdw::Float >( "edgeWidth" ) }
		, depthFactor{ getMember< sdw::Float >( "depthFactor" ) }
		, normalFactor{ getMember< sdw::Float >( "normalFactor" ) }
		, objectFactor{ getMember< sdw::Float >( "objectFactor" ) }
		, edgeColour{ getMember< sdw::Vec4 >( "edgeColour" ) }
		, smoothBand{ getMember< sdw::Float >( "smoothBand" ) }
	{
	}

	ast::type::BaseStructPtr ToonPhongLightMaterial::makeType( ast::type::TypesCache & cache )
	{
		auto result = LightMaterial::makeType( cache );

		if ( !result->hasMember( "edgeWidth" ) )
		{
			result->declMember( "edgeWidth", ast::type::Kind::eFloat );
			result->declMember( "depthFactor", ast::type::Kind::eFloat );
			result->declMember( "normalFactor", ast::type::Kind::eFloat );
			result->declMember( "objectFactor", ast::type::Kind::eFloat );
			result->declMember( "edgeColour", ast::type::Kind::eVec4F );
			result->declMember( "smoothBand", ast::type::Kind::eFloat );
		}

		return result;
	}

	void ToonPhongLightMaterial::create( sdw::Vec3 const & palbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, c3d::Materials const & materials
		, c3d::Material const & material )
	{
		if ( isEnabled() )
		{
			albedo = palbedo;
			specular = spcMtl.rgb();
			ambient = material.colourDiv().a();
			shininess = colRgh.a();

			if ( materials.hasSpecificsBuffer< ToonProfile >() )
			{
				auto & toonProfiles = materials.getSpecificsBuffer< ToonProfile >();
				auto toonProfile = m_writer->declLocale( "toonProfile"
					, toonProfiles.getData( material.passId() - 1u ) );
				edgeWidth = toonProfile.edgeWidth();
				depthFactor = toonProfile.depthFactor();
				normalFactor = toonProfile.normalFactor();
				objectFactor = toonProfile.objectFactor();
				edgeColour = toonProfile.edgeColour();
				smoothBand = toonProfile.smoothBand();
			}
		}
	}

	void ToonPhongLightMaterial::create( sdw::Vec3 const & palbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, sdw::Float const & pambient )
	{
		if ( isEnabled() )
		{
			albedo = palbedo;
			specular = spcMtl.rgb();
			ambient = pambient;
			shininess = colRgh.a();

			edgeWidth = 0.0_f;
			depthFactor = 0.0_f;
			normalFactor = 0.0_f;
			objectFactor = 0.0_f;
			edgeColour = vec4( 0.0_f );
			smoothBand = 0.0_f;
		}
	}

	void ToonPhongLightMaterial::create( sdw::Vec3 const & vtxColour
		, c3d::Materials const & materials
		, c3d::Material const & material )
	{
		if ( vtxColour.isEnabled() )
		{
			create( material.colour() * vtxColour
				, material.specDiv()
				, material.specDiv()
				, materials
				, material );
		}
		else
		{
			create( material.colour()
				, material.specDiv()
				, material.specDiv()
				, materials
				, material );
		}
	}

	void ToonPhongLightMaterial::output( sdw::Vec4 & outColRgh, sdw::Vec4 & outSpcMtl )const
	{
		outColRgh = vec4( albedo, shininess );
		outSpcMtl = vec4( specular, 0.0_f );
	}

	sdw::Vec3 ToonPhongLightMaterial::getAmbient( sdw::Vec3 const & ambientLight )const
	{
		return ambientLight * albedo;
	}

	void ToonPhongLightMaterial::adjustDirectSpecular( sdw::Vec3 & directSpecular )const
	{
		directSpecular *= specular;
	}

	sdw::Vec3 ToonPhongLightMaterial::getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const
	{
		return indirectAmbient * ambient;
	}

	sdw::Float ToonPhongLightMaterial::getMetalness()const
	{
		return c3d::LightMaterial::computeMetalness( albedo, specular );
	}

	sdw::Float ToonPhongLightMaterial::getRoughness()const
	{
		return c3d::LightMaterial::computeRoughness( ToonPhongLightMaterial::computeGlossiness( shininess ) );
	}

	sdw::Float ToonPhongLightMaterial::computeGlossiness( sdw::Float const & shininess )
	{
		return shininess / castor3d::MaxPhongShininess;
	}

	sdw::Float ToonPhongLightMaterial::computeShininess( sdw::Float const & glossiness )
	{
		return glossiness * castor3d::MaxPhongShininess;
	}

	void ToonPhongLightMaterial::doBlendWith( c3d::LightMaterial const & material
		, sdw::Float const & weight )
	{
		auto & toonMaterial = static_cast< ToonPhongLightMaterial const & >( material );
		edgeWidth = lighting::interpolate( edgeWidth, toonMaterial.edgeWidth, weight );
		depthFactor = lighting::interpolate( depthFactor, toonMaterial.depthFactor, weight );
		normalFactor = lighting::interpolate( normalFactor, toonMaterial.normalFactor, weight );
		objectFactor = lighting::interpolate( objectFactor, toonMaterial.objectFactor, weight );
		edgeColour = lighting::interpolate( edgeColour, toonMaterial.edgeColour, weight );
		smoothBand = lighting::interpolate( smoothBand, toonMaterial.smoothBand, weight );
	}

	//*********************************************************************************************

	ToonPbrLightMaterial::ToonPbrLightMaterial( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: LightMaterial{ writer, std::move( expr ), enabled }
		, roughness{ albDiv }
		, metalness{ spcDiv }
		, edgeWidth{ getMember< sdw::Float >( "edgeWidth" ) }
		, depthFactor{ getMember< sdw::Float >( "depthFactor" ) }
		, normalFactor{ getMember< sdw::Float >( "normalFactor" ) }
		, objectFactor{ getMember< sdw::Float >( "objectFactor" ) }
		, edgeColour{ getMember< sdw::Vec4 >( "edgeColour" ) }
		, smoothBand{ getMember< sdw::Float >( "smoothBand" ) }
	{
	}

	ast::type::BaseStructPtr ToonPbrLightMaterial::makeType( ast::type::TypesCache & cache )
	{
		auto result = LightMaterial::makeType( cache );

		if ( !result->hasMember( "edgeWidth" ) )
		{
			result->declMember( "edgeWidth", ast::type::Kind::eFloat );
			result->declMember( "depthFactor", ast::type::Kind::eFloat );
			result->declMember( "normalFactor", ast::type::Kind::eFloat );
			result->declMember( "objectFactor", ast::type::Kind::eFloat );
			result->declMember( "edgeColour", ast::type::Kind::eVec4F );
			result->declMember( "smoothBand", ast::type::Kind::eFloat );
		}

		return result;
	}

	void ToonPbrLightMaterial::create( sdw::Vec3 const & palbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, c3d::Materials const & materials
		, c3d::Material const & material )
	{
		if ( isEnabled() )
		{
			albedo = palbedo;
			specular = spcMtl.rgb();
			metalness = spcMtl.a();
			roughness = colRgh.a();

			if ( materials.hasSpecificsBuffer< ToonProfile >() )
			{
				auto & toonProfiles = materials.getSpecificsBuffer< ToonProfile >();
				auto toonProfile = m_writer->declLocale( "toonProfile"
					, toonProfiles.getData( material.passId() - 1u ) );
				edgeWidth = toonProfile.edgeWidth();
				depthFactor = toonProfile.depthFactor();
				normalFactor = toonProfile.normalFactor();
				objectFactor = toonProfile.objectFactor();
				edgeColour = toonProfile.edgeColour();
				smoothBand = toonProfile.smoothBand();
			}
		}
	}

	void ToonPbrLightMaterial::create( sdw::Vec3 const & palbedo
		, sdw::Vec4 const & spcMtl
		, sdw::Vec4 const & colRgh
		, sdw::Float const & ambient )
	{
		if ( isEnabled() )
		{
			albedo = palbedo;
			specular = spcMtl.rgb();
			metalness = spcMtl.a();
			roughness = colRgh.a();

			edgeWidth = 0.0_f;
			depthFactor = 0.0_f;
			normalFactor = 0.0_f;
			objectFactor = 0.0_f;
			edgeColour = vec4( 0.0_f );
			smoothBand = 0.0_f;
		}
	}

	void ToonPbrLightMaterial::create( sdw::Vec3 const & vtxColour
		, c3d::Materials const & materials
		, c3d::Material const & material )
	{
		if ( vtxColour.isEnabled() )
		{
			create( material.colour() * vtxColour
				, material.specDiv()
				, material.colourDiv()
				, materials
				, material );
		}
		else
		{
			create( material.colour()
				, material.specDiv()
				, material.colourDiv()
				, materials
				, material );
		}
	}

	void ToonPbrLightMaterial::output( sdw::Vec4 & outColRgh, sdw::Vec4 & outSpcMtl )const
	{
		outColRgh = vec4( albedo, roughness );
		outSpcMtl = vec4( specular, metalness );
	}

	sdw::Vec3 ToonPbrLightMaterial::getAmbient( sdw::Vec3 const & ambientLight )const
	{
		return ambientLight;
	}

	void ToonPbrLightMaterial::adjustDirectSpecular( sdw::Vec3 & directSpecular )const
	{
	}

	sdw::Vec3 ToonPbrLightMaterial::getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const
	{
		return indirectAmbient;
	}

	sdw::Float ToonPbrLightMaterial::getMetalness()const
	{
		return metalness;
	}

	sdw::Float ToonPbrLightMaterial::getRoughness()const
	{
		return roughness;
	}

	void ToonPbrLightMaterial::doBlendWith( c3d::LightMaterial const & material
		, sdw::Float const & weight )
	{
		auto & toonMaterial = static_cast< ToonPbrLightMaterial const & >( material );
		edgeWidth = lighting::interpolate( edgeWidth, toonMaterial.edgeWidth, weight );
		depthFactor = lighting::interpolate( depthFactor, toonMaterial.depthFactor, weight );
		normalFactor = lighting::interpolate( normalFactor, toonMaterial.normalFactor, weight );
		objectFactor = lighting::interpolate( objectFactor, toonMaterial.objectFactor, weight );
		edgeColour = lighting::interpolate( edgeColour, toonMaterial.edgeColour, weight );
		smoothBand = lighting::interpolate( smoothBand, toonMaterial.smoothBand, weight );
	}

	//*********************************************************************************************
}
