/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Sampler___
#define ___C3D_Sampler___

#include "Castor3DPrerequisites.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Colour.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/03/2013
	\version	0.7.0
	\~english
	\brief		Defines a sampler for a texture
	\~french
	\brief		Définit un sampler pour une texture
	*/
	class Sampler
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0
		\date		21/03/2014
		\~english
		\brief		Sampler loader
		\remark
		\~french
		\brief		Loader de Sampler
		*/
		class TextWriter
			: public castor::TextWriter< Sampler >
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
			 *\brief			Writes a sampler into a text file
			 *\param[in]		p_sampler	The sampler to save
			 *\param[in,out]	p_file		The file where to save the sampler
			 *\~french
			 *\brief			Ecrit un échantillonneur dans un fichier texte
			 *\param[in]		p_sampler	L'échantillonneur
			 *\param[in,out]	p_file		Le fichier
			 */
			C3D_API bool operator()( Sampler const & p_sampler, castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	    The sampler name
		 *\param[in]	engine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	    Le nom du sampler
		 *\param[in]	engine	Le moteur
		 */
		C3D_API Sampler( Engine & engine, castor::String const & p_name = castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Sampler();
		/**
		 *\~english
		 *\brief		Initialises the sampler
		 *\return		\p true if ok
		 *\~french
		 *\brief		Initialise le sampler
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanups the sampler
		 *\~french
		 *\brief		Nettoie le sampler
		 */
		C3D_API virtual void cleanup() = 0;
		/**
		 *\~english
		 *\brief		Applies the sampler
		 *\param[in]	p_index	The sampler index
		 *\~french
		 *\brief		Applique le sampler
		 *\param[in]	p_index	L'index du sampler
		 */
		C3D_API virtual void bind( uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Removes the sampler
		 *\param[in]	p_index	The sampler index
		 *\~french
		 *\brief		Enlève le sampler
		 *\param[in]	p_index	L'index du sampler
		 */
		C3D_API virtual void unbind( uint32_t p_index )const = 0;
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
		inline InterpolationMode getInterpolationMode( InterpolationFilter p_eFilter )const
		{
			return m_eInterpolationModes[uint32_t( p_eFilter )];
		}
		/**
		 *\~english
		 *\brief		Defines the interpolation mode for given filter
		 *\param[in]	p_eFilter	The concerned filter
		 *\param[in]	p_mode		The wanted mode
		 *\~french
		 *\brief		Définit le mode d'interpolation pour le filtre donné
		 *\param[in]	p_eFilter	Le filtre concerné
		 *\param[in]	p_mode		Le mode souhaité
		 */
		inline void setInterpolationMode( InterpolationFilter p_eFilter, InterpolationMode p_mode )
		{
			m_eInterpolationModes[uint32_t( p_eFilter )] = p_mode;
		}
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
		inline WrapMode getWrappingMode( TextureUVW p_eUVW )const
		{
			return m_eWrapModes[uint32_t( p_eUVW )];
		}
		/**
		 *\~english
		 *\brief		Defines the wrapping mode for given dimension
		 *\param[in]	p_eUVW	The dimension
		 *\param[in]	p_mode	The wrapping mode
		 *\~french
		 *\brief		Définit le mode de wrap pour la dimension donnée
		 *\param[in]	p_eUVW	La dimension
		 *\param[in]	p_mode	Le mode de wrap
		 */
		inline void setWrappingMode( TextureUVW p_eUVW, WrapMode p_mode )
		{
			m_eWrapModes[uint32_t( p_eUVW )] = p_mode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the maximal anisotropy filtering value
		 \return		The value
		 *\~french
		 *\brief		Récupère la valeur maximale de filtrage anisotropique
		 *\return		La valeur
		 */
		inline real getMaxAnisotropy()const
		{
			return m_rMaxAnisotropy;
		}
		/**
		 *\~english
		 *\brief		Defines the maximal anisotropy filtering value
		 *\param[in]	p_rValue	The new value
		 *\~french
		 *\brief		Définit la valeur maximale de filtrage anisotropique
		 *\param[in]	p_rValue	La nouvelle valeur
		 */
		inline void setMaxAnisotropy( real p_rValue )
		{
			m_rMaxAnisotropy = p_rValue;
		}
		/**
		 *\~english
		 *\brief		Retrieves the minimal LOD level
		 \return		The level
		 *\~french
		 *\brief		Récupère le niveau minimal pour le LOD
		 *\return		Le niveau
		 */
		inline real getMinLod()const
		{
			return m_rMinLod;
		}
		/**
		 *\~english
		 *\brief		Defines the minimal LOD level
		 *\param[in]	p_rLod	The new value
		 *\~french
		 *\brief		Définit le niveau minimal pour le LOD
		 *\param[in]	p_rLod	La nouvelle valeur
		 */
		inline void setMinLod( real p_rLod )
		{
			m_rMinLod = p_rLod;
		}
		/**
		 *\~english
		 *\brief		Retrieves the maximal LOD level
		 \return		The level
		 *\~french
		 *\brief		Récupère le niveau maximal pour le LOD
		 *\return		Le niveau
		 */
		inline real getMaxLod()const
		{
			return m_rMaxLod;
		}
		/**
		 *\~english
		 *\brief		Defines the maximal LOD level
		 *\param[in]	p_rLod	The new value
		 *\~french
		 *\brief		Définit le niveau maximal pour le LOD
		 *\param[in]	p_rLod	La nouvelle valeur
		 */
		inline void setMaxLod( real p_rLod )
		{
			m_rMaxLod = p_rLod;
		}
		/**
		 *\~english
		 *\brief		Retrieves the LOD bias
		 \return		The level
		 *\~french
		 *\brief		Récupère le LOD bias
		 *\return		Le niveau
		 */
		inline real getLodBias()const
		{
			return m_rLodBias;
		}
		/**
		 *\~english
		 *\brief		Defines the LOD bias
		 *\param[in]	p_rLod	The new value
		 *\~french
		 *\brief		Définit le LOD bias
		 *\param[in]	p_rLod	La nouvelle valeur
		 */
		inline void setLodBias( real p_rLod )
		{
			m_rLodBias = p_rLod;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture border colour
		 *\return		The colour
		 *\~french
		 *\brief		Récupère la couleur de bord de la texture
		 *\return		La couleur
		 */
		inline	castor::Colour const & getBorderColour()const
		{
			return m_clrBorderColour;
		}
		/**
		 *\~english
		 *\brief		Defines the texture border colour
		 *\param[in]	p_crColour	The new value
		 *\~french
		 *\brief		Définit la couleur de bord de la texture
		 *\param[in]	p_crColour	La nouvelle valeur
		 */
		inline	void setBorderColour( castor::Colour const & p_crColour )
		{
			m_clrBorderColour = p_crColour;
		}
		/**
		 *\~english
		 *\return		The comparison mode.
		 *\~french
		 *\return		Le mode de comparaison.
		 */
		inline ComparisonMode getComparisonMode()const
		{
			return m_comparisonMode;
		}
		/**
		 *\~english
		 *\brief		Defines the comparison mode.
		 *\param[in]	p_mode		The wanted mode.
		 *\~french
		 *\brief		Définit le mode de comparaison.
		 *\param[in]	p_mode		Le mode souhaité.
		 */
		inline void setComparisonMode( ComparisonMode p_mode )
		{
			m_comparisonMode = p_mode;
		}
		/**
		 *\~english
		 *\return		The comparison function.
		 *\~french
		 *\return		La fonction de comparaison.
		 */
		inline ComparisonFunc getComparisonFunc()const
		{
			return m_comparisonFunc;
		}
		/**
		 *\~english
		 *\brief		Defines the comparison function.
		 *\param[in]	p_mode		The wanted function.
		 *\~french
		 *\brief		Définit la fonction de comparaison.
		 *\param[in]	p_mode		La fonction souhaitée.
		 */
		inline void setComparisonFunc( ComparisonFunc p_mode )
		{
			m_comparisonFunc = p_mode;
		}

	private:
		//!\~english	Sampler interpolation modes
		//!\~french		Modes d'interpolation de l'échantillonneur.
		std::array< InterpolationMode, size_t( InterpolationFilter::eCount ) > m_eInterpolationModes
		{
			{
				InterpolationMode::eNearest,
				InterpolationMode::eNearest,
				InterpolationMode::eUndefined,
			}
		};
		//!\~english	Sampler wrapping modes.
		//!\~french		Modes de wrapping de l'échantillonneur.
		std::array< WrapMode, size_t( TextureUVW::eCount ) > m_eWrapModes
		{
			{
				WrapMode::eRepeat,
				WrapMode::eRepeat,
				WrapMode::eRepeat,
			}
		};
		//!\~english	Minimal LOD Level.
		//!\~french		Niveau de LOD minimal.
		real m_rMinLod{ -1000.0_r };
		//!\~english	Maximal LOD Level.
		//!\~french		Niveau de LOD maximal.
		real m_rMaxLod{ 1000.0_r };
		//!\~english	The texture LOD offset.
		//!\~french		Le décalage de LOD de la texture.
		real m_rLodBias{ 0.0_r };
		//!\~english	Texture border colour.
		//!\~french		Couleur des bords de la texture.
		castor::Colour m_clrBorderColour{ castor::Colour::fromComponents( 0, 0, 0, 0 ) };
		//!\~english	Maximal anisotropic filtering value.
		//!\~french		Valeur maximale pour le filtrage anisotropique.
		real m_rMaxAnisotropy{ 1.0_r };
		//!\~english	The texture comparison mode.
		//!\~french		Le mode de comparaison de la texture.
		ComparisonMode m_comparisonMode{ ComparisonMode::eNone };
		//!\~english	The texture comparison function.
		//!\~french		La fonction de comparaison de la texture.
		ComparisonFunc m_comparisonFunc{ ComparisonFunc::eLEqual };
	};
}

#endif
