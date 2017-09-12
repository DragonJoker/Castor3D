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
#ifndef ___C3D_PassBuffer_H___
#define ___C3D_PassBuffer_H___

#include "Material/Pass.hpp"
#include "Shader/ShaderBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		ShaderBuffer holding the Passes data.
	\~french
	\brief		ShaderBuffer contenant les données des Pass.
	*/
	class PassBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	count	The max passes count.
		 *\param[in]	size	The size of a pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	count	Le nombre maximal de passes.
		 *\param[in]	size	La taille d'une passe.
		 */
		C3D_API PassBuffer( Engine & engine
			, uint32_t count
			, uint32_t size );
		/**
		 *\~english
		 *\brief		Adds a pass to the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Ajoute une passe au tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API uint32_t addPass( Pass & pass );
		/**
		 *\~english
		 *\brief		Removes a pass from the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Supprime une pass du tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API void removePass( Pass & pass );
		/**
		 *\~english
		 *\brief		Updates the passes buffer.
		 *\~french
		 *\brief		Met à jour le tampon de passes.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Binds the buffer.
		 *\~french
		 *\brief		Active le tampon.
		 */
		C3D_API void bind()const;
		/**
		 *\~english
		 *\brief		Puts the pass data into the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Met les données de la passe dans le tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API virtual void visit( LegacyPass const & pass );
		/**
		 *\~english
		 *\brief		Puts the pass data into the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Met les données de la passe dans le tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API virtual void visit( MetallicRoughnessPbrPass const & pass );
		/**
		 *\~english
		 *\brief		Puts the pass data into the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Met les données de la passe dans le tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API virtual void visit( SpecularGlossinessPbrPass const & pass );

	public:
		struct RgbColour
		{
			float r;
			float g;
			float b;
		};

		struct RgbaColour
		{
			float r;
			float g;
			float b;
			float a;
		};

#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		struct ExtendedData
		{
			castor::ArrayView< RgbaColour > sssInfo;
			castor::ArrayView< RgbaColour > transmittance;
		};

#else

		struct ExtendedData
		{
			RgbaColour sssInfo;
			RgbaColour transmittance;
		};

#endif

	protected:
		C3D_API void doVisitExtended( Pass const & pass
			, ExtendedData & data );
		C3D_API void doVisit( SubsurfaceScattering const & subsurfaceScattering
			, uint32_t index
			, ExtendedData & data );

	protected:
		//!\~english	The shader buffer.
		//!\~french		Le tampon shader.
		ShaderBuffer m_buffer;
		//!\~english	The current passes.
		//!\~french		Les passes actuelles.
		std::vector< Pass * > m_passes;
		//!\~english	The current passes.
		//!\~french		Les passes actuelles.
		std::vector< Pass const * > m_dirty;
		//!\~english	The connections to change signal for current passes.
		//!\~french		Les connexions aux signaux de changement des passes actuelles.
		std::vector< Pass::OnChangedConnection > m_connections;
		//!\~english	The maximum pass count.
		//!\~french		Le nombre maximal de passes.
		uint32_t m_passCount;
		//!\~english	The next pass ID.
		//!\~french		L'ID de la passe suivante.
		uint32_t m_passID{ 1u };
	};
}

#endif
