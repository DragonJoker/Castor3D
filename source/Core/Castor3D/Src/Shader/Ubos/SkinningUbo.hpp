/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkinningUbo_H___
#define ___C3D_SkinningUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Matrices Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour les matrices.
	*/
	class SkinningUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API SkinningUbo( SkinningUbo const & ) = delete;
		C3D_API SkinningUbo & operator=( SkinningUbo const & ) = delete;
		C3D_API SkinningUbo( SkinningUbo && ) = default;
		C3D_API SkinningUbo & operator=( SkinningUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit SkinningUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkinningUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	skeleton	The overlay's material index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	skeleton	L'index du matériau de l'incrustation.
		 */
		C3D_API void update( AnimatedSkeleton const & skeleton )const;
		/**
		 *\~english
		 *\brief		Declares the GLSL variables needed to compute skinning in vertex shader.
		 *\param[in]	writer	The GLSL writer.
		 *\param[in]	set		The descriptor set index.
		 *\param[in]	flags	The program flags.
		 *\~french
		 *\brief		Déclare les variables nécessaires au calcul du skinning dans le vertex shader.
		 *\param[in]	writer	Le GLSL writer.
		 *\param[in]	set		L'index du descriptor set.
		 *\param[in]	flags	Les indicateurs du programme.
		 */
		C3D_API static void declare( glsl::GlslWriter & writer
			, uint32_t set
			, ProgramFlags const & flags );
		/**
		 *\~english
		 *\brief		Computes skinning transformation in vertex shader.
		 *\param[in]	writer	The GLSL writer.
		 *\param[in]	flags	The program flags.
		 *\return		The resulting matrix.
		 *\~french
		 *\brief		Effectue le calcul de la transformation du skinning dans le vertex shader.
		 *\param[in]	writer	Le GLSL writer.
		 *\param[in]	flags	Les indicateurs du programme.
		 *\return		La matrice résultat.
		 */
		C3D_API static glsl::Mat4 computeTransform( glsl::GlslWriter & writer
			, ProgramFlags const & flags );
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		inline UniformBuffer & getUbo()
		{
			return m_ubo;
		}

		inline UniformBuffer const & getUbo()const
		{
			return m_ubo;
		}
		/**@}*/

	public:
		static constexpr uint32_t BindingPoint = 5u;
		//!\~english	Name of the skinning animation frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données d'animation de skinning.
		C3D_API static castor::String const BufferSkinning;
		//!\~english	Name of the bones matrices attribute.
		//!\~french		Nom de l'attribut de de matrices d'os.
		C3D_API static castor::String const Bones;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The bones matrices uniform variable.
		//!\~french		Le variable uniforme contenant les matrices des os.
		Uniform4x4f & m_bonesMatrix;
	};
}

#endif
