/* ========================================================================== */
/* === umf_config.h ========================================================= */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* UMFPACK Copyright (c) Timothy A. Davis, CISE,                              */
/* Univ. of Florida.  All Rights Reserved.  See ../Doc/License for License.   */
/* web: http://www.cise.ufl.edu/research/sparse/umfpack                       */
/* -------------------------------------------------------------------------- */

/*
    This file controls the compile-time configuration of UMFPACK.  Modify the
    UFconfig/UFconfig.mk file and this file if necessary, to control these
    options.  The following flags may be given as options to your C compiler
    (as in "cc -DNSUNPERF", for example).  These flags are normally placed in
    your UMFPACK_CONFIG string, defined in the UFconfig/UFconfig.mk file.

    All of these options, except for the timer, are for accessing the BLAS.

	-DNSUNPERF

	    Applies only to Sun Solaris.  If -DNSUNPERF is set, then the Sun
	    Performance Library BLAS will not be used.

	    The Sun Performance Library BLAS is used by default when compiling
	    the C-callable libumfpack.a library on Sun Solaris.

	-DLONGBLAS

	-DNPOSIX

	    If -DNPOSIX is set, then your Unix operating system is not POSIX-
	    compliant, and the POSIX routines sysconf ( ) and times ( )
	    routines are not used.  These routines provide CPU time and
	    wallclock time information.  If -DNPOSIX is set, then the ANSI
	    C clock ( ) routine is used.  If -DNPOSIX is not set, then
	    sysconf ( ) and times ( ) are used in umfpack_tic and umfpack_toc.
	    See umfpack_tictoc.c for more information.
	    The default is to use the POSIX routines, except for Windows,
	    which is not POSIX-compliant.

	-DGETRUSAGE

	    If -DGETRUSAGE is set, then your system's getrusage ( ) routine
	    will be used for getting the process CPU time.  Otherwise the ANSI
	    C clock ( ) routine will be used.  The default is to use getrusage
	    ( ) on Unix systems, and to use clock on all other architectures.

	-DNO_TIMER

	    If -DNO_TIMER is set, then no timing routines are used at all.

	-DNRECIPROCAL

	    This option controls a tradeoff between speed and accuracy.  Using
	    -DNRECIPROCAL can lead to more accurate results, but with perhaps
	    some cost in performance, particularly if floating-point division
	    is much more costly than floating-point multiplication.

	    This option determines the method used to scale the pivot column.
	    If set, or if the absolute value of the pivot is < 1e-12 (or is a
	    NaN), then the pivot column is divided by the pivot value.
	    Otherwise, the reciprocal of the pivot value is computed, and the
	    pivot column is multiplied by (1/pivot).  Multiplying by the
	    reciprocal can be slightly less accurate than dividing by the
	    pivot, but it is often faster.  See umf_scale.c.

	    This has a small effect on the performance of UMFPACK, at least on
	    a Pentium 4M.  It may have a larger effect on other architectures
	    where floating-point division is much more costly than floating-
	    point multiplication.  The RS 6000 is one such example.

	    By default, the method chosen is to multiply by the reciprocal
	    (sacrificing accuracy for speed), except when compiling UMFPACK
	    as a built-in routine in MATLAB, or when gcc is being used.

	    When MATHWORKS is defined, -DNRECIPROCAL is forced on, and the pivot
	    column is divided by the pivot value.  The only way of using the
	    other method in this case is to edit this file.

	    If -DNRECIPROCAL is enabled, then the row scaling factors are always
	    applied by dividing each row by the scale factor, rather than
	    multiplying by the reciprocal.  If -DNRECIPROCAL is not enabled
	    (the default case), then the scale factors are normally applied by
	    multiplying by the reciprocal.  If, however, the smallest scale
	    factor is tiny, then the scale factors are applied via division.

	-DNO_DIVIDE_BY_ZERO

	    If the pivot is zero, and this flag is set, then no divide-by-zero
	    occurs.

    The following options are controlled by amd_internal.h:

	-DMATLAB_MEX_FILE

	    This flag is turned on when compiling the umfpack mexFunction for
	    use in MATLAB.  The -DNRECIPROCAL flag is forced on (more accurate,
	    slightly slower).  The umfpack mexFunction always returns
	    L*U = P*(R\A)*Q.

	-DMATHWORKS

	    This flag is turned on when compiling umfpack as a built-in routine
	    in MATLAB.  The -DNRECIPROCAL flag is forced on.

	-DNDEBUG

	    Debugging mode (if NDEBUG is not defined).  The default, of course,
	    is no debugging.  Turning on debugging takes some work (see below).
	    If you do not edit this file, then debugging is turned off anyway,
	    regardless of whether or not -DNDEBUG is specified in your compiler
	    options.
*/

