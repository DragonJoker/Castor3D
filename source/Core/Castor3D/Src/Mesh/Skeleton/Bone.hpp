/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BONE_H___
#define ___C3D_BONE_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

#include <Design/Named.hpp>
#include <Math/SquareMatrix.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/06/2013
	\version	0.7.0
	\~english
	\brief		Class holding bone data
	\remark		Holds weight for each vertice and matrix from mesh space to bone space
	\~french
	\brief		Classe contenant les données d'un bone
	\remark		Contient les poids pour chaque vertice et la matrice de transformation de l'espace objet vers l'espace bone
	*/
	class Bone
		: public std::enable_shared_from_this< Bone >
		, public castor::Named
	{
		friend class Skeleton;
		friend class BinaryWriter< Bone >;
		friend class BinaryParser< Bone >;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_skeleton	The parent skeleton
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_skeleton	Le squelette parent
		 */
		C3D_API explicit Bone( Skeleton & p_skeleton );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Bone();
		/**
		 *\~english
		 *\brief		Retrieves the transfromation matrix from mesh space to bone space
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice de transformation de l'espace objet vers l'espace du bone
		 *\return		La valeur
		 */
		inline const castor::Matrix4x4r & getOffsetMatrix()const
		{
			return m_offset;
		}
		/**
		 *\~english
		 *\brief		sets the transfromation matrix from mesh space to bone space
		 *\param[in]	p_offset	The new value
		 *\~french
		 *\brief		Définit la matrice de transformation de l'espace objet vers l'espace du bone
		 *\param[in]	p_offset	La nouvelle valeur
		 */
		inline void setOffsetMatrix( const castor::Matrix4x4r & p_offset )
		{
			m_offset = p_offset;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent skeleton
		 *\return		The value
		 *\~french
		 *\brief		Récupère le squelette parent
		 *\return		La valeur
		 */
		inline const Skeleton & getSkeleton()const
		{
			return m_skeleton;
		}
		/**
		 *\~english
		 *\return		The parent bone.
		 *\~french
		 *\return		L'os parent.
		 */
		BoneSPtr getParent()const
		{
			return m_parent;
		}

	private:
		/**
		 *\~english
		 *\brief		adds a child bone
		 *\param[in]	p_bone	The bone
		 *\~french
		 *\brief		Ajoute un os enfant
		 *\param[in]	p_bone	L'os
		 */
		void addChild( BoneSPtr p_bone );
		/**
		 *\~english
		 *\brief		sets the parent bone.
		 *\param[in]	p_bone	The bone.
		 *\~french
		 *\brief		Définit l'os parent.
		 *\param[in]	p_bone	L'os.
		 */
		void setParent( BoneSPtr p_bone )
		{
			m_parent = p_bone;
		}

	private:
		//!\~english	The parent bone.
		//!\~french		L'os parent.
		BoneSPtr m_parent;
		//!\~english	The matrix from mesh to bone space.
		//!\~french		La matrice de transformation de l'espace mesh vers l'espace bone.
		castor::Matrix4x4r m_offset;
		//!\~english	The bones depending on this one.
		//!\~french		Les bones dépendant de celui-ci.
		BonePtrStrMap m_children;
		//!\~english	The parent skeleton.
		//!\~french		Le squelette parent.
		Skeleton & m_skeleton;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for Bone.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour Bone.
	*/
	template<>
	struct ChunkTyper< Bone >
	{
		static ChunkType const Value = ChunkType::eSkeletonBone;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		Animable binary loader.
	\~english
	\brief		Loader binaire d'Animable.
	*/
	template<>
	class BinaryWriter< Bone >
		: public BinaryWriterBase< Bone >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data.
		 *\param[in]	p_obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	p_obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doWrite( Bone const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		Animable binary loader.
	\~english
	\brief		Loader binaire d'Animable.
	*/
	template<>
	class BinaryParser< Bone >
		: public BinaryParserBase< Bone >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk.
		 *\param[out]	p_obj	The object to read.
		 *\param[in]	p_chunk	The chunk containing data.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	p_obj	L'objet à lire.
		 *\param[in]	p_chunk	Le chunk contenant les données.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse( Bone & p_obj )override;
	};
}

#endif
