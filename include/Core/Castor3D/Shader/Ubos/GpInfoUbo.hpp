/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpInfoUbo_H___
#define ___C3D_GpInfoUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
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
		 *\~french
		 *\brief		Initialise l'UBO.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		C3D_API void cpuUpdate( castor::Size const & renderSize
			, Camera const & camera );
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		inline UniformBufferOffsetT< Configuration > & getUbo()
		{
			return m_ubo;
		}

		inline UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}
		/**@}*/

	public:
		C3D_API static const castor::String GPInfo;
		C3D_API static const castor::String InvViewProj;
		C3D_API static const castor::String InvView;
		C3D_API static const castor::String InvProj;
		C3D_API static const castor::String View;
		C3D_API static const castor::String Proj;
		C3D_API static const castor::String RenderSize;
		C3D_API static uint32_t const BindingPoint;

	private:
		Engine & m_engine;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_GPINFO( writer, binding, set )\
	sdw::Ubo gpInfo{ writer\
		, castor3d::GpInfoUbo::GPInfo\
		, binding\
		, set };\
	auto c3d_mtxInvViewProj = gpInfo.declMember< sdw::Mat4 >( castor3d::GpInfoUbo::InvViewProj );\
	auto c3d_mtxInvView = gpInfo.declMember< sdw::Mat4 >( castor3d::GpInfoUbo::InvView );\
	auto c3d_mtxInvProj = gpInfo.declMember< sdw::Mat4 >( castor3d::GpInfoUbo::InvProj );\
	auto c3d_mtxGView = gpInfo.declMember< sdw::Mat4 >( castor3d::GpInfoUbo::View );\
	auto c3d_mtxGProj = gpInfo.declMember< sdw::Mat4 >( castor3d::GpInfoUbo::Proj );\
	auto c3d_renderSize = gpInfo.declMember< sdw::Vec2 >( castor3d::GpInfoUbo::RenderSize );\
	gpInfo.end()

#endif
