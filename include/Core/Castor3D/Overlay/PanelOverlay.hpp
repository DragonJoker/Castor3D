/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PanelOverlay_H___
#define ___C3D_PanelOverlay_H___

#include "Castor3D/Overlay/OverlayCategory.hpp"

namespace castor3d
{
	class PanelOverlay
		: public OverlayCategory
	{
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
		C3D_API static OverlayCategorySPtr create();
		/**
		 *\copydoc	castor3d::OverlayCategory::accept
		 */
		C3D_API void accept( OverlayVisitor & visitor )const override;
		/**
		 *\~english
		 *\brief		Fills the given buffer.
		 *\param[out]	buffer	The buffer.
		 *\~french
		 *\brief		Remplit le tampon de sommets donné.
		 *\param[out]	buffer	Le buffer.
		 */
		C3D_API uint32_t fillBuffer( castor::Size const & refSize
			, Vertex * buffer
			, bool secondary )const;

	private:
		void doUpdate( OverlayRenderer const & renderer )override;
	};
}

#endif
