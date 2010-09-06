#ifndef Pyramide_H
#define Pyramide_H    
//______________________________________________________________________________
#include <windows.h>
#include <GL/glut.h>
#include <dos.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>                                                               
#include <string>
//______________________________________________________________________________
#include "Figure.h"
#include "Vertex3d.h"
//______________________________________________________________________________
using namespace std;                                                            
//______________________________________________________________________________
class Cone : public Figure {
    public :
        int NbFaces;
        double AlphaX, AlphaY, AlphaZ;
        double Hauteur;
        double Rayon;
        Vertex3d Centre;

        Pyramide ();
        ~Pyramide ();
};                                                                              
//______________________________________________________________________________
#endif
