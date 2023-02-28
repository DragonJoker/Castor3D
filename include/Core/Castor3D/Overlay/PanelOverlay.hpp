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
		C3D_API uint32_t fillBuffer( Vertex * buffer
			, bool secondary )const;

	protected:
		/**
		 *\copydoc	castor3d::OverlayCategory::doUpdateBuffer
		 */
		C3D_API void doUpdateBuffer( castor::Size const & size )override;

	protected:
		castor::Size m_refSize;
	};
}

#endif
