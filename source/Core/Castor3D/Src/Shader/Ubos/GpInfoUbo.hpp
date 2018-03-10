/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpInfoUbo_H___
#define ___C3D_GpInfoUbo_H___

#include "Castor3DPrerequisites.hpp"

#include <Buffer/UniformBuffer.hpp>

namespace castor3d
{
	class GpInfoUbo
	{
	private:
		struct Configuration
		{
			renderer::Mat4 invViewProj;
			renderer::Mat4 invView;
			renderer::Mat4 invProj;
			renderer::Mat4 gView;
			renderer::Mat4 gProj;
			castor::Point2f renderSize;
		};

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
		C3D_API GpInfoUbo & operator=( GpInfoUbo && ) = default;
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
		void update( castor::Size const & renderSize
			, Camera const & camera
			, castor::Matrix4x4r const & invViewProj
			, castor::Matrix4x4r const & invView
			, castor::Matrix4x4r const & invProj );
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		inline renderer::UniformBuffer< Configuration > & getUbo()
		{
			return *m_ubo;
		}

		inline renderer::UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

	public:
		static const castor::String GPInfo;
		static const castor::String InvViewProj;
		static const castor::String InvView;
		static const castor::String InvProj;
		static const castor::String View;
		static const castor::String Proj;
		static const castor::String RenderSize;
		static constexpr uint32_t BindingPoint = 4u;

	private:
		Engine & m_engine;
		renderer::UniformBufferPtr< Configuration > m_ubo;
	};
}

#define UBO_GPINFO( writer, binding, set )\
	glsl::Ubo gpInfo{ writer\
		, castor3d::GpInfoUbo::GPInfo\
		, binding\
		, set };\
	auto c3d_mtxInvViewProj = gpInfo.declMember< glsl::Mat4 >( castor3d::GpInfoUbo::InvViewProj );\
	auto c3d_mtxInvView = gpInfo.declMember< glsl::Mat4 >( castor3d::GpInfoUbo::InvView );\
	auto c3d_mtxInvProj = gpInfo.declMember< glsl::Mat4 >( castor3d::GpInfoUbo::InvProj );\
	auto c3d_mtxGView = gpInfo.declMember< glsl::Mat4 >( castor3d::GpInfoUbo::View );\
	auto c3d_mtxGProj = gpInfo.declMember< glsl::Mat4 >( castor3d::GpInfoUbo::Proj );\
	auto c3d_renderSize = gpInfo.declMember< glsl::Vec2 >( castor3d::GpInfoUbo::RenderSize );\
	gpInfo.end()

#endif
