/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BorderPanelOverlay_H___
#define ___C3D_BorderPanelOverlay_H___

#include "Castor3D/Overlay/OverlayCategory.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace castor3d
{
	class BorderPanelOverlay
		: public OverlayCategory
	{
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
		 *\param[in]	borders	\p true for borders, \p false for center.
		 *\return		The vertex count needed for this overlay.
		 *\~french
		 *\param[in]	borders	\p true pour les bordures, \p false pour le centre.
		 *\return		Le nombre de sommets nécessaires pour cette incrustation.
		 */
		C3D_API uint32_t getCount( bool borders )const;
		/**
		 *\~english
		 *\brief		Sets the border material
		 *\param[in]	material	The new value
		 *\~french
		 *\brief		Définit le matériau des bords
		 *\param[in]	material	La nouvelle valeur
		 */
		C3D_API void setBorderMaterial( MaterialRPtr material );
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay borders size, in pixels
		 *\param[in]	size	The screen size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue des bordures de l'incrustation
		 *\param[in]	size	La taille de l'écran
		 *\return		La taille
		 */
		C3D_API castor::Rectangle getAbsoluteBorderSize( castor::Size const & size )const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay borders size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue des bordures de l'incrustation
		 *\return		La taille
		 */
		C3D_API castor::Point4d getAbsoluteBorderSize()const;
		/**
		 *\~english
		 *\brief		Creates the shader program used to compute the overlay's vertices.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme utilisé pour calculer les sommets de l'incrustation.
		 *\return		Le programme.
		 */
		C3D_API static ashes::PipelineShaderStageCreateInfo createProgram( RenderDevice const & device );
		/**
		 *\~english
		 *\return		\p true if this overlay's has changed.
		 *\~french
		 *\return		\p true si cette incrustation a changé.
		 */
		C3D_API bool isChanged()const override
		{
			return m_borderChanged;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord gauche
		 *\return		La valeur
		 */
		double getLeftBorderSize()const
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
		double getTopBorderSize()const
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
		double getRightBorderSize()const
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
		double getBottomBorderSize()const
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
		castor::Point4d const & getBorderSize()const
		{
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
		MaterialRPtr getBorderMaterial()const
		{
			return m_pBorderMaterial;
		}
		/**
		 *\~english
		 *\brief		Sets the left border thickness
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord gauche
		 *\param[in]	size	La nouvelle valeur
		 */
		void setLeftBorderSize( double size )
		{
			m_ptBorderSize[0] = size;
			m_ptBorderSize[0] = std::max( 0.0, m_ptBorderSize[0] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the top border thickness
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord haut
		 *\param[in]	size	La nouvelle valeur
		 */
		void setTopBorderSize( double size )
		{
			m_ptBorderSize[1] = size;
			m_ptBorderSize[1] = std::max( 0.0, m_ptBorderSize[1] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the right border thickness
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord droit
		 *\param[in]	size	La nouvelle valeur
		 */
		void setRightBorderSize( double size )
		{
			m_ptBorderSize[2] = size;
			m_ptBorderSize[2] = std::max( 0.0, m_ptBorderSize[2] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the bottom border thickness
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord bas
		 *\param[in]	size	La nouvelle valeur
		 */
		void setBottomBorderSize( double size )
		{
			m_ptBorderSize[3] = size;
			m_ptBorderSize[3] = std::max( 0.0, m_ptBorderSize[3] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the borders thicknesses
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur des bords
		 *\param[in]	size	La nouvelle valeur
		 */
		void setBorderSize( castor::Point4d const & size )
		{
			m_ptBorderSize = size;
			m_ptBorderSize[0] = std::max( 0.0, m_ptBorderSize[0] );
			m_ptBorderSize[1] = std::max( 0.0, m_ptBorderSize[1] );
			m_ptBorderSize[2] = std::max( 0.0, m_ptBorderSize[2] );
			m_ptBorderSize[3] = std::max( 0.0, m_ptBorderSize[3] );
			m_sizeChanged = true;
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
		int getLeftBorderPixelSize()const
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
		int getTopBorderPixelSize()const
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
		int getRightBorderPixelSize()const
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
		int getBottomBorderPixelSize()const
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
		castor::Rectangle const & getBorderPixelSize()const
		{
			return m_borderSize;
		}
		/**
		 *\~english
		 *\brief		Sets the left border thickness
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord gauche
		 *\param[in]	size	La nouvelle valeur
		 */
		void setLeftBorderPixelSize( int size )
		{
			m_borderSize[0] = size;
			m_borderSize[0] = std::max( 0, m_borderSize[0] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the top border thickness
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord haut
		 *\param[in]	size	La nouvelle valeur
		 */
		void setTopBorderPixelSize( int size )
		{
			m_borderSize[1] = size;
			m_borderSize[1] = std::max( 0, m_borderSize[1] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the right border thickness
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord droit
		 *\param[in]	size	La nouvelle valeur
		 */
		void setRightBorderPixelSize( int size )
		{
			m_borderSize[2] = size;
			m_borderSize[2] = std::max( 0, m_borderSize[2] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the bottom border thickness
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord bas
		 *\param[in]	size	La nouvelle valeur
		 */
		void setBottomBorderPixelSize( int size )
		{
			m_borderSize[3] = size;
			m_borderSize[3] = std::max( 0, m_borderSize[3] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the borders thicknesses
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit l'épaisseur des bords
		 *\param[in]	size	La nouvelle valeur
		 */
		void setBorderPixelSize( castor::Rectangle const & size )
		{
			m_borderSize = size;
			m_borderSize[0] = std::max( 0, m_borderSize[0] );
			m_borderSize[1] = std::max( 0, m_borderSize[1] );
			m_borderSize[2] = std::max( 0, m_borderSize[2] );
			m_borderSize[3] = std::max( 0, m_borderSize[3] );
			m_sizeChanged = true;
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
		BorderPosition getBorderPosition()const
		{
			return m_borderPosition;
		}
		/**
		 *\~english
		 *\brief		Sets the border position
		 *\param[in]	position	The new value
		 *\~french
		 *\brief		Définit la position de la bordure
		 *\param[in]	position	La nouvelle valeur
		 */
		void setBorderPosition( BorderPosition position )
		{
			m_borderPosition = position;
		}
		/**
		 *\~english
		 *\brief		Sets the borders outer part UV
		 *\param[in]	value	The new value (left, top, right and bottom)
		 *\~french
		 *\brief		Définit Les UV de la partie extérieure de la bordure
		 *\param[in]	value	La nouvelle valeur (gauche, haut, droite, bas)
		 */
		void setBorderOuterUV( castor::Point4d const & value )
		{
			m_borderOuterUv = value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders outer part UV
		 *\return		The value (left, top, right and bottom)
		 *\~french
		 *\brief		Récupère Les UV de la partie extérieure de la bordure
		 *\return		La valeur (gauche, haut, droite, bas)
		 */
		castor::Point4d const & getBorderOuterUV()const
		{
			return m_borderOuterUv;
		}
		/**
		 *\~english
		 *\brief		Sets the borders inner part UV
		 *\param[in]	value	The new value (left, top, right and bottom)
		 *\~french
		 *\brief		Définit Les UV de la partie intérieure de la bordure
		 *\param[in]	value	La nouvelle valeur (gauche, haut, droite, bas)
		 */
		void setBorderInnerUV( castor::Point4d const & value )
		{
			m_borderInnerUv = value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders inner part UV
		 *\return		The value (left, top, right and bottom)
		 *\~french
		 *\brief		Récupère Les UV de la partie intérieure de la bordure
		 *\return		La valeur (gauche, haut, droite, bas)
		 */
		castor::Point4d const & getBorderInnerUV()const
		{
			return m_borderInnerUv;
		}

	protected:
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\param[in]	size	The render target size.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 *\param[in]	size	Les dimensions de la cible de rendu.
		 */
		C3D_API void doUpdate( OverlayRenderer const & renderer )override;
		/**
		 *\~english
		 *\brief		Updates the overlay size, taking care of wanted pixel size.
		 *\~french
		 *\brief		Met à jour la taille de l'incrustation, en prenant en compte la taille en pixel voulue.
		 */
		C3D_API void doUpdateSize( OverlayRenderer const & renderer )override;

	protected:
		//!\~english	The border material.
		//!\~french		Le matériau des bords.
		MaterialRPtr m_pBorderMaterial;
		//!\~english	The border size.
		//!\~french		La taille des bords.
		castor::Point4d m_ptBorderSize;
		//!\~english	The absolute size in pixels.
		//!\~french		La taille absolue en pixels.
		castor::Rectangle m_borderSize;
		//!\~english	The border material name.
		//!\~french		Le nom du matériau des bords.
		BorderPosition m_borderPosition{ BorderPosition::eInternal };
		//!\~english	The UV for the outer part of the border.
		//!\~french		Les UV de la partie extérieure de la bordure.
		castor::Point4d m_borderOuterUv;
		//!\~english	The UV for the inner part of the border.
		//!\~french		Les UV de la partie intérieure de la bordure.
		castor::Point4d m_borderInnerUv;
		//!\~english	Tells if the border has changed, in any way.
		//!\~french		Dit si la bordure a changé, de quelque manière que ce soit.
		bool m_borderChanged;
	};
}

#endif
