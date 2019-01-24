/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VIEWPORT_H___
#define ___C3D_VIEWPORT_H___

#include "Castor3DPrerequisites.hpp"
#include "Design/GroupChangeTracked.hpp"

#include <Pipeline/Scissor.hpp>
#include <Pipeline/Viewport.hpp>

#include <Math/Angle.hpp>
#include <Math/PlaneEquation.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Position.hpp>
#include <Graphics/Size.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class to represent a viewport
	\remark		Gives its projection type, FOV, ...
	\~french
	\brief		Classe de représentation de viewport
	\remark		donne le type de projection FOV, ...
	*/
	class Viewport
	{
	public:
		C3D_API static const std::array< castor::String, size_t( ViewportType::eCount ) > string_type;

		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Viewport loader
		\~french
		\brief		Loader de Viewport
		*/
		class TextWriter
			: public castor::TextWriter< Viewport >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a Viewport into a text file
			 *\param[in]	p_file		The file to save the cameras in
			 *\param[in]	p_viewport	The Viewport to save
			 *\~french
			 *\brief		Ecrit un Viewport dans un fichier texte
			 *\param[in]	p_file		Le fichier
			 *\param[in]	p_viewport	Le Viewport
			 */
			C3D_API bool operator()( castor3d::Viewport const & p_viewport, castor::TextFile & p_file )override;
		};

	private:
		C3D_API Viewport( Engine const & engine
			, ViewportType type
			, castor::Angle const & fovy
			, real aspect
			, real left
			, real right
			, real bottom
			, real top
			, real near
			, real far );

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
			, real aspect
			, real near
			, real far );
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
		C3D_API void setFrustum( real left
			, real right
			, real bottom
			, real top
			, real near
			, real far );
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
		C3D_API void setOrtho( real left
			, real right
			, real bottom
			, real top
			, real near
			, real far );
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

		inline real getRatio()const
		{
			return m_ratio;
		}

		inline real getNear()const
		{
			return m_near.value();
		}

		inline real getFar()const
		{
			return m_far.value();
		}

		inline castor::Angle const & getFovY()const
		{
			return m_fovY.value();
		}

		inline real getLeft()const
		{
			return m_left.value();
		}

		inline real getRight()const
		{
			return m_right.value();
		}

		inline real getTop()const
		{
			return m_top.value();
		}

		inline real getBottom()const
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

		inline castor::Matrix4x4r const & getProjection()const
		{
			return m_projection;
		}

		inline ashes::Viewport const & getViewport()const
		{
			return m_viewport;
		}

		inline ashes::Scissor const & getScissor()const
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

		inline void updateRatio( real value )
		{
			m_ratio = value;
		}

		inline void updateNear( real value )
		{
			m_near = value;
		}

		inline void updateFar( real value )
		{
			m_far = value;
		}

		inline void updateFovY( castor::Angle const & value )
		{
			m_fovY = value;
		}

		inline void updateLeft( real value )
		{
			m_left = value;
		}

		inline void updateRight( real value )
		{
			m_right = value;
		}

		inline void updateTop( real value )
		{
			m_top = value;
		}

		inline void updateBottom( real value )
		{
			m_bottom = value;
		}
		/**@}*/

	private:
		Engine const & m_engine;
		bool m_modified{ true };
		castor::GroupChangeTracked< real > m_left;
		castor::GroupChangeTracked< real > m_right;
		castor::GroupChangeTracked< real > m_top;
		castor::GroupChangeTracked< real > m_bottom;
		castor::GroupChangeTracked< real > m_far;
		castor::GroupChangeTracked< real > m_near;
		castor::GroupChangeTracked< castor::Angle > m_fovY;
		castor::GroupChangeTracked< real > m_ratio;
		ViewportType m_type;
		castor::Size m_size;
		castor::Position m_position;
		castor::Matrix4x4r m_projection;
		ashes::Viewport m_viewport;
		ashes::Scissor m_scissor;
	};
}

#endif
