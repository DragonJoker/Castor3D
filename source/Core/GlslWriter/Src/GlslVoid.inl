namespace glsl
{
	Void::Void()
		: Type( cuT( "void " ) )
	{
	}

	Void::Void( GlslWriter * writer )
		: Type( cuT( "void " ), writer, castor::String() )
	{
	}
}
