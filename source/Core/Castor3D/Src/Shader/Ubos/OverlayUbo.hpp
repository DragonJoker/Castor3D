/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayUbo_H___
#define ___C3D_OverlayUbo_H___

#include "Castor3DPrerequisites.hpp"

#include <Buffer/UniformBuffer.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Matrices Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour les matrices.
	*/
	class OverlayUbo
	{
	private:
		struct Configuration
		{
			castor::Point2f position;
			castor::Point2i renderSize;
			castor::Point2f renderRatio;
			int32_t materialIndex;
		};

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API OverlayUbo( OverlayUbo const & ) = delete;
		C3D_API OverlayUbo & operator=( OverlayUbo const & ) = delete;
		C3D_API OverlayUbo( OverlayUbo && ) = default;
		C3D_API OverlayUbo & operator=( OverlayUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit OverlayUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~OverlayUbo();
		/**
		 *\~english
		 *\brief		Sets the overlay position and render ratio.
		 *\param[in]	position	The new value.
		 *\param[in]	renderSize	The current render size.
		 *\param[in]	renderRatio	The ratio between computed position value's reference size and current render size.
		 *\~french
		 *\brief		Définit la la position de l'incrustation et le ratio de rendu.
		 *\param[in]	position	La nouvelle valeur.
		 *\param[in]	renderSize	Les dimensions de rendu actuelles.
		 *\param[in]	renderRatio	Le ratio entre les dimensions de référence lors du calcul de la position, et les dimensions de rendu actuelles.
		 */
		C3D_API void setPosition( castor::Point2d const & position
			, castor::Size const & renderSize
			, castor::Point2f const & renderRatio );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	materialIndex	The overlay's material index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	materialIndex	L'index du matériau de l'incrustation.
		 */
		C3D_API void update( int materialIndex );
		/**
		 *\~english
		 *\name			Getters.
		 *\~french
		 *\name			Getters.
		 */
		inline renderer::UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

	public:
		static constexpr uint32_t BindingPoint = 2u;
		//!\~english	Name of the overlay information frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les informations de l'incrustation.
		C3D_API static castor::String const BufferOverlayName;
		//!\~english	Name of the overlay information frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les informations de l'incrustation.
		C3D_API static castor::String const BufferOverlayInstance;
		//!\~english	Name of the overlay position frame variable.
		//!\~french		Nom de la frame variable contenant la position de l'incrustation.
		C3D_API static castor::String const Position;
		//!\~english	Name of the render ratio frame variable.
		//!\~french		Nom de la frame variable contenant le ratio de rendu.
		C3D_API static castor::String const RenderSize;
		//!\~english	Name of the render ratio frame variable.
		//!\~french		Nom de la frame variable contenant le ratio de rendu.
		C3D_API static castor::String const RenderRatio;
		//!\~english	Name of the material index frame variable.
		//!\~french		Nom de la frame variable contenant l'indice du matériau.
		C3D_API static castor::String const MaterialIndex;

	private:
		Engine & m_engine;
		renderer::UniformBufferPtr< Configuration > m_ubo;
	};
}

#define UBO_OVERLAY( writer, binding, set )\
	glsl::Ubo overlay{ writer, OverlayUbo::BufferOverlayName, binding, set };\
	auto c3d_position = overlay.declMember< glsl::Vec2 >( castor3d::OverlayUbo::Position );\
	auto c3d_renderSize = overlay.declMember< glsl::IVec2 >( castor3d::OverlayUbo::RenderSize );\
	auto c3d_renderRatio = overlay.declMember< glsl::Vec2 >( castor3d::OverlayUbo::RenderRatio );\
	auto c3d_materialIndex = overlay.declMember< glsl::Int >( castor3d::OverlayUbo::MaterialIndex );\
	overlay.end()

#endif
