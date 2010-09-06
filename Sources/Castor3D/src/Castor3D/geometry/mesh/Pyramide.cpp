#include "Pyramide.h"
//______________________________________________________________________________
Pyramide :: Pyramide ()
{
    
    v0 = new Vertex3d(0.0, -0.5, 0.0);
    v1 = new Vertex3d(0.5, 0.0, 0.0);
    v2 = new Vertex3d(-0.5, 0.0, 0.0);
    v3 = new Vertex3d(0.0, 0.5, 0.0);
    v4 = new Vertex3d(0.0, 0.0, 1.0);
}
//______________________________________________________________________________
Pyramide :: ~Pyramide ()
{
    delete(v0);
    delete(v1);
    delete(v2);
    delete(v3);
    delete(v4);
}
//______________________________________________________________________________

//______________________________________________________________________________
