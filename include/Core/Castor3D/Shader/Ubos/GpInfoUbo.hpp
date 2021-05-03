/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpInfoUbo_H___
#define ___C3D_GpInfoUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct GpInfoData
			: public sdw::StructInstance
		{
			C3D_API GpInfoData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API GpInfoData & operator=( GpInfoData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec3 readNormal( sdw::Vec3 const & input )const;
			C3D_API sdw::Vec3 writeNormal( sdw::Vec3 const & input )const;
			C3D_API sdw::Vec3 projToWorld( Utils const & utils
				, sdw::Vec2 const & texCoord
				, sdw::Float const & depth )const;
			C3D_API sdw::Vec3 projToView( Utils const & utils
				, sdw::Vec2 const & texCoord
				, sdw::Float const & depth )const;
			C3D_API sdw::Vec2 calcTexCoord( Utils const & utils
				, sdw::Vec2 const & fragCoord )const;

			sdw::Mat4 const & getInvViewProj()const
			{
				return m_mtxInvViewProj;
			}

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Mat4 m_mtxInvViewProj;
			sdw::Mat4 m_mtxInvView;
			sdw::Mat4 m_mtxInvProj;
			sdw::Mat4 m_mtxGView;
			sdw::Mat4 m_mtxGProj;
			sdw::Vec2 m_renderSize;
		};
	}

	class GpInfoUbo
	{
	public:
		using Configuration = GpInfoUboConfiguration;

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API GpInfoUbo( GpInfoUbo const & ) = delete;
		C3D_API GpInfoUbo & operator=( GpInfoUbo const & ) = delete;
		C3D_API GpInfoUbo( GpInfoUbo && ) = default;
		C3D_API GpInfoUbo & operator=( GpInfoUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		explicit GpInfoUbo( Engine & engine );
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The GPU device.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le device GPU.
		 */
		explicit GpInfoUbo( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~GpInfoUbo();
		/**
		 *\~english
		 *\brief		Initialises the UBO.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise l'UBO.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		C3D_API void cpuUpdate( castor::Size const & renderSize
			, Camera const & camera );

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
		C3D_API static const castor::String BufferGPInfo;
		C3D_API static const castor::String GPInfoData;

	private:
		Engine & m_engine;
		RenderDevice const * m_device{};
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_GPINFO( writer, binding, set )\
	sdw::Ubo gpInfo{ writer\
		, castor3d::GpInfoUbo::BufferGPInfo\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_gpInfoData = gpInfo.declStructMember< shader::GpInfoData >( castor3d::GpInfoUbo::GPInfoData );\
	gpInfo.end()

#endif
