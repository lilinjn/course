#include <iostream>
using namespace std;

#include "Viewer.h"
#include "Direction.h"
using namespace DDG;

int main( int argc, char** argv )
{
   const char *file = "bunny.obj";
   if( argc == 2 )
   {
      file = argv[1];
   }

   Viewer viewer;
   viewer.mesh.read( file );
   viewer.mesh.init();
   
   DirectionField field;
   field.generate( viewer.mesh, viewer.angle );
   
   viewer.init();

   return 0;
}

