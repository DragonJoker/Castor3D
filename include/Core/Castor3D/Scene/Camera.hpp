/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Camera_H___
#define ___C3D_Camera_H___

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Render/Frustum.hpp"
#include "Castor3D/Render/ToneMapping/ColourGradingConfig.hpp"
#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Scene/MovableObject.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	class Camera
		: public MovableObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name		The camera name.
		 *\param[in]	scene		The parent scene.
		 *\param[in]	node		The parent scene node.
		 *\param[in]	viewport	Viewport to copy.
		 *\param[in]	ownProjMtx	Tells if the projection matrix is held by the viewport \p false, or not \p true.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name		Le nom de la caméra.
		 *\param[in]	scene		La scène parente.
		 *\param[in]	node		Le noeud de scène parent.
		 *\param[in]	viewport	Viewport à copier.
		 *\param[in]	ownProjMtx	Dit si la matrice de projection est définie par le viewport \p false, ou pas \p true.
		 */
		C3D_API Camera( castor::String const & name
			, Scene & scene
			, SceneNode & node
			, Viewport viewport
			, bool ownProjMtx = false );
		/**
		 *\~english
		 *\brief		Constructor, needs the camera renderer, the name, window size and projection type. Creates a viewport renderer and a viewport.
		 *\param[in]	name		The camera name.
		 *\param[in]	scene		The parent scene.
		 *\param[in]	node		The parent scene node.
		 *\param[in]	ownProjMtx	Tells if the projection matrix is held by the viewport \p false, or not \p true.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name		Le nom de la caméra.
		 *\param[in]	scene		La scène parente.
		 *\param[in]	node		SceneNode parent.
		 *\param[in]	ownProjMtx	Dit si la matrice de projection est définie par le viewport \p false, ou pas \p true.
		 */
		C3D_API Camera( castor::String const & name
			, Scene & scene
			, SceneNode & node
			, bool ownProjMtx = false );
		/**
		 *\~english
		 *\brief		Updates the frustum.
		 *\~french
		 *\brief		Met à jour le frustum.
		 */
		C3D_API void updateFrustum();
		/**
		 *\~english
		 *\brief		Updates the viewport, the view matrix, and the frustum.
		 *\~french
		 *\brief		Met à jour le viewport, la matrice de vue et le frustum.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Sets the projection matrix, updates the viewport and the frustum.
		 *\param[in]	projection	The projection matrix.
		 *\~french
		 *\brief		Définit la matrice de projection, met à jour le viewport et le frustum.
		 *\param[in]	projection	La matrice de projection.
		 */
		C3D_API void setProjection( castor::Matrix4x4f const & projection );
		/**
		 *\~english
		 *\brief		Checks if a submesh is visible, through a geometry.
		 *\param[in]	geometry	The geometry.
		 *\param[in]	submesh		The submesh.
		 *\return		\p false if the submesh is not visible.
		 *\~french
		 *\brief
		 *\brief		Vérifie si un sous-maillage est visible, via une géométrie.
		 *\param[in]	geometry	La géométrie.
		 *\param[in]	submesh		Le sous-maillage.
		 *\return		\p false si le sous-maillage n'est pas visible.
		 */
		C3D_API bool isVisible( Geometry const & geometry, Submesh const & submesh )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API castor::Matrix4x4f getRescaledProjection( float scale
			, bool safeBanded )const;

		Viewport const & getViewport()const
		{
			return m_viewport;
		}

		Viewport & getViewport()
		{
			return m_viewport;
		}

		castor::Matrix4x4f const & getView()const
		{
			return m_view;
		}

		castor::Matrix4x4f const & getProjection( bool safeBanded )const
		{
			return m_ownProjection
				? m_projection
				: ( safeBanded
					? m_viewport.getSafeBandedProjection()
					: m_viewport.getProjection() );
		}

		ViewportType getViewportType()const
		{
			return m_viewport.getType();
		}

		castor::Size const & getSize()const
		{
			return m_viewport.getSize();
		}

		uint32_t getWidth()const
		{
			return m_viewport.getWidth();
		}

		uint32_t getHeight()const
		{
			return m_viewport.getHeight();
		}

		float getNear()const
		{
			return m_viewport.getNear();
		}

		float getFar()const
		{
			return m_viewport.getFar();
		}

		float getRatio()const
		{
			return m_viewport.getRatio();
		}

		castor::Angle const & getFovY()const
		{
			return m_viewport.getFovY();
		}

		float getProjectionScale()const
		{
			return m_viewport.getProjectionScale();
		}

		bool isVisible( castor::BoundingBox const & box
			, castor::Matrix4x4f const & transformations )const
		{
			return m_frustum.isVisible( box
				, transformations );
		}

		bool isVisible( castor::BoundingSphere const & sphere
			, castor::Matrix4x4f const & transformations
			, castor::Point3f const & scale )const
		{
			return m_frustum.isVisible( sphere
				, transformations
				, scale );
		}

		bool isVisible( castor::Point3f const & point )const
		{
			return m_frustum.isVisible( point );
		}

		Frustum const & getFrustum()const
		{
			return m_frustum;
		}

		HdrConfig const & getHdrConfig()const
		{
			return m_hdrConfig;
		}

		HdrConfig & getHdrConfig()
		{
			return m_hdrConfig;
		}

		ColourGradingConfig const & getColourGradingConfig()const noexcept
		{
			return m_colourGradingConfig;
		}

		ColourGradingConfig & getColourGradingConfig()noexcept
		{
			return m_colourGradingConfig;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void setView( castor::Matrix4x4f const & view )
		{
			m_view = view;
			markDirty();
		}

		void resize( uint32_t width, uint32_t height )
		{
			resize( castor::Size( width, height ) );
		}

		void resize( castor::Size const & size )
		{
			if ( m_viewport.getSize() != size )
			{
				m_viewport.resize( size );
				markDirty();
			}
		}

		void setViewportType( ViewportType value )
		{
			m_viewport.updateType( value );
			markDirty();
		}

		void setHdrConfig( HdrConfig value )
		{
			m_hdrConfig = castor::move( value );
		}

		void setColourGradingConfig( ColourGradingConfig config )noexcept
		{
			m_colourGradingConfig = castor::move( config );
		}
		/**@}*/

	public:
		//!\~english	The signal raised when the camera has changed.
		//!\~french		Le signal levé lorsque la caméra a changé.
		mutable OnCameraChanged onGpuChanged;

	private:
		friend class Scene;
		Viewport m_viewport;
		Frustum m_frustum;
		castor::Matrix4x4f m_view;
		HdrConfig m_hdrConfig;
		ColourGradingConfig m_colourGradingConfig;
		bool m_ownProjection{ false };
		castor::Matrix4x4f m_projection;
	};

	struct CameraContext
		: public MovableContext
	{
		ViewportUPtr viewport{};
		VkPrimitiveTopology primitiveType{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
		HdrConfig hdrConfig{};
		ColourGradingConfig colourGradingConfig{};
	};
}

namespace castor
{
	template<>
	struct ParserEnumTraits< castor3d::ViewportType >
	{
		static inline xchar const * const Name = cuT( "ViewportType" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::ViewportType >();
				return result;
			}( );
	};
}

#endif
