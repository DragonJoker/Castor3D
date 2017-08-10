namespace GLSL
{
	Void::Void()
		: Type( cuT( "void " ) )
	{
	}

	Void::Void( GlslWriter * p_writer )
		: Type( cuT( "void " ), p_writer, castor::String() )
	{
	}
}
