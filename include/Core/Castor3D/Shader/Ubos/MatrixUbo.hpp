/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MatrixUbo_H___
#define ___C3D_MatrixUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"

namespace castor3d
{
	class MatrixUbo
	{
	public:
		struct Configuration
		{
			castor::Matrix4x4f projection;
			castor::Matrix4x4f invProjection;
			castor::Matrix4x4f curView;
			castor::Matrix4x4f prvView;
			castor::Matrix4x4f curViewProj;
			castor::Matrix4x4f prvViewProj;
			castor::Point2f jitter;
		};

	public:
		/**
		*\~english
		*\name
		*	Copy/Move construction/assignment operation.
		*\~french
		*\name
		*	Constructeurs/Opérateurs d'affectation par copie/déplacement.
		*/
		/**@{*/
		C3D_API MatrixUbo( MatrixUbo const & ) = delete;
		C3D_API MatrixUbo & operator=( MatrixUbo const & ) = delete;
		C3D_API MatrixUbo( MatrixUbo && ) = default;
		C3D_API MatrixUbo & operator=( MatrixUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit MatrixUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MatrixUbo();
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
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	view		The new view matrix.
		 *\param[in]	projection	The new projection matrix.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	view		La nouvelle matrice de vue.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API void update( castor::Matrix4x4f const & view
			, castor::Matrix4x4f const & projection
			, castor::Point2f const & jitter = castor::Point2f{} )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	view			The new view matrix.
		 *\param[in]	projection		The new projection matrix.
		 *\param[in]	jitter			The jittering value.
		 *\param[in]	stagingBuffer	The staging buffer used to transfer the data.
		 *\param[in]	commandBuffer	The command buffer on which the transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	view			La nouvelle matrice de vue.
		 *\param[in]	projection		La nouvelle matrice de projection.
		 *\param[in]	jitter			La valeur de jittering.
		 *\param[in]	stagingBuffer	Le staging buffer utilisé pour effectuer le transfer.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void update( castor::Matrix4x4f const & view
			, castor::Matrix4x4f const & projection
			, castor::Point2f const & jitter
			, ashes::StagingBuffer & stagingBuffer
			, ashes::CommandBuffer const & commandBuffer )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\remarks		View matrix won't be updated.
		 *\param[in]	projection	The new projection matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\remarks		La matrice de vue ne sera pas mise à jour.
		 *\param[in]	projection	La nouvelle matrice de projection.
		 */
		C3D_API void update( castor::Matrix4x4f const & projection )const;
		/**
		 *\~english
		 *\name			Getters.
		 *\~french
		 *\name			Accesseurs.
		 */
		inline UniformBuffer< Configuration > & getUbo()
		{
			return *m_ubo;
		}

		inline UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

	public:
		C3D_API static uint32_t const BindingPoint;
		C3D_API static castor::String const BufferMatrix;
		C3D_API static castor::String const Projection;
		C3D_API static castor::String const CurView;
		C3D_API static castor::String const PrvView;
		C3D_API static castor::String const CurViewProj;
		C3D_API static castor::String const PrvViewProj;
		C3D_API static castor::String const InvProjection;
		C3D_API static castor::String const Jitter;

	private:
		Engine & m_engine;
		UniformBufferUPtr< Configuration > m_ubo;
	};
}

#define UBO_MATRIX( writer, binding, set )\
	sdw::Ubo matrices{ writer\
		, castor3d::MatrixUbo::BufferMatrix\
		, binding\
		, set };\
	auto c3d_projection = matrices.declMember< sdw::Mat4 >( castor3d::MatrixUbo::Projection );\
	auto c3d_invProjection = matrices.declMember< sdw::Mat4 >( castor3d::MatrixUbo::InvProjection );\
	auto c3d_curView = matrices.declMember< sdw::Mat4 >( castor3d::MatrixUbo::CurView );\
	auto c3d_prvView = matrices.declMember< sdw::Mat4 >( castor3d::MatrixUbo::PrvView );\
	auto c3d_curViewProj = matrices.declMember< sdw::Mat4 >( castor3d::MatrixUbo::CurViewProj );\
	auto c3d_prvViewProj = matrices.declMember< sdw::Mat4 >( castor3d::MatrixUbo::PrvViewProj );\
	auto c3d_jitter = matrices.declMember< sdw::Vec2 >( castor3d::MatrixUbo::Jitter );\
	matrices.end()

#endif
