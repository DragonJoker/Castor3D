/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_BORDER_PANEL_OVERLAY_H___
#define ___C3D_BORDER_PANEL_OVERLAY_H___

#include "OverlayCategory.hpp"

#include <Graphics/Rectangle.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/08/2010
	\~english
	\brief		A rectangular overlay with a border
	\remark		Uses a spceific material for the border
	\~french
	\brief		Une incrustation rectangulaire avec un bord
	\remark		Utilise un matériau spécifique pour le bord
	*/
	class BorderPanelOverlay
		: public OverlayCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		BorderPanelOverlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		BorderPanelOverlay loader
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
			C3D_API TextWriter( Castor::String const & p_tabs, BorderPanelOverlay const * p_category = nullptr );
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
			C3D_API bool operator()( BorderPanelOverlay const & p_overlay, Castor::TextFile & p_file );
			/**
			 *\copydoc		Castor::OverlayCategory::WriteInto
			 */
			C3D_API bool WriteInto( Castor::TextFile & p_file )override;

		private:
			BorderPanelOverlay const * m_category;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API BorderPanelOverlay();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~BorderPanelOverlay();
		/**
		 *\~english
		 *\brief		Creation function, used by the factory
		 *\return		An overlay
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		Un overlay
		 */
		C3D_API static OverlayCategorySPtr Create();
		/**
		 *\copydoc		Castor::OverlayCategory::CreateTextLoader
		 */
		C3D_API std::unique_ptr < OverlayCategory::TextWriter > CreateTextWriter( Castor::String const & p_tabs )override
		{
			return std::make_unique< TextWriter >( p_tabs, this );
		}
		/**
		 *\~english
		 *\brief		Sets the border material
		 *\param[in]	p_material	The new value
		 *\~french
		 *\brief		Définit le matériau des bords
		 *\param[in]	p_material	La nouvelle valeur
		 */
		C3D_API void SetBorderMaterial( MaterialSPtr p_material );
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay borders size, in pixels
		 *\param[in]	p_size	The screen size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue des bordures de l'incrustation
		 *\param[in]	p_size	La taille de l'écran
		 *\return		La taille
		 */
		C3D_API Castor::Rectangle GetAbsoluteBorderSize( Castor::Size const & p_size )const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay borders size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue des bordures de l'incrustation
		 *\return		La taille
		 */
		C3D_API Castor::Point4d GetAbsoluteBorderSize()const;
		/**
		 *\~english
		 *\return		\p true if this overlay's has changed.
		 *\~french
		 *\return		\p true si cette incrustation a changé.
		 */
		C3D_API virtual bool IsChanged()const
		{
			return m_borderChanged;
		}
		/**
		 *\~english
		 *\brief		Retrieves the panel vertex buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de sommets du panneau
		 *\return		Le tampon
		 */
		inline OverlayCategory::VertexArray const & GetPanelVertex()const
		{
			return m_arrayVtx;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord gauche
		 *\return		La valeur
		 */
		inline double GetLeftBorderSize()const
		{
			return m_ptBorderSize[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves the top border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord haut
		 *\return		La valeur
		 */
		inline double GetTopBorderSize()const
		{
			return m_ptBorderSize[1];
		}
		/**
		 *\~english
		 *\brief		Retrieves the right border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord droit
		 *\return		La valeur
		 */
		inline double GetRightBorderSize()const
		{
			return m_ptBorderSize[2];
		}
		/**
		 *\~english
		 *\brief		Retrieves the bottom border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord bas
		 *\return		La valeur
		 */
		inline double GetBottomBorderSize()const
		{
			return m_ptBorderSize[3];
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders thicknesses
		 *\return		The value
		 *\~french
		 *\brief		Récupère les épaisseurs des bords
		 *\return		La valeur
		 */
		inline Castor::Point4d const & GetBorderSize()const
		{
			return m_ptBorderSize;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders thicknesses
		 *\return		The value
		 *\~french
		 *\brief		Récupère les épaisseurs des bords
		 *\return		La valeur
		 */
		inline Castor::Point4d & GetBorderSize()
		{
			m_borderChanged = true;
			return m_ptBorderSize;
		}
		/**
		 *\~english
		 *\brief		Retrieves the border material
		 *\return		The value
		 *\~french
		 *\brief		Récupère le matériau des bords
		 *\return		La valeur
		 */
		inline MaterialSPtr GetBorderMaterial()const
		{
			return m_pBorderMaterial.lock();
		}
		/**
		 *\~english
		 *\brief		Sets the left border thickness
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord gauche
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetLeftBorderSize( double p_size )
		{
			m_ptBorderSize[0] = p_size;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the top border thickness
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord haut
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetTopBorderSize( double p_size )
		{
			m_ptBorderSize[1] = p_size;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the right border thickness
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord droit
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetRightBorderSize( double p_size )
		{
			m_ptBorderSize[2] = p_size;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the bottom border thickness
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord bas
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetBottomBorderSize( double p_size )
		{
			m_ptBorderSize[3] = p_size;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the borders thicknesses
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur des bords
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetBorderSize( Castor::Point4d const & p_size )
		{
			m_ptBorderSize = p_size;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord gauche
		 *\return		La valeur
		 */
		inline int GetLeftBorderPixelSize()const
		{
			return m_borderSize[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves the top border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord haut
		 *\return		La valeur
		 */
		inline int GetTopBorderPixelSize()const
		{
			return m_borderSize[1];
		}
		/**
		 *\~english
		 *\brief		Retrieves the right border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord droit
		 *\return		La valeur
		 */
		inline int GetRightBorderPixelSize()const
		{
			return m_borderSize[2];
		}
		/**
		 *\~english
		 *\brief		Retrieves the bottom border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord bas
		 *\return		La valeur
		 */
		inline int GetBottomBorderPixelSize()const
		{
			return m_borderSize[3];
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders thicknesses
		 *\return		The value
		 *\~french
		 *\brief		Récupère les épaisseurs des bords
		 *\return		La valeur
		 */
		inline Castor::Rectangle const & GetBorderPixelSize()const
		{
			return m_borderSize;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders thicknesses
		 *\return		The value
		 *\~french
		 *\brief		Récupère les épaisseurs des bords
		 *\return		La valeur
		 */
		inline Castor::Rectangle & GetBorderPixelSize()
		{
			m_borderChanged = true;
			return m_borderSize;
		}
		/**
		 *\~english
		 *\brief		Sets the left border thickness
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord gauche
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetLeftBorderPixelSize( int p_size )
		{
			m_borderSize[0] = p_size;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the top border thickness
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord haut
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetTopBorderPixelSize( int p_size )
		{
			m_borderSize[1] = p_size;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the right border thickness
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord droit
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetRightBorderPixelSize( int p_size )
		{
			m_borderSize[2] = p_size;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the bottom border thickness
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord bas
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetBottomBorderPixelSize( int p_size )
		{
			m_borderSize[3] = p_size;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the borders thicknesses
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur des bords
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetBorderPixelSize( Castor::Rectangle const & p_size )
		{
			m_borderSize = p_size;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Retrieves the border position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de la bordure
		 *\return		La valeur
		 */
		eBORDER_POSITION GetBorderPosition()const
		{
			return m_borderPosition;
		}
		/**
		 *\~english
		 *\brief		Sets the border position
		 *\param[in]	p_position	The new value
		 *\~french
		 *\brief		Définit la position de la bordure
		 *\param[in]	p_position	La nouvelle valeur
		 */
		void SetBorderPosition( eBORDER_POSITION p_position )
		{
			m_borderPosition = p_position;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders vertex buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de sommets de la bordure
		 *\return		Le tampon
		 */
		inline OverlayCategory::VertexArray const & GetBorderVertex()const
		{
			return m_arrayVtxBorder;
		}
		/**
		 *\~english
		 *\brief		Sets the borders outer part UV
		 *\param[in]	p_value	The new value (left, top, right and bottom)
		 *\~french
		 *\brief		Définit Les UV de la partie extérieure de la bordure
		 *\param[in]	p_value	La nouvelle valeur (gauche, haut, droite, bas)
		 */
		inline void SetBorderOuterUV( Castor::Point4d const & p_value )
		{
			m_borderOuterUv = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders outer part UV
		 *\return		The value (left, top, right and bottom)
		 *\~french
		 *\brief		Récupère Les UV de la partie extérieure de la bordure
		 *\return		La valeur (gauche, haut, droite, bas)
		 */
		inline Castor::Point4d const & GetBorderOuterUV()const
		{
			return m_borderOuterUv;
		}
		/**
		 *\~english
		 *\brief		Sets the borders inner part UV
		 *\param[in]	p_value	The new value (left, top, right and bottom)
		 *\~french
		 *\brief		Définit Les UV de la partie intérieure de la bordure
		 *\param[in]	p_value	La nouvelle valeur (gauche, haut, droite, bas)
		 */
		inline void SetBorderInnerUV( Castor::Point4d const & p_value )
		{
			m_borderInnerUv = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders inner part UV
		 *\return		The value (left, top, right and bottom)
		 *\~french
		 *\brief		Récupère Les UV de la partie intérieure de la bordure
		 *\return		La valeur (gauche, haut, droite, bas)
		 */
		inline Castor::Point4d const & GetBorderInnerUV()const
		{
			return m_borderInnerUv;
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
		C3D_API virtual void DoRender( OverlayRendererSPtr p_renderer );
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\param[in]	p_size	The render target size.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 *\param[in]	p_size	Les dimensions de la cible de rendu.
		 */
		C3D_API virtual void DoUpdateBuffer( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Updates the overlay size, taking care of wanted pixel size.
		 *\~french
		 *\brief		Met à jour la taille de l'incrustation, en prenant en compte la taille en pixel voulue.
		 */
		C3D_API virtual void DoUpdateSize();

	protected:
		//!\~english The border material	\~french Le matériau des bords
		MaterialWPtr m_pBorderMaterial;
		//!\~english The border size	\~french La taille des bords
		Castor::Point4d m_ptBorderSize;
		//!\~english The absolute size in pixels	\~french La taille absolue en pixels
		Castor::Rectangle m_borderSize;
		//!\~english The border material name	\~french Le nom du matériau des bords
		Castor::String m_strBorderMatName;
		//!\~english The border material name	\~french Le nom du matériau des bords
		eBORDER_POSITION m_borderPosition;
		//!\~english The vertex buffer data	\~french Les données du tampon de sommets
		VertexArray m_arrayVtx;
		//!\~english The borders vertex buffer data	\~french Les données du tampon de sommets pour la bordure
		OverlayCategory::VertexArray m_arrayVtxBorder;
		//!\~english The UV for the outer part of the border	\~french Les UV de la partie extérieure de la bordure
		Castor::Point4d m_borderOuterUv;
		//!\~english The UV for the inner part of the border	\~french Les UV de la partie intérieure de la bordure
		Castor::Point4d m_borderInnerUv;
		//!\~english Tells if the border has changed, in any way.	\~french Dit si la bordure a changé, de quelque manière que ce soit.
		bool m_borderChanged;
	};
}

#endif
