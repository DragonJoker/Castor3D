/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VIEWPORT_H___
#define ___C3D_VIEWPORT_H___

#include "RenderModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/PlaneEquation.hpp>

namespace castor3d
{
	class Viewport
	{
	public:
		C3D_API static const std::array< castor::String, size_t( ViewportType::eCount ) > TypeName;

	private:
		C3D_API Viewport( Engine const & engine
			, ViewportType type
			, castor::Angle const & fovy
			, float aspect
			, float left
			, float right
			, float bottom
			, float top
			, float nearZ
			, float farZ );

	public:
		C3D_API Viewport & operator=( Viewport const & rhs ) = delete;
		C3D_API Viewport & operator=( Viewport && rhs )noexcept = delete;
		C3D_API Viewport( Viewport const & rhs );
		C3D_API Viewport( Viewport && rhs )noexcept;
		C3D_API ~Viewport()noexcept = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit Viewport( Engine const & engine );
		/**
		 *\~english
		 *\brief		Applies the perspective
		 *\return		\p true if the frustum view has been modified
		 *\~french
		 *\brief		Applique la perspective
		 *\return		\p true si le frustum de vue a été modifié
		 */
		C3D_API bool update();
		/**
		 *\~english
		 *\brief		Builds a centered perspective viewport.
		 *\param[in]	fovy	Y Field of View.
		 *\param[in]	aspect	Width / Height ratio.
		 *\param[in]	nearZ	Near clipping plane value.
		 *\param[in]	farZ	Far clipping plane value.
		 *\~french
		 *\brief		Construit vioewport en perspective centrée.
		 *\param[in]	fovy	Angle de vision Y.
		 *\param[in]	aspect	Ratio Largeur / Hauteur.
		 *\param[in]	nearZ	Position du plan proche.
		 *\param[in]	farZ	Position du plan éloigné.
		 */
		C3D_API void setPerspective( castor::Angle const & fovy
			, float aspect
			, float nearZ
			, float farZ );
		/**
		 *\~english
		 *\brief		Builds a centered perspective viewport with infinite far plane.
		 *\param[in]	fovy	Y Field of View.
		 *\param[in]	aspect	Width / Height ratio.
		 *\param[in]	nearZ	Near clipping plane value.
		 *\~french
		 *\brief		Construit vioewport en perspective centrée avec plan éloigné infini.
		 *\param[in]	fovy	Angle de vision Y.
		 *\param[in]	aspect	Ratio Largeur / Hauteur.
		 *\param[in]	nearZ	Position du plan proche.
		 */
		C3D_API void setInfinitePerspective( castor::Angle const & fovy
			, float aspect
			, float nearZ );
		/**
		 *\~english
		 *\brief		Builds a matrix that Sets a non centered perspective projection from the given parameters.
		 *\param[in]	left	Left clipping plane value.
		 *\param[in]	right	Right clipping plane value.
		 *\param[in]	bottom	Bottom clipping plane value.
		 *\param[in]	top		Top clipping plane value.
		 *\param[in]	nearZ	Near clipping plane value.
		 *\param[in]	farZ	Far clipping plane value.
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée.
		 *\param[in]	left	Position du plan gauche.
		 *\param[in]	right	Position du plan droit.
		 *\param[in]	bottom	Position du plan bas.
		 *\param[in]	top		Position du plan haut.
		 *\param[in]	nearZ	Position du plan proche.
		 *\param[in]	farZ	Position du plan éloigné.
		 */
		C3D_API void setFrustum( float left
			, float right
			, float bottom
			, float top
			, float nearZ
			, float farZ );
		/**
		 *\~english
		 *\brief		Builds a matrix that Sets an orthogonal projection.
		 *\param[in]	left	Left clipping plane value.
		 *\param[in]	right	Right clipping plane value.
		 *\param[in]	bottom	Bottom clipping plane value.
		 *\param[in]	top		Top clipping plane value.
		 *\param[in]	nearZ	Near clipping plane value.
		 *\param[in]	farZ	Far clipping plane value.
		 *\~french
		 *\brief		Construit une matrice de projection orthographique.
		 *\param[in]	left	Position du plan gauche.
		 *\param[in]	right	Position du plan droit.
		 *\param[in]	bottom	Position du plan bas.
		 *\param[in]	top		Position du plan haut.
		 *\param[in]	nearZ	Position du plan proche.
		 *\param[in]	farZ	Position du plan éloigné.
		 */
		C3D_API void setOrtho( float left
			, float right
			, float bottom
			, float top
			, float nearZ
			, float farZ );
		/**
		 *\~english
		 *\brief		Sets the viewport render size
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit les dimensions de rendu du viewport
		 *\param[in]	value	La nouvelle valeur
		 */
		C3D_API void resize( const castor::Size & value );
		/**
		 *\~english
		 *\return		The number of pixels per meter at z = -1.
		 *\~french
		 *\return		Le nombre de pixels par mètre, pour z = -1.
		 */
		C3D_API float getProjectionScale()const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API castor::Matrix4x4f getRescaledProjection( float scale )const;
		C3D_API castor::Matrix4x4f getRescaledSafeBandedProjection( float scale )const;