/* ========================================================================== */
/* === AMD configuration ==================================================== */
/* ========================================================================== */

/* NDEBUG, PRINTF defined in amd_internal.h */

/* ========================================================================== */
/* === reciprocal option ==================================================== */
/* ========================================================================== */

/* Force the definition NRECIPROCAL when MATHWORKS or MATLAB_MEX_FILE
 * are defined.  Do not multiply by the reciprocal in those cases. */

#ifndef NRECIPROCAL
#if defined (MATHWORKS) || defined (MATLAB_MEX_FILE)
#define NRECIPROCAL
#endif
#endif

/* ========================================================================== */
/* === Microsoft Windows configuration ====================================== */
/* ========================================================================== */

#if defined (UMF_WINDOWS) || defined (UMF_MINGW)
/* Windows isn't Unix.  Profound. */
#define NPOSIX
#endif

/* ========================================================================== */
/* === 0-based or 1-based printing ========================================== */
/* ========================================================================== */

#if defined (MATLAB_MEX_FILE) && defined (NDEBUG)
/* In MATLAB, matrices are 1-based to the user, but 0-based internally. */
/* One is added to all row and column indices when printing matrices */
/* for the MATLAB user.  The +1 shift is turned off when debugging. */
#define INDEX(i) ((i)+1)
#else
/* In ANSI C, matrices are 0-based and indices are reported as such. */
/* This mode is also used for debug mode, and if MATHWORKS is defined rather */
/* than MATLAB_MEX_FILE. */
#define INDEX(i) (i)
#endif

/* ========================================================================== */
/* === Timer ================================================================ */
/* ========================================================================== */

/*
    If you have the getrusage routine (all Unix systems I've test do), then use
    that.  Otherwise, use the ANSI C clock function.   Note that on many
    systems, the ANSI clock function wraps around after only 2147 seconds, or
    about 36 minutes.  BE CAREFUL:  if you compare the run time of UMFPACK with
    other sparse matrix packages, be sure to use the same timer.  See
    umfpack_tictoc.c for the timer used internally by UMFPACK.  See also
    umfpack_timer.c for the timer used in an earlier version of UMFPACK.
    That timer is still available as a user-callable routine, but it is no
    longer used internally by UMFPACK.
*/

/* Sun Solaris, SGI Irix, Linux, Compaq Alpha, and IBM RS 6000 all have */
/* getrusage.  It's in BSD unix, so perhaps all unix systems have it. */
#if defined (UMF_SOL2) || defined (UMF_SGI) || defined (UMF_LINUX) \
|| defined (UMF_ALPHA) || defined (UMF_AIX) || defined (UMF_CYGWIN) \
|| defined (UMF_MAC)
#define GETRUSAGE
#endif


/* ========================================================================== */
/* === BLAS ================================================================= */
/* ========================================================================== */

#define BLAS_OK blas_ok
#include "cholmod_blas.h"


/* -------------------------------------------------------------------------- */
/* DGEMM */
/* -------------------------------------------------------------------------- */

/* C = C - A*B', where:
 * A is m-by-k with leading dimension ldac
 * B is k-by-n with leading dimension ldb
 * C is m-by-n with leading dimension ldac */
