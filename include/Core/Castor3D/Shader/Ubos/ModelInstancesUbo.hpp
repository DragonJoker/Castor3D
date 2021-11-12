/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelInstancesUbo_H___
#define ___C3D_ModelInstancesUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct ModelInstancesData
			: public sdw::StructInstance
		{
			C3D_API ModelInstancesData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, ModelInstancesData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		public:
			sdw::Array< sdw::UVec4 > instances;
			sdw::UInt instanceCount;
		};
	}

	class ModelInstancesUbo
	{
	public:
		using Configuration = ModelInstancesUboConfiguration;
		C3D_API ModelInstancesUbo( ModelInstancesUbo const & rhs ) = delete;
		C3D_API ModelInstancesUbo & operator=( ModelInstancesUbo const & rhs ) = delete;
		C3D_API ModelInstancesUbo( ModelInstancesUbo && rhs ) = default;
		C3D_API ModelInstancesUbo & operator=( ModelInstancesUbo && rhs ) = delete;
		C3D_API explicit ModelInstancesUbo( RenderDevice const & device );
		C3D_API ~ModelInstancesUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	instances	The instances data.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	instances	Les données d'instance.
		 */
		C3D_API void cpuUpdate( UInt32Array const & instances );

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
		C3D_API static castor::String const BufferModelInstances;
		C3D_API static castor::String const ModelInstancesData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
	void cpuUpdate( UniformBufferOffsetT< ModelInstancesUboConfiguration > & buffer
		, UInt32Array const & instances );
}

#define UBO_MODEL_INSTANCES( writer, binding, set )\
	sdw::Ubo modelInstances{ writer\
		, castor3d::ModelInstancesUbo::BufferModelInstances\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_modelInstancesData = modelInstances.declStructMember< castor3d::shader::ModelInstancesData >( castor3d::ModelInstancesUbo::ModelInstancesData );\
	modelInstances.end()

#endif
