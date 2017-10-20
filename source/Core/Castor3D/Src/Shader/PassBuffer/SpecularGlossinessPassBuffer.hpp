/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SpecularGlossinessPassBuffer_H___
#define ___C3D_SpecularGlossinessPassBuffer_H___

#include "Shader/PassBuffer/PassBuffer.hpp"

#include <Design/ArrayView.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		SSBO holding the SpecularGlossinessPbrPasses data.
	\~french
	\brief		SSBO contenant les données des SpecularGlossinessPbrPass.
	*/
	class SpecularGlossinessPassBuffer
		: public PassBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	count	The max passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	count	Le nombre maximal de passes.
		 */
		C3D_API SpecularGlossinessPassBuffer( Engine & engine
			, uint32_t size );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SpecularGlossinessPassBuffer();
		/**
		 *\copydoc		castor3d::PassBuffer::Visit
		 */
		C3D_API void visit( LegacyPass const & data )override;
		/**
		 *\copydoc		castor3d::PassBuffer::Visit
		 */
		C3D_API void visit( SpecularGlossinessPbrPass const & pass )override;

	public:
		struct PassData
		{
			RgbaColour diffDiv;
			RgbaColour specGloss;
			RgbaColour common;
			RgbaColour reflRefr;
			ExtendedData extended;
		};
		using PassesData = castor::ArrayView< PassData >;

	private:
		static constexpr uint32_t DataSize = ( sizeof( RgbaColour ) * 4 )
			+ ( PassBuffer::ExtendedDataSize );

	private:
		//!\~english	The specular/glossiness PBR passes data.
		//!\~french		Les données des passes PBR specular/glossiness.
		PassesData m_data;
	};
}

#endif
