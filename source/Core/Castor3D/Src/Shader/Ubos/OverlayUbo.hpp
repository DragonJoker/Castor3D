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
			castor::Point4f positionRatio;
			castor::Point4i renderSizeIndex;
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
		inline ashes::UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

	public:
		C3D_API static uint32_t const BindingPoint;
		//!\~english	Name of the overlay information frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les informations de l'incrustation.
		C3D_API static castor::String const BufferOverlayName;
		//!\~english	Name of the overlay information frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les informations de l'incrustation.
		C3D_API static castor::String const BufferOverlayInstance;
		//!\~english	Name of the overlay position (xy) and render ratio (zw) frame variable.
		//!\~french		Nom de la frame variable contenant la position (xy) et le ratio de rendu (zw) de l'incrustation.
		C3D_API static castor::String const PositionRatio;
		//!\~english	Name of the render size (xy) and material index (z) frame variable.
		//!\~french		Nom de la frame variable contenant la taille de rendu (xy) et l'index du matériau (z).
		C3D_API static castor::String const RenderSizeIndex;

	private:
		Engine & m_engine;
		ashes::UniformBufferPtr< Configuration > m_ubo;
	};
}

#define UBO_OVERLAY( writer, binding, set )\
	sdw::Ubo overlay{ writer, OverlayUbo::BufferOverlayName, binding, set };\
	auto c3d_positionRatio = overlay.declMember< sdw::Vec4 >( castor3d::OverlayUbo::PositionRatio );\
	auto c3d_renderSizeIndex = overlay.declMember< sdw::IVec4 >( castor3d::OverlayUbo::RenderSizeIndex );\
	overlay.end()

#endif
