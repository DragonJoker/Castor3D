#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	LightSurface::LightSurface( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, castor::move( expr ), enabled }
		, m_eyePosition{ getMember< sdw::Vec3 >( "eyePosition" ) }
		, m_worldPosition{ getMember< DerivVec4 >( "worldPosition" ) }
		, m_viewPosition{ getMember< DerivVec3 >( "viewPosition" ) }
		, m_clipPosition{ getMember< sdw::Vec3 >( "clipPosition" ) }
		, m_vertexToLight{ getMember< DerivVec3 >( "vertexToLight" ) }
		, m_V{ getMember< DerivVec3 >( "V" ) }
		, m_N{ getMember< DerivVec3 >( "N" ) }
		, m_L{ getMember< DerivVec3 >( "L" ) }
		, m_H{ getMember< DerivVec3 >( "H" ) }
		, m_lengthV{ getMember< DerivFloat >( "lengthV" ) }
		, m_lengthL{ getMember< DerivFloat >( "lengthL" ) }
		, m_NdotV{ getMember< DerivFloat >( "NdotV" ) }
		, m_NdotL{ getMember< DerivFloat >( "NdotL", derivFloat( 0.0_f ) ) }
		, m_NdotH{ getMember< DerivFloat >( "NdotH", derivFloat( 0.0_f ) ) }
		, m_HdotV{ getMember< DerivFloat >( "HdotV", derivFloat( 0.0_f ) ) }
		, m_F{ getMember< DerivVec3 >( "F", derivVec3( 0.0_f ) ) }
		, m_spcF{ getMember< DerivVec3 >( "spcF", m_F ) }
		, m_difF{ getMember< DerivVec3 >( "difF", m_F ) }
	{
	}

	LightSurface::LightSurface( sdw::Vec3 const eye
		, DerivVec4 const world
		, DerivVec3 const view
		, sdw::Vec3 const clip
		, DerivVec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
		: LightSurface{ findWriterMandat( eye, world, view, clip, normal )
			, makeInit( makeType( findTypesCache( eye, world, view, clip, normal ), enableDotProducts, enableFresnel, enableIridescence )
				, eye, world, view, clip, normal
				, enableDotProducts, enableFresnel, enableIridescence )
			, true }
	{
	}

	sdw::type::BaseStructPtr LightSurface::makeType( ast::type::TypesCache & cache
		, sdw::type::TypePtr type )
	{
		return std::static_pointer_cast< sdw::type::BaseStruct >( type );
	}

	sdw::type::BaseStructPtr LightSurface::makeType( ast::type::TypesCache & cache
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		auto type = cache.getStruct( ast::type::MemoryLayout::eC
			, "C3D_LightSurface"
				+ ( enableDotProducts ? castor::MbString{ "Prods" } : castor::MbString{} )
				+ ( enableFresnel ? castor::MbString{ "F" } : castor::MbString{} )
				+ ( enableIridescence ? castor::MbString{ "I" } : castor::MbString{} ) );

		if ( type->empty() )
		{
			type->declMember( "eyePosition", sdw::Vec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "worldPosition", DerivVec4::makeType( cache ), ast::type::NotArray );
			type->declMember( "viewPosition", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "clipPosition", sdw::Vec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "vertexToLight", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "V", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "N", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "L", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "H", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "lengthV", DerivFloat::makeType( cache ), ast::type::NotArray );
			type->declMember( "lengthL", DerivFloat::makeType( cache ), ast::type::NotArray );
			type->declMember( "NdotV", DerivFloat::makeType( cache ), ast::type::NotArray );
			type->declMember( "NdotL", DerivFloat::makeType( cache ), ast::type::NotArray, enableDotProducts );
			type->declMember( "NdotH", DerivFloat::makeType( cache ), ast::type::NotArray, enableDotProducts );
			type->declMember( "HdotV", DerivFloat::makeType( cache ), ast::type::NotArray, enableDotProducts );
			type->declMember( "F", DerivVec3::makeType( cache ), ast::type::NotArray, enableFresnel );
			type->declMember( "spcF", DerivVec3::makeType( cache ), ast::type::NotArray, enableFresnel && enableIridescence );
			type->declMember( "difF", DerivVec3::makeType( cache ), ast::type::NotArray, enableFresnel && enableIridescence );
		}

		return type;
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, castor::MbString const & name
		, sdw::Vec3 const eye
		, DerivVec4 const world
		, DerivVec3 const view
		, sdw::Vec3 const clip
		, DerivVec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		auto result = writer.declLocale< LightSurface >( name
			, LightSurface{ eye, world, view, clip, normal
				, enableDotProducts, enableFresnel, enableIridescence } );
		result.m_NdotV = max( derivFloat( 0.0_f ), dot( result.N(), result.V() ) );
		return result;
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, castor::MbString const & name
		, DerivVec4 const world
		, sdw::Vec3 const clip
		, DerivVec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		return create( writer, name, vec3( 0.0_f ), world, derivVec3( 0.0_f ), clip, normal
			, enableDotProducts, enableFresnel, enableIridescence );
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, Utils & utils
		, castor::MbString const & name
		, sdw::Vec3 const eye
		, DerivVec4 const world
		, DerivVec3 const view
		, sdw::Vec3 const clip
		, DerivVec3 const normal
		, sdw::Vec3 const f0
		, BlendComponents const & components
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		auto result = create( writer, name, eye, world, view, clip, normal
			, enableDotProducts, enableFresnel, enableIridescence );
		result.doUpdateF( utils, f0, components, result.NdotV() );
		return result;
	}

	void LightSurface::updateN( DerivVec3 const n )const
	{
		N() = n;

		m_NdotL = max( derivFloat( 0.0_f ), dot( N(), L() ) );
		m_NdotH = max( derivFloat( 0.0_f ), dot( N(), H() ) );
		m_NdotV = max( derivFloat( 0.0_f ), dot( N(), V() ) );
	}

	void LightSurface::updateL( DerivVec3 const VtoL )const
	{
		vertexToLight() = VtoL;
		L() = normalize( vertexToLight() );
		lengthL() = length( vertexToLight() );
		H() = normalize( L() + V() );

		m_NdotL = max( derivFloat( 0.0_f ), dot( N(), L() ) );
		m_NdotH = max( derivFloat( 0.0_f ), dot( N(), H() ) );
		m_HdotV = max( derivFloat( 0.0_f ), dot( H(), V() ) );
	}

	void LightSurface::updateN( Utils & utils
		, DerivVec3 const n
		, sdw::Vec3 const f0
		, BlendComponents const & components )const
	{
		updateN( n );
		doUpdateF( utils, f0, components, NdotV() );
	}

	void LightSurface::updateL( Utils & utils
		, DerivVec3 const VtoL
		, sdw::Vec3 const f0
		, BlendComponents const & components )const
	{
		updateL( VtoL );
		doUpdateF( utils, f0, components, HdotV() );
	}

	void LightSurface::doUpdateF( Utils & utils
		, sdw::Vec3 const f0
		, BlendComponents const & components
		, DerivFloat const & dotProduct )const
	{
		m_F = utils.conductorFresnel( dotProduct
			, f0
			, components.f90 );

		if ( m_difF )
		{
			auto & writer = *getWriter();

			IF( writer, components.iridescenceFactor != 0.0_f )
			{
				// Blend default specular Fresnel with iridescence Fresnel
				m_spcF = mix( F(), derivVec3( components.iridescenceFresnel ), derivVec3( components.iridescenceFactor ) );
				// Use the maximum component of the iridescence Fresnel color
				// Maximum is used instead of the RGB value to not get inverse colors for the diffuse BRDF
				m_difF = mix( F()
					, derivVec3( max( max( components.iridescenceFresnel.r(), components.iridescenceFresnel.g() ), components.iridescenceFresnel.b() ) )
					, derivVec3( components.iridescenceFactor ) );
			}
			ELSE
			{
				m_spcF = F();
				m_difF = F();
			}
			FI;
		}
	}

	sdw::expr::ExprPtr LightSurface::makeInit( sdw::type::BaseStructPtr type
		, sdw::Vec3 const eye
		, DerivVec4 const world
		, DerivVec3 const view
		, sdw::Vec3 const clip
		, DerivVec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		using shader::operator-;
		sdw::expr::ExprList inits;
		inits.push_back( makeExpr( eye ) ); // eyePosition
		inits.push_back( makeExpr( world ) ); // worldPosition
		inits.push_back( makeExpr( view ) ); // viewPosition
		inits.push_back( makeExpr( clip ) ); // clipPosition
		inits.push_back( makeExpr( derivVec3( 0.0_f ) ) ); // vertexToLight
		inits.push_back( makeExpr( normalize( derivVec3( eye ) - getXYZ( world ) ) ) ); // V
		inits.push_back( makeExpr( normal ) ); // N
		inits.push_back( makeExpr( derivVec3( 0.0_f ) ) ); // L
		inits.push_back( makeExpr( derivVec3( 0.0_f ) ) ); // H
		inits.push_back( makeExpr( length( derivVec3( eye ) - getXYZ( world ) ) ) ); // lengthV
		inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // lengthL
		inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // NdotV

		if ( enableDotProducts )
		{
			inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // NdotL
			inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // NdotH
			inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // HdotV
		}

		if ( enableFresnel )
		{
			inits.push_back( makeExpr( derivVec3( 0.0_f ) ) ); // F

			if ( enableIridescence )
			{
				inits.push_back( makeExpr( derivVec3( 0.0_f ) ) ); // spcF
				inits.push_back( makeExpr( derivVec3( 0.0_f ) ) ); // difF
			}
		}

		return sdw::makeAggrInit( type, castor::move( inits ) );
	}
}