		castor::Size const & getSize()const noexcept
		{
			return m_size.value();
		}

		castor::Position const & getPosition()const noexcept
		{
			return m_position.value();
		}

		ViewportType getType()const noexcept
		{
			return m_type;
		}

		float getRatio()const noexcept
		{
			return m_ratio;
		}

		float getNear()const noexcept
		{
			return m_near.value();
		}

		float getFar()const noexcept
		{
			return m_far.value();
		}

		castor::Angle const & getFovY()const noexcept
		{
			return m_fovY.value();
		}

		float getLeft()const noexcept
		{
			return m_left.value();
		}

		float getRight()const noexcept
		{
			return m_right.value();
		}

		float getTop()const noexcept
		{
			return m_top.value();
		}

		float getBottom()const noexcept
		{
			return m_bottom.value();
		}

		uint32_t getWidth()const noexcept
		{
			return m_size.value().getWidth();
		}

		uint32_t getHeight()const noexcept
		{
			return m_size.value().getHeight();
		}

		bool isModified()const noexcept
		{
			return m_modified;
		}

		castor::Matrix4x4f const & getProjection()const noexcept
		{
			return m_projection;
		}

		castor::Matrix4x4f const & getSafeBandedProjection()const noexcept
		{
			return m_safeBandedProjection;
		}

		VkViewport const & getViewport()const noexcept
		{
			return m_viewport;
		}

		VkRect2D const & getScissor()const noexcept
		{
			return m_scissor;
		}

		Engine const & getEngine()const noexcept
		{
			return m_engine;
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
		void setPosition( castor::Position value )noexcept
		{
			m_position = std::move( value );
		}

		void updateType( ViewportType value )noexcept
		{
			m_type = value;
		}

		void updateRatio( float value )noexcept
		{
			m_ratio = value;
		}

		void updateNear( float value )noexcept
		{
			m_near = value;
		}

		void updateFar( float value )noexcept
		{
			m_far = value;
		}

		void updateFovY( castor::Angle value )noexcept
		{
			m_fovY = std::move( value );
		}

		void updateLeft( float value )noexcept
		{
			m_left = value;
		}

		void updateRight( float value )noexcept
		{
			m_right = value;
		}

		void updateTop( float value )noexcept
		{
			m_top = value;
		}

		void updateBottom( float value )noexcept
		{
			m_bottom = value;
		}
		/**@}*/

	private:
		Engine const & m_engine;
		bool m_modified{ true };
		castor::GroupChangeTracked< float > m_left;
		castor::GroupChangeTracked< float > m_right;
		castor::GroupChangeTracked< float > m_top;
		castor::GroupChangeTracked< float > m_bottom;
		castor::GroupChangeTracked< float > m_far;
		castor::GroupChangeTracked< float > m_near;
		castor::GroupChangeTracked< castor::Angle > m_fovY;
		castor::GroupChangeTracked< float > m_ratio;
		castor::GroupChangeTracked< ViewportType > m_type;
		castor::GroupChangeTracked< castor::Size > m_size;
		castor::GroupChangeTracked< castor::Position > m_position;
		VkViewport m_viewport;
		VkRect2D m_scissor;
		castor::Matrix4x4f m_projection;
		castor::Matrix4x4f m_safeBandedProjection;
	};
}

#endif
