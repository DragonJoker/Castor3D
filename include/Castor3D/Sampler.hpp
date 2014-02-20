/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Sampler___
#define ___C3D_Sampler___

#include "Prerequisites.hpp"

#include "Renderable.hpp"
#include "Renderer.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/03/2013
	\version	0.7.0
	\~english
	\brief		Sampler renderer
	\~french
	\brief		Rendereur de sampler
	*/
	class C3D_API SamplerRenderer : public Renderer< Sampler, SamplerRenderer >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param		p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param		p_pRenderSystem	Le render system
		 */
		SamplerRenderer( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~SamplerRenderer();
		/**
		 *\~english
		 *\brief		Initialises the sampler
		 *\return		\p true if ok
		 *\~french
		 *\brief		Initialise le sampler
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Initialise()=0;
		/**
		 *\~english
		 *\brief		Cleanups the sampler
		 *\~french
		 *\brief		Nettoie le sampler
		 */
		virtual void Cleanup()=0;
		/**
		 *\~english
		 *\brief		Applies the sampler
		 *\param[in]	p_eDimension	The texture dimension
		 *\param[in]	p_uiIndex		The sampler index
		 *\return		\p true if ok
		 *\~french
		 *\brief		Applique le sampler
		 *\param[in]	p_eDimension	La dimension de la texture
		 *\param[in]	p_uiIndex		L'index du sampler
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Bind( eTEXTURE_DIMENSION p_eDimension, uint32_t p_uiIndex )=0;
		/**
		 *\~english
		 *\brief		Removes the sampler
		 *\~french
		 *\brief		Enlève le sampler
		 */
		virtual void Unbind()=0;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/03/2013
	\version	0.7.0
	\~english
	\brief		Defines a sampler for a texture
	\~french
	\brief		Définit un sampler pour une texture
	*/
	class C3D_API Sampler : public Renderable< Sampler, SamplerRenderer >
	{
	private:
		//!\~english	Sampler interpolation modes				\~french	Modes d'interpolation du sampler
		eINTERPOLATION_MODE m_eInterpolationModes[eINTERPOLATION_FILTER_COUNT];
		//!\~english	Sampler wrapping modes					\~french	Modes de wrapping du sampler
		eWRAP_MODE m_eWrapModes[eTEXTURE_UVW_COUNT];
		//!\~english	Minimal LOD Level						\~french	Niveau de LOD minimal
		real m_rMinLod;
		//!\~english	Maximal LOD Level						\~french	Niveau de LOD maximal
		real m_rMaxLod;
		//!\~english	The texture LOD offset					\~french	Le décalage de Lod de la texture
		real m_rLodBias;
		//!\~english	Texture border colour					\~french	Couleur des bords de la texture
		Castor::Colour m_clrBorderColour;
		//!\~english	Maximal anisotropic filtering value		\~french	Valeur maximale pour le filtrage anisotropique
		real m_rMaxAnisotropy;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine	Le moteur
		 */
		Sampler( Engine * p_pEngine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Sampler();
		/**
		 *\~english
		 *\brief		Initialises the sampler
		 *\return		\p true if ok
		 *\~french
		 *\brief		Initialise le sampler
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Initialise();
		/**
		 *\~english
		 *\brief		Cleanups the sampler
		 *\~french
		 *\brief		Nettoie le sampler
		 */
		virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Applies the sampler
		 *\param[in]	p_eDimension	The texture dimension
		 *\param[in]	p_uiIndex		The sampler index
		 *\return		\p true if ok
		 *\~french
		 *\brief		Applique le sampler
		 *\param[in]	p_eDimension	La dimension de la texture
		 *\param[in]	p_uiIndex		L'index du sampler
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Bind( eTEXTURE_DIMENSION p_eDimension, uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Removes the sampler
		 *\~french
		 *\brief		Enlève le sampler
		 */
		virtual void Unbind();
		/**
		 *\~english
		 *\brief		Retrieves the interpolation mode for given filter
		 *\param[in]	p_eFilter	The concerned filter
		 *\return		The interpolation mode
		 *\~french
		 *\brief		Récupère le mode d'interpolation pour le filtre donné
		 *\param[in]	p_eFilter	Le filtre concerné
		 *\return		Le mode d'interpolation
		 */
		 inline eINTERPOLATION_MODE GetInterpolationMode( eINTERPOLATION_FILTER p_eFilter )const { return m_eInterpolationModes[p_eFilter]; }
		/**
		 *\~english
		 *\brief		Defines the interpolation mode for given filter
		 *\param[in]	p_eFilter	The concerned filter
		 *\param[in]	p_eMode		The wanted mode
		 *\~french
		 *\brief		Définit le mode d'interpolation pour le filtre donné
		 *\param[in]	p_eFilter	Le filtre concerné
		 *\param[in]	p_eMode		Le mode souhaité
		 */
		 inline void SetInterpolationMode( eINTERPOLATION_FILTER p_eFilter, eINTERPOLATION_MODE p_eMode ) { m_eInterpolationModes[p_eFilter] = p_eMode; }
		/**
		 *\~english
		 *\brief		Retrieves the wrapping mode for given dimension
		 *\param[in]	p_eUVW	The dimension
		 *\return		The wrapping mode
		 *\~french
		 *\brief		Récupère le mode de wrap pour la dimension donnée
		 *\param[in]	p_eUVW	La dimension
		 *\return		Le mode de wrap
		 */
		 inline eWRAP_MODE GetWrappingMode( eTEXTURE_UVW p_eUVW )const { return m_eWrapModes[p_eUVW]; }
		/**
		 *\~english
		 *\brief		Defines the wrapping mode for given dimension
		 *\param[in]	p_eUVW	The dimension
		 *\param[in]	p_eMode	The wrapping mode
		 *\~french
		 *\brief		Définit le mode de wrap pour la dimension donnée
		 *\param[in]	p_eUVW	La dimension
		 *\param[in]	p_eMode	Le mode de wrap
		 */
		 inline void SetWrappingMode( eTEXTURE_UVW p_eUVW, eWRAP_MODE p_eMode ) { m_eWrapModes[p_eUVW] = p_eMode; }
		/**
		 *\~english
		 *\brief		Retrieves the maximal anisotropy filtering value
		 \return		The value
		 *\~french
		 *\brief		Récupère la valeur maximale de filtrage anisotropique
		 *\return		La valeur
		 */
		 inline real GetMaxAnisotropy()const { return m_rMaxAnisotropy; }
		/**
		 *\~english
		 *\brief		Defines the maximal anisotropy filtering value
		 *\param[in]	p_rValue	The new value
		 *\~french
		 *\brief		Définit la valeur maximale de filtrage anisotropique
		 *\param[in]	p_rValue	La nouvelle valeur
		 */
		 inline void SetMaxAnisotropy( real p_rValue ) { m_rMaxAnisotropy = p_rValue; }
		/**
		 *\~english
		 *\brief		Retrieves the minimal LOD level
		 \return		The level
		 *\~french
		 *\brief		Récupère le niveau minimal pour le LOD
		 *\return		Le niveau
		 */
		 inline real GetMinLod()const { return m_rMinLod; }
		/**
		 *\~english
		 *\brief		Defines the minimal LOD level
		 *\param[in]	p_rLod	The new value
		 *\~french
		 *\brief		Définit le niveau minimal pour le LOD
		 *\param[in]	p_rLod	La nouvelle valeur
		 */
		 inline void SetMinLod( real p_rLod ) { m_rMinLod = p_rLod; }
		/**
		 *\~english
		 *\brief		Retrieves the maximal LOD level
		 \return		The level
		 *\~french
		 *\brief		Récupère le niveau maximal pour le LOD
		 *\return		Le niveau
		 */
		 inline real GetMaxLod()const { return m_rMaxLod; }
		/**
		 *\~english
		 *\brief		Defines the maximal LOD level
		 *\param[in]	p_rLod	The new value
		 *\~french
		 *\brief		Définit le niveau maximal pour le LOD
		 *\param[in]	p_rLod	La nouvelle valeur
		 */
		 inline void SetMaxLod( real p_rLod ) { m_rMaxLod = p_rLod; }
		/**
		 *\~english
		 *\brief		Retrieves the LOD bias
		 \return		The level
		 *\~french
		 *\brief		Récupère le LOD bias
		 *\return		Le niveau
		 */
		 inline real GetLodBias()const { return m_rLodBias; }
		/**
		 *\~english
		 *\brief		Defines the LOD bias
		 *\param[in]	p_rLod	The new value
		 *\~french
		 *\brief		Définit le LOD bias
		 *\param[in]	p_rLod	La nouvelle valeur
		 */
		 inline void SetLodBias( real p_rLod ) { m_rLodBias = p_rLod; }
		 /**
		  *\~english
		  *\brief		Retrieves the texture border colour
		  *\return		The colour
		  *\~french
		  *\brief		Récupère la couleur de bord de la texture
		  *\return		La couleur
		  */
		 inline	Castor::Colour const & GetBorderColour()const 	{ return m_clrBorderColour; }
		 /**
		  *\~english
		  *\brief		Defines the texture border colour
		  *\param[in]	p_crColour	The new value
		  *\~french
		  *\brief		Définit la couleur de bord de la texture
		  *\param[in]	p_crColour	La nouvelle valeur
		  */
		 inline	void SetBorderColour( Castor::Colour const & p_crColour ) { m_clrBorderColour = p_crColour; }
		 /**
		  *\~english
		  *\brief		Defines the texture border colour
		  *\param[in]	red, green, blue, alpha	The new values
		  *\~french
		  *\brief		Définit la couleur de bord de la texture
		  *\param[in]	red, green, blue, alpha	Les nouvelles valeurs
		  */
		 inline	void SetBorderColour( float red, float green, float blue, float alpha ) { m_clrBorderColour = Castor::Colour::from_components( red, green, blue, alpha ); }
		 /**
		  *\~english
		  *\brief		Defines the texture border colour
		  *\param[in]	rvba	The new values
		  *\~french
		  *\brief		Définit la couleur de bord de la texture
		  *\param[in]	rvba	Les nouvelles valeurs
		  */
		 inline	void SetBorderColour( float * rvba ) { m_clrBorderColour = Castor::Colour::from_components( rvba[0], rvba[1], rvba[2], rvba[3] ); }
		 /**
		  *\~english
		  *\brief		Defines the texture border colour
		  *\param[in]	rvb, alpha	The new values
		  *\~french
		  *\brief		Définit la couleur de bord de la texture
		  *\param[in]	rvb, alpha	Les nouvelles valeurs
		  */
		 inline	void SetBorderColour( float * rvb, float alpha ) { m_clrBorderColour = Castor::Colour::from_components( rvb[0], rvb[1], rvb[2], alpha ); }
	};
}

#pragma warning( pop )

#endif
