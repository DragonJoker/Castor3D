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
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		bool isChanged()const noexcept override
		{
			return m_borderChanged;
		}

		double getLeftBorderSize()const noexcept
		{
			return m_ptBorderSize[0];
		}

		double getTopBorderSize()const noexcept
		{
			return m_ptBorderSize[1];
		}

		double getRightBorderSize()const noexcept
		{
			return m_ptBorderSize[2];
		}

		double getBottomBorderSize()const noexcept
		{
			return m_ptBorderSize[3];
		}

		castor::Point4d const & getBorderSize()const noexcept
		{
			return m_ptBorderSize;
		}

		MaterialRPtr getBorderMaterial()const noexcept
		{
			return m_pBorderMaterial;
		}

		int getLeftBorderPixelSize()const noexcept
		{
			return m_borderSize[0];
		}

		int getTopBorderPixelSize()const noexcept
		{
			return m_borderSize[1];
		}

		int getRightBorderPixelSize()const noexcept
		{
			return m_borderSize[2];
		}

		int getBottomBorderPixelSize()const noexcept
		{
			return m_borderSize[3];
		}

		castor::Rectangle const & getBorderPixelSize()const noexcept
		{
			return m_borderSize;
		}

		BorderPosition getBorderPosition()const noexcept
		{
			return m_borderPosition;
		}

		castor::Point4d const & getBorderOuterUV()const noexcept
		{
			return m_borderOuterUv;
		}

		castor::Point4d const & getBorderInnerUV()const noexcept
		{
			return m_borderInnerUv;
		}
		/**@}*/
		/**
		*\~english
		*\name
		*	Mutators.
		*\~french
		*\name
		*	Mutateurs.
		*/
		/**@{*/
		void setLeftBorderSize( double size )noexcept
		{
			m_ptBorderSize[0] = size;
			m_ptBorderSize[0] = std::max( 0.0, m_ptBorderSize[0] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setTopBorderSize( double size )noexcept
		{
			m_ptBorderSize[1] = size;
			m_ptBorderSize[1] = std::max( 0.0, m_ptBorderSize[1] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setRightBorderSize( double size )noexcept
		{
			m_ptBorderSize[2] = size;
			m_ptBorderSize[2] = std::max( 0.0, m_ptBorderSize[2] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setBottomBorderSize( double size )noexcept
		{
			m_ptBorderSize[3] = size;
			m_ptBorderSize[3] = std::max( 0.0, m_ptBorderSize[3] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setBorderSize( castor::Point4d const & size )noexcept
		{
			m_ptBorderSize = size;
			m_ptBorderSize[0] = std::max( 0.0, m_ptBorderSize[0] );
			m_ptBorderSize[1] = std::max( 0.0, m_ptBorderSize[1] );
			m_ptBorderSize[2] = std::max( 0.0, m_ptBorderSize[2] );
			m_ptBorderSize[3] = std::max( 0.0, m_ptBorderSize[3] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setLeftBorderPixelSize( int size )noexcept
		{
			m_borderSize[0] = size;
			m_borderSize[0] = std::max( 0, m_borderSize[0] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setTopBorderPixelSize( int size )noexcept
		{
			m_borderSize[1] = size;
			m_borderSize[1] = std::max( 0, m_borderSize[1] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setRightBorderPixelSize( int size )noexcept
		{
			m_borderSize[2] = size;
			m_borderSize[2] = std::max( 0, m_borderSize[2] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setBottomBorderPixelSize( int size )noexcept
		{
			m_borderSize[3] = size;
			m_borderSize[3] = std::max( 0, m_borderSize[3] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setBorderPixelSize( castor::Rectangle const & size )noexcept
		{
			m_borderSize = size;
			m_borderSize[0] = std::max( 0, m_borderSize[0] );
			m_borderSize[1] = std::max( 0, m_borderSize[1] );
			m_borderSize[2] = std::max( 0, m_borderSize[2] );
			m_borderSize[3] = std::max( 0, m_borderSize[3] );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setBorderPosition( BorderPosition position )noexcept
		{
			m_borderPosition = position;
		}

		void setBorderOuterUV( castor::Point4d const & value )noexcept
		{
			m_borderOuterUv = value;
		}

		void setBorderInnerUV( castor::Point4d const & value )noexcept
		{
			m_borderInnerUv = value;
		}
		/**@}*/

	private:
		/**
		 *\copydoc		castor3d::OverlayCategory::doUpdateSize
		 */
		void doUpdateSize( OverlayRenderer const & renderer )override;
		/**
		 *\copydoc		castor3d::OverlayCategory::doUpdateScissor
		 */
		void doUpdateScissor()override;
		/**
		 *\copydoc		castor3d::OverlayCategory::doUpdate
		 */
		void doUpdate( OverlayRenderer const & renderer )override;

	private:
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
