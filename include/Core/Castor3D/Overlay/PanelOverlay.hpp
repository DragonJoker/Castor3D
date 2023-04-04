/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PanelOverlay_H___
#define ___C3D_PanelOverlay_H___

#include "Castor3D/Overlay/OverlayCategory.hpp"

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace castor3d
{
	class PanelOverlay
		: public OverlayCategory
	{
	public:
		enum class ComputeBindingIdx
		{
			eCamera,
			eOverlays,
			eVertex,
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API PanelOverlay();
		/**
		 *\~english
		 *\brief		Creation function, used by the factory
		 *\return		An overlay
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		Un overlay
		 */
		C3D_API static OverlayCategoryUPtr create();
		/**
		 *\copydoc	castor3d::OverlayCategory::accept
		 */
		C3D_API void accept( OverlayVisitor & visitor )const override;
		/**
		 *\~english
		 *\return		The vertex count needed for this overlay.
		 *\~french
		 *\return		Le nombre de sommets nécessaires pour cette incrustation.
		 */
		C3D_API uint32_t getCount( bool )const;
		/**
		 *\~english
		 *\brief		Creates the shader program used to compute the overlay's vertices.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme utilisé pour calculer les sommets de l'incrustation.
		 *\return		Le programme.
		 */
		C3D_API static ashes::PipelineShaderStageCreateInfo createProgram( RenderDevice const & device );
	};
}

#endif
