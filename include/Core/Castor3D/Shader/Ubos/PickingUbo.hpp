/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PickingUbo_H___
#define ___C3D_PickingUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>

namespace castor3d
{
	namespace shader
	{
		struct PickingData
			: public sdw::StructInstance
		{
			C3D_API PickingData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, PickingData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec4 getIndex( sdw::UInt const & instance
				, sdw::Int const & primitive )const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Int m_drawIndex;
			sdw::Int m_nodeIndex;
		};
	}

	class PickingUbo
	{
	public:
		using Configuration = PickingUboConfiguration;
		C3D_API PickingUbo( PickingUbo const & rhs ) = delete;
		C3D_API PickingUbo & operator=( PickingUbo const & rhs ) = delete;
		C3D_API PickingUbo( PickingUbo && rhs ) = default;
		C3D_API PickingUbo & operator=( PickingUbo && rhs ) = delete;
		C3D_API explicit PickingUbo( RenderDevice const & device );
		C3D_API ~PickingUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	configuration	The configuration
		 *\param[in]	drawIndex		The draw call index.
		 *\param[in]	nodeIndex		The node index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	configuration	La configuration.
		 *\param[in]	drawIndex		L'indice de la fonction de dessin.
		 *\param[in]	nodeIndex		L'indice du noeud.
		 */
		C3D_API static void update( Configuration & configuration
			, uint32_t drawIndex
			, uint32_t nodeIndex );

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		C3D_API static castor::String const BufferPicking;
		C3D_API static castor::String const PickingData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_PICKING( writer, binding, set )\
	sdw::Ubo picking{ writer\
		, castor3d::PickingUbo::BufferPicking\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_pickingData = picking.declMember< castor3d::shader::PickingData >( castor3d::PickingUbo::PickingData );\
	picking.end()

#endif