#ifdef COMPLEX
#define BLAS_GEMM(m,n,k,A,B,ldb,C,ldac) \
{ \
    double alpha [2] = {-1,0}, beta [2] = {1,0} ; \
    BLAS_zgemm ("N", "T", m, n, k, alpha, (double *) A, ldac, \
	(double *) B, ldb, beta, (double *) C, ldac) ; \
}
#else
#define BLAS_GEMM(m,n,k,A,B,ldb,C,ldac) \
{ \
    double alpha = -1, beta = 1 ; \
    BLAS_dgemm ("N", "T", m, n, k, &alpha, A, ldac, B, ldb, &beta, C, ldac) ; \
}
#endif


/* -------------------------------------------------------------------------- */
/* GER */
/* -------------------------------------------------------------------------- */

/* A = A - x*y', where:
 * A is m-by-n with leading dimension d
   x is a column vector with stride 1
   y is a column vector with stride 1 */
#ifdef COMPLEX
#define BLAS_GER(m,n,x,y,A,d) \
{ \
    double alpha [2] = {-1,0} ; \
    BLAS_zgeru (m, n, alpha, (double *) x, 1, (double *) y, 1, \
	(double *) A, d) ; \
}
#else
#define BLAS_GER(m,n,x,y,A,d) \
{ \
    double alpha = -1 ; \
    BLAS_dger (m, n, &alpha, x, 1, y, 1, A, d) ; \
}
#endif


/* -------------------------------------------------------------------------- */
/* GEMV */
/* -------------------------------------------------------------------------- */

/* y = y - A*x, where A is m-by-n with leading dimension d,
   x is a column vector with stride 1
   y is a column vector with stride 1 */
#ifdef COMPLEX
#define BLAS_GEMV(m,n,A,x,y,d) \
{ \
    double alpha [2] = {-1,0}, beta [2] = {1,0} ; \
    BLAS_zgemv ("N", m, n, alpha, (double *) A, d, (double *) x, 1, beta, \
	(double *) y, 1) ; \
}
#else
#define BLAS_GEMV(m,n,A,x,y,d) \
{ \
    double alpha = -1, beta = 1 ; \
    BLAS_dgemv ("N", m, n, &alpha, A, d, x, 1, &beta, y, 1) ; \
}
#endif


/* -------------------------------------------------------------------------- */
/* TRSV */
/* -------------------------------------------------------------------------- */

/* solve Lx=b, where:
 * B is a column vector (m-by-1) with leading dimension d
 * A is m-by-m with leading dimension d */
#ifdef COMPLEX
#define BLAS_TRSV(m,A,b,d) \
{ \
    BLAS_ztrsv ("L", "N", "U", m, (double *) A, d, (double *) b, 1) ; \
}
#else
#define BLAS_TRSV(m,A,b,d) \
{ \
    BLAS_dtrsv ("L", "N", "U", m, A, d, b, 1) ; \
}
#endif


/* -------------------------------------------------------------------------- */
/* TRSM */
/* -------------------------------------------------------------------------- */

/* solve XL'=B where:
 * B is m-by-n with leading dimension ldb
 * A is n-by-n with leading dimension lda */
#ifdef COMPLEX
#define BLAS_TRSM_RIGHT(m,n,A,lda,B,ldb) \
{ \
    double alpha [2] = {1,0} ; \
    BLAS_ztrsm ("R", "L", "T", "U", m, n, alpha, (double *) A, lda, \
	(double *) B, ldb) ; \
}
#else
#define BLAS_TRSM_RIGHT(m,n,A,lda,B,ldb) \
{ \
    double alpha = 1 ; \
    BLAS_dtrsm ("R", "L", "T", "U", m, n, &alpha, A, lda, B, ldb) ; \
}
#endif


/* -------------------------------------------------------------------------- */
/* SCAL */
/* -------------------------------------------------------------------------- */

/* x = s*x, where x is a stride-1 vector of length n */
#ifdef COMPLEX
#define BLAS_SCAL(n,s,x) \
{ \
    double alpha [2] ; \
    alpha [0] = REAL_COMPONENT (s) ; \
    alpha [1] = IMAG_COMPONENT (s) ; \
    BLAS_zscal (n, alpha, (double *) x, 1) ; \
}
#else
#define BLAS_SCAL(n,s,x) \
{ \
    double alpha = REAL_COMPONENT (s) ; \
    BLAS_dscal (n, &alpha, (double *) x, 1) ; \
}
#endif
