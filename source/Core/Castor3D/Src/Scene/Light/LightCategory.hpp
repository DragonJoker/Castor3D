/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_CATEGORY_H___
#define ___C3D_LIGHT_CATEGORY_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/BoundingBox.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/02/2010
	\~english
	\brief		Holds light components
	\remark		Derived into DirectionalLight, PointLight and SpotLight
	\~french
	\brief		Contient les composantes d'une lumière
	\remark		Dérivée en DirectionalLight, PointLight et SpotLight
	*/
	class LightCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		LightCategory loader
		\~french
		\brief		Loader de LightCategory
		*/
		class TextWriter
			: public castor::TextWriter< LightCategory >
		{
		public:
			/**
			 *\~english
			 *\brief		Writes a LightCategory into a text file.
			 *\param[in]	file	The file.
			 *\~french
			 *\brief		Ecrit une LightCategory dans un fichier texte.
			 *\param[in]	file	Le fichier.
			 */
			C3D_API virtual bool writeInto( castor::TextFile & file ) = 0;

		protected:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a LightCategory into a text file.
			 *\param[in]	file	The file.
			 *\param[in]	light	The LightCategory to save.
			 *\~french
			 *\brief		Ecrit une LightCategory dans un fichier texte.
			 *\param[in]	file	Le fichier.
			 *\param[in]	light	La LightCategory.
			 */
			C3D_API bool operator()( LightCategory const & light, castor::TextFile & file )override;
		};

	private:
		friend class Light;

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	lightType	The light category type.
		 *\param[in]	light		The parent Light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	lightType	Le type de catégorie de lumière.
		 *\param[in]	light		La Light parente.
		 */
		C3D_API explicit LightCategory( LightType lightType, Light & light );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~LightCategory();
		/**
		 *\~english
		 *\brief		Updates the light.
		 *\~french
		 *\brief		Met la source à jour.
		 */
		C3D_API virtual void update() = 0;
		/**
		 *\~english
		 *\brief			Updates the light viewport.
		 *\param[in]		target		The target position, used by directional shadow map.
		 *\param[in,out]	viewport	The viewport to update.
		 *\param[in]		index		The light shadow map index, -1 if it doesn't have any.
		 *\~french
		 *\brief			Met le viewport de la source à jour.
		 *\param[in]		target		La position de la cible, utilisée pour la map d'ombres des source directionnelles.
		 *\param[in,out]	viewport	Le viewport à mettre à jour.
		 *\param[in]		index		L'indice de la shadow map de la lumière, -1 si elle n'en a pas.
		 */
		C3D_API virtual void updateShadow( castor::Point3r const & target
			, Viewport & viewport
			, int32_t index = -1 ) = 0;
		/**
		 *\~english
		 *\brief		Creates a LightCategroy specific TextLoader.
		 *\param[in]	tabs	The current indentation level.
		 *\return		The TextLoader.
		 *\~french
		 *\brief		Crée un TextLoader spécifique à la LightCategory.
		 *\param[in]	tabs	Le niveau d'intentation actuel.
		 *\return		Le TextLoader.
		 */
		C3D_API virtual std::unique_ptr< TextWriter > createTextWriter( castor::String const & tabs ) = 0;
		/**
		 *\~english
		 *\brief		Puts the light into the given texture.
		 *\param[out]	texture	The texture that receives the light's data.
		 *\param[in]	index	The light index.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 *\param[out]	texture	La texture recevant les données de la source lumineuse.
		 *\param[in]	index	L'indice de la source lumineuse.
		 */
		C3D_API void bind( castor::PxBufferBase & texture, uint32_t index )const;
		/**
		 *\~english
		 *\brief		Sets the light's colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setColour( castor::Point3f const & value )
		{
			m_colour = value;
		}
		/**
		 *\~english
		 *\brief		Sets the light's intensity values.
		 *\param[in]	value	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	value	Les nouvelles valeurs.
		 */
		inline void setIntensity( castor::Point2f const & value )
		{
			m_intensity = value;
		}
		/**
		 *\~english
		 *\brief		Sets the light's diffuse intensity.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'intensité diffuse.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setDiffuseIntensity( float value )
		{
			m_intensity[0] = value;
		}
		/**
		 *\~english
		 *\brief		Sets the light's specular intensity.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'intensité spéculaire.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setSpecularIntensity( float value )
		{
			m_intensity[1] = value;
		}
		/**
		 *\~english
		 *\return		The light type.
		 *\~french
		 *\return		Le type de lumière.
		 */
		inline LightType getLightType()const
		{
			return m_lightType;
		}
		/**
		 *\~english
		 *\return		The diffuse intensity.
		 *\~french
		 *\return		L'intensité diffuse.
		 */
		inline float getDiffuseIntensity()const
		{
			return m_intensity[0];
		}
		/**
		 *\~english
		 *\return		The specular intensity.
		 *\~french
		 *\return		L'intensité spéculaire.
		 */
		inline float getSpecularIntensity()const
		{
			return m_intensity[1];
		}
		/**
		 *\~english
		 *\return		The intensity values.
		 *\~french
		 *\return		Les valeurs d'intensité.
		 */
		inline castor::Point2f const & getIntensity()const
		{
			return m_intensity;
		}
		/**
		 *\~english
		 *\return		The intensity values.
		 *\~french
		 *\return		Les valeurs d'intensité.
		 */
		inline castor::Point2f & getIntensity()
		{
			return m_intensity;
		}
		/**
		 *\~english
		 *\return		The depth of the far plane.
		 *\~french
		 *\return		La profondeur du plan éloigné.
		 */
		inline float getFarPlane()const
		{
			return m_farPlane;
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		inline castor::Point3f const & getColour()const
		{
			return m_colour;
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		inline castor::Point3f & getColour()
		{
			return m_colour;
		}
		/**
		 *\~english
		 *\return		The parent light.
		 *\~french
		 *\return		La lumière parente.
		 */
		inline Light const & getLight()const
		{
			return m_light;
		}
		/**
		 *\~english
		 *\return		The parent light.
		 *\~french
		 *\return		La lumière parente.
		 */
		inline Light & getLight()
		{
			return m_light;
		}
		/**
		 *\~english
		 *\return		The light's cube box.
		 *\~french
		 *\return		La cube box de la lumière.
		 */
		inline castor::BoundingBox const & getCubeBox()const
		{
			return m_cubeBox;
		}

	protected:
		/**
		 *\~english
		 *\brief		Updates the light source's informations related to the scene node.
		 *\param[in]	p_node	The scene node.
		 *\~french
		 *\brief		Met à jour les informations de la source lumineuse, relatives au noeud de scène.
		 *\param[in]	p_node	Le noeud de scène.
		 */
		C3D_API virtual void updateNode( SceneNode const & p_node ) = 0;

	protected:
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	index		The light index.
		 *\param[in]	offset		The components offset.
		 *\param[out]	data		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	index		L'indice de la source lumineuse.
		 *\param[in]	offset		Le décalage de la composante.
		 *\param[in]	data		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point2f const & components
			, uint32_t index
			, uint32_t & offset
			, castor::PxBufferBase & data )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	component	The last component.
		 *\param[in]	index		The light index.
		 *\param[in]	offset		The components offset.
		 *\param[out]	data		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	component	La dernière composante.
		 *\param[in]	index		L'indice de la source lumineuse.
		 *\param[in]	offset		Le décalage de la composante.
		 *\param[in]	data		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point2f const & components
			, float component
			, uint32_t index
			, uint32_t & offset
			, castor::PxBufferBase & data )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	index		The light index.
		 *\param[in]	offset		The components offset.
		 *\param[out]	data		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	index		L'indice de la source lumineuse.
		 *\param[in]	offset		Le décalage de la composante.
		 *\param[in]	data		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point3f const & components
			, uint32_t index
			, uint32_t & offset
			, castor::PxBufferBase & data )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	component	The last component.
		 *\param[in]	index		The light index.
		 *\param[in]	offset		The components offset.
		 *\param[out]	data		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	Les composantes.
		 *\param[in]	component	La dernière composante.
		 *\param[in]	index		L'indice de la source lumineuse.
		 *\param[in]	offset		Le décalage de la composante.
		 *\param[in]	data		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point3f const & components
			, float component
			, uint32_t index
			, uint32_t & offset
			, castor::PxBufferBase & data )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	index		The light index.
		 *\param[in]	offset		The components offset.
		 *\param[out]	data		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	index		L'indice de la source lumineuse.
		 *\param[in]	offset		Le décalage de la composante.
		 *\param[in]	data		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point4f const & components
			, uint32_t index
			, uint32_t & offset
			, castor::PxBufferBase & data )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	index		The light index.
		 *\param[in]	offset		The components offset.
		 *\param[out]	data		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	index		L'indice de la source lumineuse.
		 *\param[in]	offset		Le décalage de la composante.
		 *\param[in]	data		Le tampon de destination.
		 */
		void doCopyComponent( castor::ConstCoords4f const & components
			, uint32_t index
			, uint32_t & offset
			, castor::PxBufferBase & data )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	index		The light index.
		 *\param[in]	offset		The components offset.
		 *\param[out]	data		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	index		L'indice de la source lumineuse.
		 *\param[in]	offset		Le décalage de la composante.
		 *\param[in]	data		Le tampon de destination.
		 */
		void doCopyComponent( castor::Coords4f const & components
			, uint32_t index
			, uint32_t & offset
			, castor::PxBufferBase & data )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	index		The light index.
		 *\param[in]	offset		The components offset.
		 *\param[out]	data		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	index		L'indice de la source lumineuse.
		 *\param[in]	offset		Le décalage de la composante.
		 *\param[in]	data		Le tampon de destination.
		 */
		void doCopyComponent( castor::Matrix4x4f const & components
			, uint32_t index
			, uint32_t & offset
			, castor::PxBufferBase & data )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	index		The light index.
		 *\param[in]	offset		The components offset.
		 *\param[out]	data		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	index		L'indice de la source lumineuse.
		 *\param[in]	offset		Le décalage de la composante.
		 *\param[in]	data		Le tampon de destination.
		 */
		void doCopyComponent( int32_t const & components
			, uint32_t index
			, uint32_t & offset
			, castor::PxBufferBase & data )const;

	private:
		/**
		 *\~english
		 *\brief		Puts the light source's type specific data into the given texture.
		 *\param[out]	texture	The texture that receives the light's data.
		 *\param[in]	index	The light index.
		 *\param[in]	offset	The starting offset.
		 *\~french
		 *\brief		Met les informations spécifiques au type de source lumineuse dans la texture donnée.
		 *\param[out]	texture	La texture recevant les données de la source lumineuse.
		 *\param[in]	index	L'indice de la source lumineuse.
		 *\param[in]	offset	Le décalage de début.
		 */
		C3D_API virtual void doBind( castor::PxBufferBase & texture
			, uint32_t index
			, uint32_t & offset )const = 0;

	protected:
		//!\~english	The cube box for the light volume of effect.
		//!\~french		La cube box pour le volume d'effet de la lumière.
		castor::BoundingBox m_cubeBox;
		//!\~english	The far plane's depth.
		//!\~french		La profondeur du plan éloigné.
		float m_farPlane{ 1.0f };

	private:
		//!\~english	The light type.
		//!\~french		Le type de lumière.
		LightType m_lightType;
		//!\~english	The parent light.
		//!\~french		La lumière parente.
		Light & m_light;
		//!\~english	The colour.
		//!\~french		La couleur.
		castor::Point3f m_colour{ 1.0, 1.0, 1.0 };
		//!\~english	The intensity values.
		//!\~french		Les valeurs d'intensité.
		castor::Point2f m_intensity{ 1.0, 1.0 };
	};
}

#endif
