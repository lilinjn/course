-------------------------------------
Notes
-------------------------------------

Differences from the upstream build:

----------------------------

  * Add simple cmake support
      * Cmake only configures things for OS X (but it should be simple to add linux)
      * In Cmake, turned off deprecations warnings since OS X is anti GLUT

--------------------------

  * Placed suitesparse and metis from circa 2012 into vendor directory
      * metis is in a subdir of suitesparse
      * They were built to install their headers and libraries
      into "usr" under the project root.
      * These build artifacts have been checked into source control
      for simplicity/laziness.  (The binaries are OSX intel-64bit)


------------------------------------
