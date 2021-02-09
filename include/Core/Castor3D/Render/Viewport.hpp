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
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Viewport();
		/**
		 *\~english
		 *\brief		Renders the viewport specifics
		 *\remarks		Applies the perspective
		 *\return		\p true if the frustum view has been modified
		 *\~french
		 *\brief		Rend le viewport
		 *\remarks		Applique la perspective
		 *\return		\p true si le frustum de vue a été modifié
		 */
		C3D_API bool update();
		/**
		 *\~english
		 *\brief		Builds a centered perspective viewport.
		 *\param[in]	fovy	Y Field of View.
		 *\param[in]	aspect	Width / Height ratio.
		 *\param[in]	near	Near clipping plane value.
		 *\param[in]	far		Far clipping plane value.
		 *\~french
		 *\brief		Construit vioewport en perspective centrée.
		 *\param[in]	fovy	Angle de vision Y.
		 *\param[in]	aspect	Ratio Largeur / Hauteur.
		 *\param[in]	near	Position du plan proche.
		 *\param[in]	far		Position du plan éloigné.
		 */
		C3D_API void setPerspective( castor::Angle const & fovy
			, float aspect
			, float nearZ
			, float farZ );
		/**
		 *\~english
		 *\brief		Builds a matrix that Sets a non centered perspective projection from the given parameters.
		 *\param[in]	left	Left clipping plane value.
		 *\param[in]	right	Right clipping plane value.
		 *\param[in]	bottom	Bottom clipping plane value.
		 *\param[in]	top		Top clipping plane value.
		 *\param[in]	near	Near clipping plane value.
		 *\param[in]	far		Far clipping plane value.
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée.
		 *\param[in]	left	Position du plan gauche.
		 *\param[in]	right	Position du plan droit.
		 *\param[in]	bottom	Position du plan bas.
		 *\param[in]	top		Position du plan haut.
		 *\param[in]	near	Position du plan proche.
		 *\param[in]	far		Position du plan éloigné.
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
		 *\param[in]	near	Near clipping plane value.
		 *\param[in]	far		Far clipping plane value.
		 *\~french
		 *\brief		Construit une matrice de projection orthographique.
		 *\param[in]	left	Position du plan gauche.
		 *\param[in]	right	Position du plan droit.
		 *\param[in]	bottom	Position du plan bas.
		 *\param[in]	top		Position du plan haut.
		 *\param[in]	near	Position du plan proche.
		 *\param[in]	far		Position du plan éloigné.
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
		inline const castor::Size & getSize()const
		{
			return m_size;
		}

		inline const castor::Position & getPosition()const
		{
			return m_position;
		}

		inline ViewportType getType()const
		{
			return m_type;
		}

		inline float getRatio()const
		{
			return m_ratio;
		}

		inline float getNear()const
		{
			return m_near.value();
		}

		inline float getFar()const
		{
			return m_far.value();
		}

		inline castor::Angle const & getFovY()const
		{
			return m_fovY.value();
		}

		inline float getLeft()const
		{
			return m_left.value();
		}

		inline float getRight()const
		{
			return m_right.value();
		}

		inline float getTop()const
		{
			return m_top.value();
		}

		inline float getBottom()const
		{
			return m_bottom.value();
		}

		inline uint32_t getWidth()const
		{
			return m_size.getWidth();
		}

		inline uint32_t getHeight()const
		{
			return m_size.getHeight();
		}

		inline bool isModified()const
		{
			return m_modified;
		}

		inline castor::Matrix4x4f const & getProjection()const
		{
			return m_projection;
		}

		inline VkViewport const & getViewport()const
		{
			return m_viewport;
		}

		inline VkRect2D const & getScissor()const
		{
			return m_scissor;
		}

		inline Engine const & getEngine()const
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
		inline void setPosition( const castor::Position & value )
		{
			m_position = value;
		}

		inline void updateType( ViewportType value )
		{
			m_type = value;
		}

		inline void updateRatio( float value )
		{
			m_ratio = value;
		}

		inline void updateNear( float value )
		{
			m_near = value;
		}

		inline void updateFar( float value )
		{
			m_far = value;
		}

		inline void updateFovY( castor::Angle const & value )
		{
			m_fovY = value;
		}

		inline void updateLeft( float value )
		{
			m_left = value;
		}

		inline void updateRight( float value )
		{
			m_right = value;
		}

		inline void updateTop( float value )
		{
			m_top = value;
		}

		inline void updateBottom( float value )
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
		ViewportType m_type;
		castor::Size m_size;
		castor::Position m_position;
		castor::Matrix4x4f m_projection;
		VkViewport m_viewport;
		VkRect2D m_scissor;
	};
}

#endif
