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
#ifndef ___C3D_VIEWPORT_H___
#define ___C3D_VIEWPORT_H___

#include "Castor3DPrerequisites.hpp"

#include <Math/Angle.hpp>
#include <Math/PlaneEquation.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Size.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		16/04/2016
	\~english
	\brief		Render API specific viewport implementation.
	\~french
	\brief		Implémentation de viewport spécifique à l'API de rendu.
	*/
	class IViewportImpl
		: public Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_viewport		The parent viewport.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	p_viewport		Le viewport parent.
		 */
		C3D_API IViewportImpl( RenderSystem & p_renderSystem, Viewport & p_viewport );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~IViewportImpl();
		/**
		 *\~english
		 *\brief		Applies the given viewport dimension.
		 *\param[in]	p_windowWidth, p_windowHeight	The dimensions.
		 *\~french
		 *\brief		Applique les dimensions de viewport données.
		 *\param[in]	p_windowWidth, p_windowHeight	Les dimensions.
		 */
		C3D_API virtual void Apply()const = 0;

	protected:
		//!\~english The parent viewport.	\~french Le viewport parent.
		Viewport & m_viewport;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class to represent a viewport
	\remark		Gives its projection type, FOV, ...
	\~french
	\brief		Classe de représentation de viewport
	\remark		Donne le type de projection FOV, ...
	*/
	class Viewport
		: public Castor::OwnedBy< Engine >
	{
	public:
		C3D_API static const std::array< Castor::String, size_t( ViewportType::eCount ) > string_type;

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
			: public Castor::TextWriter< Viewport >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
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
			C3D_API bool operator()( Castor3D::Viewport const & p_viewport, Castor::TextFile & p_file )override;
		};

	private:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The core engine.
		 *\param[in]	p_type		Projection type.
		 *\param[in]	p_fovy		Y Field of View.
		 *\param[in]	p_aspect	Width / Height ratio.
		 *\param[in]	p_left		Left clipping plane value.
		 *\param[in]	p_right		Right clipping plane value.
		 *\param[in]	p_bottom	Bottom clipping plane value.
		 *\param[in]	p_top		Top clipping plane value.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_type		Type de projection.
		 *\param[in]	p_fovy		Angle de vision Y.
		 *\param[in]	p_aspect	Ratio Largeur / Hauteur.
		 *\param[in]	p_left		Position du plan gauche.
		 *\param[in]	p_right		Position du plan droit.
		 *\param[in]	p_bottom	Position du plan bas.
		 *\param[in]	p_top		Position du plan haut.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 */
		C3D_API Viewport( Engine & p_engine, ViewportType p_type, Castor::Angle const & p_fovy, real p_aspect, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API explicit Viewport( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\param[in]	p_rhs	The object to copy.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	p_rhs	L'objet à copier.
		 */
		C3D_API Viewport( Viewport const & p_rhs );
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\param[in]	p_rhs	The object to copy.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 *\param[in]	p_rhs	L'objet à copier.
		 */
		C3D_API Viewport & operator=( Viewport const & p_rhs );
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\param[in]	p_rhs	The object to move.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 *\param[in]	p_rhs	L'objet à déplacer.
		 */
		C3D_API Viewport( Viewport && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\param[in]	p_rhs	The object to move.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 *\param[in]	p_rhs	L'objet à déplacer.
		 */
		C3D_API Viewport & operator=( Viewport && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Viewport();
		/**
		 *\~english
		 *\brief		Creates the render API specific implementation.
		 *\return		\p true if everything's ok.
		 *\~french
		 *\brief		Crée l'implémentation spécifique à l'API de rendu.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Destroys the render API specific implementation.
		 *\~french
		 *\brief		Détruit l'implémentation spécifique à l'API de rendu.
		 */
		C3D_API void Cleanup();
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
		C3D_API bool Update();
		/**
		 *\~english
		 *\brief		Applies the given viewport dimension.
		 *\param[in]	p_windowWidth, p_windowHeight	The dimensions.
		 *\~french
		 *\brief		Applique les dimensions de viewport données.
		 *\param[in]	p_windowWidth, p_windowHeight	Les dimensions.
		 */
		C3D_API void Apply()const;
		/**
		 *\~english
		 *\brief		Builds a centered perspective viewport.
		 *\param[in]	p_fovy		Y Field of View.
		 *\param[in]	p_aspect	Width / Height ratio.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\return		The viewport.
		 *\~french
		 *\brief		Construit vioewport en perspective centrée.
		 *\param[in]	p_fovy		Angle de vision Y.
		 *\param[in]	p_aspect	Ratio Largeur / Hauteur.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 *\return		Le viewport.
		 */
		C3D_API void SetPerspective( Castor::Angle const & p_fovy, real p_aspect, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered perspective projection from the given parameters.
		 *\param[in]	p_left		Left clipping plane value.
		 *\param[in]	p_right		Right clipping plane value.
		 *\param[in]	p_bottom	Bottom clipping plane value.
		 *\param[in]	p_top		Top clipping plane value.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\return		The viewport.
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée.
		 *\param[in]	p_left		Position du plan gauche.
		 *\param[in]	p_right		Position du plan droit.
		 *\param[in]	p_bottom	Position du plan bas.
		 *\param[in]	p_top		Position du plan haut.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 *\return		Le viewport.
		 */
		C3D_API void SetFrustum( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets an orthogonal projection.
		 *\param[in]	p_left		Left clipping plane value.
		 *\param[in]	p_right		Right clipping plane value.
		 *\param[in]	p_bottom	Bottom clipping plane value.
		 *\param[in]	p_top		Top clipping plane value.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\return		The viewport.
		 *\~french
		 *\brief		Construit une matrice de projection orthographique.
		 *\param[in]	p_left		Position du plan gauche.
		 *\param[in]	p_right		Position du plan droit.
		 *\param[in]	p_bottom	Position du plan bas.
		 *\param[in]	p_top		Position du plan haut.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 *\return		Le viewport.
		 */
		C3D_API void SetOrtho( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		/**
		 *\~english
		 *\return		The viewport render size
		 *\~french
		 *\return		Les dimensions de rendu du viewport
		 */
		inline const Castor::Size & GetSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The viewport projection type
		 *\~french
		 *\return		Le type de projection du viewport
		 */
		inline ViewportType GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\return		The aspect ratio
		 *\~french
		 *\return		Le ratio d'aspect
		 */
		inline real GetRatio()const
		{
			return m_ratio;
		}
		/**
		 *\~english
		 *\return		The near plane position
		 *\~french
		 *\return		La position du plan proche
		 */
		inline real GetNear()const
		{
			return m_near;
		}
		/**
		 *\~english
		 *\return		The far plane position
		 *\~french
		 *\return		La position du plan lointain
		 */
		inline real GetFar()const
		{
			return m_far;
		}
		/**
		 *\~english
		 *\return		The view vertical angle
		 *\~french
		 *\return		L'angle de vue vertical
		 */
		inline Castor::Angle GetFovY()const
		{
			return m_fovY;
		}
		/**
		 *\~english
		 *\return		The left plane position
		 *\~french
		 *\return		La position du plan gauche
		 */
		inline real GetLeft()const
		{
			return m_left;
		}
		/**
		 *\~english
		 *\return		The right plane position
		 *\~french
		 *\return		La position du plan droit
		 */
		inline real GetRight()const
		{
			return m_right;
		}
		/**
		 *\~english
		 *\return		The top plane position
		 *\~french
		 *\return		La position du plan haut
		 */
		inline real GetTop()const
		{
			return m_top;
		}
		/**
		 *\~english
		 *\return		The bottom plane position
		 *\~french
		 *\return		La position du plan bas
		 */
		inline real GetBottom()const
		{
			return m_bottom;
		}
		/**
		 *\~english
		 *\return		The viewport width
		 *\~french
		 *\return		La largeur du viewport
		 */
		inline uint32_t GetWidth()const
		{
			return m_size.width();
		}
		/**
		 *\~english
		 *\return		The viewport height
		 *\~french
		 *\return		La hauteur du viewport
		 */
		inline uint32_t GetHeight()const
		{
			return m_size.height();
		}
		/**
		 *\~english
		 *\return		The viewport modificiation status
		 *\~french
		 *\return		Le statut de modification du viewport
		 */
		inline bool IsModified()const
		{
			return m_modified;
		}
		/**
		 *\~english
		 *\return		The projection matrix
		 *\~french
		 *\return		La matrice de projection
		 */
		inline Castor::Matrix4x4r const & GetProjection()const
		{
			return m_projection;
		}
		/**
		*\~english
		*\brief		Sets the viewport render size
		*\param[in]	p_size	The new value
		*\~french
		*\brief		Définit les dimensions de rendu du viewport
		*\param[in]	p_size	La nouvelle valeur
		*/
		inline void Resize( const Castor::Size & p_size )
		{
			m_size = p_size;
		}
		/**
		 *\~english
		 *\brief		Retrieves the viewport projection type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de projection du viewport
		 *\return		La valeur
		 */
		inline void UpdateType( ViewportType p_type )
		{
			m_modified |= m_type != p_type;
			m_type = p_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves the aspect ratio
		 *\return		The value
		 *\~french
		 *\brief		Récupère le ratio d'aspect
		 *\return		La valeur
		 */
		inline void UpdateRatio( real p_rRatio )
		{
			m_modified |= m_ratio != p_rRatio;
			m_ratio = p_rRatio;
		}
		/**
		 *\~english
		 *\brief		Retrieves the near plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan proche
		 *\return		La valeur
		 */
		inline void UpdateNear( real p_rNear )
		{
			m_modified |= m_near != p_rNear;
			m_near = p_rNear;
		}
		/**
		 *\~english
		 *\brief		Retrieves the far plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan lointain
		 *\return		La valeur
		 */
		inline void UpdateFar( real p_rFar )
		{
			m_modified |= m_far != p_rFar;
			m_far = p_rFar;
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertical view angle
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'angle de vue vertical
		 *\return		La valeur
		 */
		inline void UpdateFovY( Castor::Angle const & p_aFovY )
		{
			m_modified |= m_fovY != p_aFovY;
			m_fovY = p_aFovY;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan gauche
		 *\return		La valeur
		 */
		inline void UpdateLeft( real p_rLeft )
		{
			m_modified |= m_left != p_rLeft;
			m_left = p_rLeft;
		}
		/**
		 *\~english
		 *\brief		Retrieves the right plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan droit
		 *\return		La valeur
		 */
		inline void UpdateRight( real p_rRight )
		{
			m_modified |= m_right != p_rRight;
			m_right = p_rRight;
		}
		/**
		 *\~english
		 *\brief		Retrieves the top plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan haut
		 *\return		La valeur
		 */
		inline void UpdateTop( real p_rTop )
		{
			m_modified |= m_top != p_rTop;
			m_top = p_rTop;
		}
		/**
		 *\~english
		 *\brief		Retrieves the bottom plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan bas
		 *\return		La valeur
		 */
		inline void UpdateBottom( real p_rBottom )
		{
			m_modified |= m_bottom != p_rBottom;
			m_bottom = p_rBottom;
		}

	private:
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a centered perspective projection from the given parameters
		 *\param[in]	p_fovy		Y Field of View
		 *\param[in]	p_aspect	Width / Height ratio
		 *\param[in]	p_near		Near clipping plane value
		 *\param[in]	p_far		Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection en perspective centrée
		 *\param[in]	p_fovy		Angle de vision Y
		 *\param[in]	p_aspect	Ratio Largeur / Hauteur
		 *\param[in]	p_near		Position du plan proche
		 *\param[in]	p_far		Position du plan lointain
		 */
		void DoComputePerspective( Castor::Angle const & p_fovy, real p_aspect, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered perspective projection from the given parameters
		 *\param[in]	p_left		Left clipping plane value
		 *\param[in]	p_right		Right clipping plane value
		 *\param[in]	p_bottom	Bottom clipping plane value
		 *\param[in]	p_top		Top clipping plane value
		 *\param[in]	p_near		Near clipping plane value
		 *\param[in]	p_far		Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée
		 *\param[in]	p_left		Position du plan gauche
		 *\param[in]	p_right		Position du plan droit
		 *\param[in]	p_bottom	Position du plan bas
		 *\param[in]	p_top		Position du plan haut
		 *\param[in]	p_near		Position du plan proche
		 *\param[in]	p_far		Position du plan lointain
		 */
		void DoComputeFrustum( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered orthogonal projection from the given parameters
		 *\param[in]	p_left		Left clipping plane value
		 *\param[in]	p_right		Right clipping plane value
		 *\param[in]	p_bottom	Bottom clipping plane value
		 *\param[in]	p_top		Top clipping plane value
		 *\param[in]	p_near		Near clipping plane value
		 *\param[in]	p_far		Far clipping plane value
		 *\~french
		 *\brief		Construit une matrice de projection orthographique non centrée
		 *\param[in]	p_left		Position du plan gauche
		 *\param[in]	p_right		Position du plan droit
		 *\param[in]	p_bottom	Position du plan bas
		 *\param[in]	p_top		Position du plan haut
		 *\param[in]	p_near		Position du plan proche
		 *\param[in]	p_far		Position du plan lointain
		 */
		void DoComputeOrtho( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Builds a view matrix that looks at a given point.
		 *\param[in]	p_eye		The eye position.
		 *\param[in]	p_center	The point to look at.
		 *\param[in]	p_up		The up direction..
		 *\~french
		 *\brief		Construit une matrice de vue regardant un point donné.
		 *\param[in]	p_eye		La position de l'oeil.
		 *\param[in]	p_center	Le point à regarder.
		 *\param[in]	p_up		La direction vers le haut.
		 */
		void DoComputeLookAt( Castor::Point3r const & p_eye, Castor::Point3r const & p_center, Castor::Point3r const & p_up );

	private:
		//!\~english The left viewport plane	\~french Position du plan gauche
		real m_left;
		//!\~english The right viewport plane	\~french Position du plan droit
		real m_right;
		//!\~english The top viewport plane	\~french Position du plan haut
		real m_top;
		//!\~english The bottom viewport plane	\~french Position du plan bas
		real m_bottom;
		//!\~english The viewport farthest viewable distance	\~french Position du plan lointain
		real m_far;
		//!\~english The viewport nearest viewable distance	\~french Position du plan proche
		real m_near;
		//!\~english The viewport vertical FOV 	\~french Angle de vue vezrtical
		Castor::Angle m_fovY;
		//!\~english The projection type	\~french Type de projection
		ViewportType m_type;
		//!\~english The viewport render size	\~french Dimensions du viewport
		Castor::Size m_size;
		//!\~english The display window ratio (4:3, 16:9, ...)	\~french Ratio d'affichage
		real m_ratio;
		//!\~english Tells the view frustum's planes need to be updated	\~french Dit que les plans du frustum de vue doivent être mis à jour
		bool m_modified;
		//!\~english The projection matrix.	\~french La matrice de projection.
		Castor::Matrix4x4r m_projection;
		//!\~english The render API specific implementation.	\~french L'implémentation spécifique à l'API de rendu.
		IViewportImplUPtr m_impl;
	};
}

#endif
