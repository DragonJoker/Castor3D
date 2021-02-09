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
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PanelOverlay();
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
		 *\brief		Retrieves the panel vertex buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de sommets du panneau
		 *\return		Le tampon
		 */
		inline OverlayCategory::VertexArray const & getPanelVertex()const
		{
			return m_arrayVtx;
		}

	protected:
		/**
		 *\copydoc	castor3d::OverlayCategory::doUpdateBuffer
		 */
		C3D_API void doUpdateBuffer( castor::Size const & size )override;

	protected:
		//!\~english	The vertex buffer data.
		//!\~french		Les données du tampon de sommets.
		VertexArray m_arrayVtx;
	};
}

#endif
