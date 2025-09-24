/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLightSurface_H___
#define ___C3D_GlslLightSurface_H___

#include "SdwModule.hpp"

#include "GlslDerivativeValue.hpp"

#include <ShaderWriter/VecTypes/Vec3.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace c3d::shader
{
	struct LightSurface
		: public sdw::StructInstance
	{
		SDW_DeclStructInstance( C3D_API, LightSurface );

		C3D_API LightSurface( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API LightSurface( sdw::Vec3 const & eye
			, DerivVec4 const & world
			, DerivVec3 const & view
			, sdw::Vec3 const & clip
			, DerivVec3 const & normal
			, bool enableDotProducts );

		C3D_API static sdw::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, sdw::type::TypePtr type );
		C3D_API static sdw::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, bool enableDotProducts );
		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, LightSurface const & rhs );

		C3D_API static LightSurface create( sdw::ShaderWriter & writer
			, MbString const & name
			, sdw::Vec3 const & eye
			, DerivVec4 const & world
			, DerivVec3 const & view
			, sdw::Vec3 const & clip
			, DerivVec3 const & normal
			, bool enableDotProducts = true );
		C3D_API static LightSurface create( sdw::ShaderWriter & writer
			, MbString const & name
			, DerivVec4 const & world
			, sdw::Vec3 const & clip
			, DerivVec3 const & normal
			, bool enableDotProducts = false );

		C3D_API void updateN( DerivVec3 const & N )const;
		C3D_API void updateL( DerivVec3 const & VtoL )const;
		C3D_API void registerDebug( DebugOutputCategory const & debugOutput )const;

		auto eyePosition()const { return m_eyePosition; }
		auto worldPosition()const { return m_worldPosition; }
		auto viewPosition()const { return m_viewPosition; }
		auto clipPosition()const { return m_clipPosition; }
		auto vertexToLight()const { return m_vertexToLight; }
		auto V()const { return m_V; }
		auto N()const { return m_N; }
		auto L()const { return m_L; }
		auto H()const { return m_H; }
		auto lengthV()const { return m_lengthV; }
		auto lengthL()const { return m_lengthL; }
		auto NdotV()const { return m_NdotV; }
		DerivFloat NdotL()const { return m_NdotL; }
		DerivFloat NdotH()const { return m_NdotH; }
		DerivFloat LdotV()const { return m_LdotV; }
		DerivFloat HdotL()const { return m_HdotL; }
		DerivFloat HdotV()const { return m_HdotV; }

	private:
		sdw::Vec3 m_eyePosition;
		DerivVec4 m_worldPosition;
		DerivVec3 m_viewPosition;
		sdw::Vec3 m_clipPosition;
		DerivVec3 m_vertexToLight;
		DerivVec3 m_V;
		DerivVec3 m_N;
		DerivVec3 m_L;
		DerivVec3 m_H;
		DerivFloat m_lengthV;
		DerivFloat m_lengthL;
		mutable DerivFloat m_NdotV;

		mutable sdw::DefaultedT< DerivFloat > m_NdotL;
		mutable sdw::DefaultedT< DerivFloat > m_NdotH;
		mutable sdw::DefaultedT< DerivFloat > m_HdotV;
		mutable sdw::DefaultedT< DerivFloat > m_LdotV;
		mutable sdw::DefaultedT< DerivFloat > m_HdotL;

	private:
		C3D_API static sdw::expr::ExprPtr makeInit( sdw::type::BaseStructPtr type
			, sdw::Vec3 const & eye
			, DerivVec4 const & world
			, DerivVec3 const & view
			, sdw::Vec3 const & clip
			, DerivVec3 const & normal
			, bool enableDotProducts );
	};
}

#endif
