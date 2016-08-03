#include <iostream>
using namespace std;

#include "Viewer.h"
#include "DenseMatrix.h"
using namespace DDG;

#include <unistd.h>

int main( int argc, char** argv )
{
   srand( time( NULL ));
   const char *file = "bunny.obj";
   if( argc == 2 )
   {
      file = argv[1];
   }

   Viewer viewer;
   viewer.mesh.read( file );


   viewer.init();

   return 0;
}

