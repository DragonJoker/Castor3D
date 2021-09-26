/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayUbo_H___
#define ___C3D_OverlayUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct OverlayData
			: public sdw::StructInstance
		{
			C3D_API OverlayData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, OverlayData );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec2 getOverlaySize()const;
			C3D_API sdw::Vec2 modelToView( sdw::Vec2 const & pos )const;
			C3D_API sdw::UInt getMaterialIndex()const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Vec4 m_positionRatio;
			sdw::IVec4 m_renderSizeIndex;
		};
	}

	class OverlayUbo
	{
	private:
		using Configuration = OverlayUboConfiguration;

	public:
		C3D_API static castor::String const BufferOverlay;
		C3D_API static castor::String const OverlayData;
	};
}

#define UBO_OVERLAY( writer, binding, set )\
	sdw::Ubo overlay{ writer\
		, castor3d::OverlayUbo::BufferOverlay\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_overlayData = overlay.declStructMember< castor3d::shader::OverlayData >( castor3d::OverlayUbo::OverlayData );\
	overlay.end()

#endif
