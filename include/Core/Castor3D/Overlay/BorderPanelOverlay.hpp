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
		 *\brief		Retrieves the absolute overlay borders size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue des bordures de l'incrustation
		 *\return		La taille
		 */
		C3D_API castor::Point4ui computePixelBorderSize()const;
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

		double getRelativeLeftBorderSize()const noexcept
		{
			return m_relBorderSize->x;
		}

		double getRelativeTopBorderSize()const noexcept
		{
			return m_relBorderSize->y;
		}

		double getRelativeRightBorderSize()const noexcept
		{
			return m_relBorderSize->z;
		}

		double getRelativeBottomBorderSize()const noexcept
		{
			return m_relBorderSize->w;
		}

		castor::Point4d const & getRelativeBorderSize()const noexcept
		{
			return m_relBorderSize;
		}

		MaterialRPtr getBorderMaterial()const noexcept
		{
			return m_borderMaterial;
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
		void setRelativeLeftBorderSize( double size )noexcept
		{
			m_relBorderSize->x = std::max( 0.0, size );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setRelativeTopBorderSize( double size )noexcept
		{
			m_relBorderSize->y = std::max( 0.0, size );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setRelativeRightBorderSize( double size )noexcept
		{
			m_relBorderSize->z = std::max( 0.0, size );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setRelativeBottomBorderSize( double size )noexcept
		{
			m_relBorderSize->w = std::max( 0.0, size );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setRelativeBorderSize( castor::Point4d const & size )noexcept
		{
			m_relBorderSize->x = std::max( 0.0, size->x );
			m_relBorderSize->y = std::max( 0.0, size->y );
			m_relBorderSize->z = std::max( 0.0, size->z );
			m_relBorderSize->w = std::max( 0.0, size->w );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setAbsoluteBorderSize( castor::Point4ui const & size )noexcept
		{
			m_pxBorderSize = size;
			( *m_pxBorderSize )->x = std::max( 0u, ( *m_pxBorderSize )->x );
			( *m_pxBorderSize )->y = std::max( 0u, ( *m_pxBorderSize )->y );
			( *m_pxBorderSize )->z = std::max( 0u, ( *m_pxBorderSize )->z );
			( *m_pxBorderSize )->w = std::max( 0u, ( *m_pxBorderSize )->w );
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setBorderPosition( BorderPosition position )noexcept
		{
			m_borderPosition = position;
			m_sizeChanged = true;
			m_borderChanged = true;
		}

		void setBorderOuterUV( castor::Point4d const & value )noexcept
		{
			m_borderOuterUv = value;
			m_borderChanged = true;
		}

		void setBorderInnerUV( castor::Point4d const & value )noexcept
		{
			m_borderInnerUv = value;
			m_borderChanged = true;
		}
		/**@}*/

	private:
		/**
		 *\copydoc		castor3d::OverlayCategory::doUpdateSize
		 */
		void doUpdateSize( OverlayRenderer const & renderer )override;
		/**
		 *\copydoc		castor3d::OverlayCategory::doUpdateSize
		 */
		void doUpdateClientArea( castor::Point4d & clientArea )const override;
		/**
		 *\copydoc		castor3d::OverlayCategory::doReset
		 */
		void doReset()override;

	private:
		//!\~english	The border material.
		//!\~french		Le matériau des bords.
		MaterialRPtr m_borderMaterial;
		//!\~english	The border size, relative to parent dimensions.
		//!\~french		La taille des bords, relative aux dimensions du parent.
		castor::Point4d m_relBorderSize;
		//!\~english	The absolute border size, in pixels.
		//!\~french		La taille absolue des bords, en pixels.
		std::optional< castor::Point4ui > m_pxBorderSize;
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
