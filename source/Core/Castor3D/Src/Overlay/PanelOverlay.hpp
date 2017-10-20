/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PANEL_OVERLAY_H___
#define ___C3D_PANEL_OVERLAY_H___

#include "OverlayCategory.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/08/2010
	\~english
	\brief		A simple rectangular overlay
	\~french
	\brief		Une simple incrustation rectangulaire
	*/
	class PanelOverlay
		: public OverlayCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		PanelOverlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		PanelOverlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class TextWriter
			: public OverlayCategory::TextWriter
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextWriter( castor::String const & p_tabs, PanelOverlay const * p_category = nullptr );
			/**
			 *\~english
			 *\brief		Saves an overlay into a text file
			 *\param[in]	p_file		the file to save the overlay in
			 *\param[in]	p_overlay	the overlay to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde l'incrustation donnée dans un fichier texte
			 *\param[in]	p_file		Le fichier où enregistrer l'incrustation
			 *\param[in]	p_overlay	L'incrustation à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			C3D_API bool operator()( PanelOverlay const & p_overlay, castor::TextFile & p_file );
			/**
			 *\copydoc		castor3d::OverlayCategory::TextWriter::writeInto
			 */
			C3D_API bool writeInto( castor::TextFile & p_file )override;

		private:
			PanelOverlay const * m_category;
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
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~PanelOverlay();
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
		 *\copydoc		castor3d::OverlayCategory::createTextWriter
		 */
		C3D_API std::unique_ptr < OverlayCategory::TextWriter > createTextWriter( castor::String const & p_tabs )override
		{
			return std::make_unique< TextWriter >( p_tabs, this );
		}
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
		 *\~english
		 *\brief		Draws the overlay
		 *\param[in]	p_renderer	The renderer used to draw this overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 *\param[in]	p_renderer	Le renderer utilisé pour dessiner cette incrustation
		 */
		C3D_API virtual void doRender( OverlayRendererSPtr p_renderer );
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\param[in]	p_size	The render target size.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 *\param[in]	p_size	Les dimensions de la cible de rendu.
		 */
		C3D_API virtual void doUpdateBuffer( castor::Size const & p_size );

	protected:
		//!\~english The vertex buffer data	\~french Les données du tampon de sommets
		VertexArray m_arrayVtx;
	};
}

#endif
