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

namespace c3d
{
	class Viewport
	{
	public:
		C3D_API static const Array< String, size_t( ViewportType::eCount ) > TypeName;

	private:
		C3D_API Viewport( Engine const & engine
			, ViewportType type
			, Angle const & fovy
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
		 *\brief			Clones this object into the given one.
		 *\param[in,out]	output	Receives this object's data.
		 *\~french
		 *\brief			Clone cet objet dans celui donné.
		 *\param[in,out]	output	Reçoit les données de cet objet.
		 */
		C3D_API void cloneInto( Viewport & output )const;
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
		C3D_API void setPerspective( Angle const & fovy
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
		C3D_API void setInfinitePerspective( Angle const & fovy
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
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API Matrix4x4f getRescaledProjection( float scale )const;
		C3D_API Matrix4x4f getRescaledSafeBandedProjection( Size const & renderSize, float scale )const;

		Position const & getPosition()const noexcept
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

		Angle const & getFovY()const noexcept
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

		bool isModified()const noexcept
		{
			return m_modified;
		}

		Matrix4x4f const & getProjection()const noexcept
		{
			return m_projection;
		}

		Matrix4x4f getSafeBandedProjection( Size const & renderSize )const
		{
			return getRescaledSafeBandedProjection( renderSize, 1.0f );
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
		void setPosition( Position const & value )noexcept
		{
			m_position = value;
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

		void updateFovY( Angle const & value )noexcept
		{
			m_fovY = value;
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
		GroupChangeTracked< float > m_left;
		GroupChangeTracked< float > m_right;
		GroupChangeTracked< float > m_top;
		GroupChangeTracked< float > m_bottom;
		GroupChangeTracked< float > m_far;
		GroupChangeTracked< float > m_near;
		GroupChangeTracked< Angle > m_fovY;
		GroupChangeTracked< float > m_ratio;
		GroupChangeTracked< ViewportType > m_type;
		GroupChangeTracked< Position > m_position;
		Matrix4x4f m_projection;
	};
}

#endif
