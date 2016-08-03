function cholmod_make
%CHOLMOD_MAKE compiles the CHOLMOD mexFunctions
%
% Example:
%   cholmod_make
%
% CHOLMOD relies on AMD and COLAMD, and optionally CCOLAMD, CAMD, and METIS.
% All but METIS are distributed with CHOLMOD.  To compile CHOLMOD to use METIS
% you must first place a copy of the metis-4.0 directory (METIS version 4.0.1)
% in same directory that contains the AMD, COLAMD, CCOLAMD, and CHOLMOD
% directories.  % See http://www-users.cs.umn.edu/~karypis/metis for a copy of
% METIS 4.0.1.
%
% You must type the cholmod_make command while in the CHOLMOD/MATLAB directory.
%
% See also analyze, bisect, chol2, cholmod2, etree2, lchol, ldlchol, ldlsolve,
%   ldlupdate, metis, spsym, nesdis, septree, resymbol, sdmult, sparse2,
%   symbfact2, mread, mwrite

%   Copyright 2006-2007, Timothy A. Davis
%   http://www.cise.ufl.edu/research/sparse

details = 0 ;	    % 1 if details of each command are to be printed

v = version ;
try
    % ispc does not appear in MATLAB 5.3
    pc = ispc ;
catch
    % if ispc fails, assume we are on a Windows PC if it's not unix
    pc = ~isunix ;
end

flags = '' ;
is64 = ~isempty (strfind (computer, '64')) ;
if (is64)
    % 64-bit MATLAB
    flags = '-largeArrayDims' ;
end

include = '-I. -I../../AMD/Include -I../../COLAMD/Include -I../../CCOLAMD/Include -I../../CAMD/Include -I../Include -I../../UFconfig' ;

if (verLessThan ('matlab', '7.0'))
    % do not attempt to compile CHOLMOD with large file support
    include = [include ' -DNLARGEFILE'] ;
elseif (~pc)
    % Linux/Unix require these flags for large file support
    include = [include ' -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE'] ;
end

if (verLessThan ('matlab', '6.5'))
    % logical class does not exist in MATLAB 6.1 or earlie
    include = [include ' -DMATLAB6p1_OR_EARLIER'] ;
end

% Determine if METIS is available
metis_path = '../../metis-4.0' ;
have_metis = exist ([metis_path '/Lib'], 'dir') ;

% fix the METIS 4.0.1 rename.h file
if (have_metis)
    fprintf ('Compiling CHOLMOD with METIS for MATLAB Version %s\n', v) ;
    f = fopen ('rename.h', 'w') ;
    if (f == -1)
        error ('unable to create rename.h in current directory') ;
    end
    fprintf (f, '/* do not edit this file; generated by cholmod_make */\n') ;
    fprintf (f, '#undef log2\n') ;
    fprintf (f, '#include "%s/Lib/rename.h"\n', metis_path) ;
    fprintf (f, '#undef log2\n') ;
    fprintf (f, '#define log2 METIS__log2\n') ;
    fprintf (f, '#include "mex.h"\n') ;
    fprintf (f, '#define malloc mxMalloc\n') ;
    fprintf (f, '#define free mxFree\n') ;
    fprintf (f, '#define calloc mxCalloc\n') ;
    fprintf (f, '#define realloc mxRealloc\n') ;
    fclose (f) ;
    include = [include ' -I' metis_path '/Lib'] ;
else
    fprintf ('Compiling CHOLMOD without METIS for MATLAB Version %s\n', v) ;
    include = ['-DNPARTITION ' include] ;
end


%-------------------------------------------------------------------------------
% BLAS option
%-------------------------------------------------------------------------------

% This is exceedingly ugly.  The MATLAB mex command needs to be told where to
% fine the LAPACK and BLAS libraries, which is a real portability nightmare.

if (pc)
    if (verLessThan ('matlab', '6.5'))
        % MATLAB 6.1 and earlier: use the version supplied here
        lapack = 'lcc_lib/libmwlapack.lib' ;
    elseif (verLessThan ('matlab', '7.5'))
        lapack = 'libmwlapack.lib' ;
    else
        lapack = 'libmwlapack.lib libmwblas.lib' ;
    end
else
    if (verLessThan ('matlab', '7.5'))
        lapack = '-lmwlapack' ;
    else
        lapack = '-lmwlapack -lmwblas' ;
    end
end

if (is64 && ~verLessThan ('matlab', '7.8'))
    % versions 7.8 and later on 64-bit platforms use a 64-bit BLAS
    fprintf ('with 64-bit BLAS\n') ;
    flags = [flags ' -DBLAS64'] ;
end

%-------------------------------------------------------------------------------

include = strrep (include, '/', filesep) ;

amd_src = { ...
    '../../AMD/Source/amd_1', ...
    '../../AMD/Source/amd_2', ...
    '../../AMD/Source/amd_aat', ...
    '../../AMD/Source/amd_control', ...
    '../../AMD/Source/amd_defaults', ...
    '../../AMD/Source/amd_dump', ...
    '../../AMD/Source/amd_global', ...
    '../../AMD/Source/amd_info', ...
    '../../AMD/Source/amd_order', ...
    '../../AMD/Source/amd_postorder', ...
    '../../AMD/Source/amd_post_tree', ...
    '../../AMD/Source/amd_preprocess', ...
    '../../AMD/Source/amd_valid' } ;

camd_src = { ...
    '../../CAMD/Source/camd_1', ...
    '../../CAMD/Source/camd_2', ...
    '../../CAMD/Source/camd_aat', ...
    '../../CAMD/Source/camd_control', ...
    '../../CAMD/Source/camd_defaults', ...
    '../../CAMD/Source/camd_dump', ...
    '../../CAMD/Source/camd_global', ...
    '../../CAMD/Source/camd_info', ...
    '../../CAMD/Source/camd_order', ...
    '../../CAMD/Source/camd_postorder', ...
    '../../CAMD/Source/camd_preprocess', ...
    '../../CAMD/Source/camd_valid' } ;

colamd_src = {
    '../../COLAMD/Source/colamd', ...
    '../../COLAMD/Source/colamd_global' } ;

ccolamd_src = {
    '../../CCOLAMD/Source/ccolamd', ...
    '../../CCOLAMD/Source/ccolamd_global' } ;

if (have_metis)
    metis_src = {
        'Lib/balance', ...
        'Lib/bucketsort', ...
        'Lib/ccgraph', ...
        'Lib/coarsen', ...
        'Lib/compress', ...
        'Lib/debug', ...
        'Lib/estmem', ...
        'Lib/fm', ...
        'Lib/fortran', ...
        'Lib/frename', ...
        'Lib/graph', ...
        'Lib/initpart', ...
        'Lib/kmetis', ...
        'Lib/kvmetis', ...
        'Lib/kwayfm', ...
        'Lib/kwayrefine', ...
        'Lib/kwayvolfm', ...
        'Lib/kwayvolrefine', ...
        'Lib/match', ...
        'Lib/mbalance2', ...
        'Lib/mbalance', ...
        'Lib/mcoarsen', ...
        'Lib/memory', ...
        'Lib/mesh', ...
        'Lib/meshpart', ...
        'Lib/mfm2', ...
        'Lib/mfm', ...
        'Lib/mincover', ...
        'Lib/minitpart2', ...
        'Lib/minitpart', ...
        'Lib/mkmetis', ...
        'Lib/mkwayfmh', ...
        'Lib/mkwayrefine', ...
        'Lib/mmatch', ...
        'Lib/mmd', ...
        'Lib/mpmetis', ...
        'Lib/mrefine2', ...
        'Lib/mrefine', ...
        'Lib/mutil', ...
        'Lib/myqsort', ...
        'Lib/ometis', ...
        'Lib/parmetis', ...
        'Lib/pmetis', ...
        'Lib/pqueue', ...
        'Lib/refine', ...
        'Lib/separator', ...
        'Lib/sfm', ...
        'Lib/srefine', ...
        'Lib/stat', ...
        'Lib/subdomains', ...
        'Lib/timing', ...
        'Lib/util' } ;

    for i = 1:length (metis_src)
        metis_src {i} = [metis_path '/' metis_src{i}] ;
    end
end

cholmod_matlab = { 'cholmod_matlab' } ;

cholmod_src = {
    '../Core/cholmod_aat', ...
    '../Core/cholmod_add', ...
    '../Core/cholmod_band', ...
    '../Core/cholmod_change_factor', ...
    '../Core/cholmod_common', ...
    '../Core/cholmod_complex', ...
    '../Core/cholmod_copy', ...
    '../Core/cholmod_dense', ...
    '../Core/cholmod_error', ...
    '../Core/cholmod_factor', ...
    '../Core/cholmod_memory', ...
    '../Core/cholmod_sparse', ...
    '../Core/cholmod_transpose', ...
    '../Core/cholmod_triplet', ...
    '../Check/cholmod_check', ...
    '../Check/cholmod_read', ...
    '../Check/cholmod_write', ...
    '../Cholesky/cholmod_amd', ...
    '../Cholesky/cholmod_analyze', ...
    '../Cholesky/cholmod_colamd', ...
    '../Cholesky/cholmod_etree', ...
    '../Cholesky/cholmod_factorize', ...
    '../Cholesky/cholmod_postorder', ...
    '../Cholesky/cholmod_rcond', ...
    '../Cholesky/cholmod_resymbol', ...
    '../Cholesky/cholmod_rowcolcounts', ...
    '../Cholesky/cholmod_rowfac', ...
    '../Cholesky/cholmod_solve', ...
    '../Cholesky/cholmod_spsolve', ...
    '../MatrixOps/cholmod_drop', ...
    '../MatrixOps/cholmod_horzcat', ...
    '../MatrixOps/cholmod_norm', ...
    '../MatrixOps/cholmod_scale', ...
    '../MatrixOps/cholmod_sdmult', ...
    '../MatrixOps/cholmod_ssmult', ...
    '../MatrixOps/cholmod_submatrix', ...
    '../MatrixOps/cholmod_vertcat', ...
    '../MatrixOps/cholmod_symmetry', ...
    '../Modify/cholmod_rowadd', ...
    '../Modify/cholmod_rowdel', ...
    '../Modify/cholmod_updown', ...
    '../Supernodal/cholmod_super_numeric', ...
    '../Supernodal/cholmod_super_solve', ...
    '../Supernodal/cholmod_super_symbolic', ...
    '../Partition/cholmod_ccolamd', ...
    '../Partition/cholmod_csymamd', ...
    '../Partition/cholmod_camd', ...
    '../Partition/cholmod_metis', ...
    '../Partition/cholmod_nesdis' } ;

cholmod_mex_src = { ...
    'analyze', ...
    'bisect', ...
    'chol2', ...
    'cholmod2', ...
    'etree2', ...
    'lchol', ...
    'ldlchol', ...
    'ldlsolve', ...
    'ldlupdate', ...
    'metis', ...
    'spsym', ...
    'nesdis', ...
    'septree', ...
    'resymbol', ...
    'sdmult', ...
    'sparse2', ...
    'symbfact2', ...
    'mread', ...
    'mwrite' } ;

if (pc)
    % Windows does not have drand48 and srand48, required by METIS.  Use
    % drand48 and srand48 in CHOLMOD/MATLAB/Windows/rand48.c instead.
    % Also provide Windows with an empty <strings.h> include file.
    obj_extension = '.obj' ;
    cholmod_matlab = [cholmod_matlab {'Windows/rand48'}] ;
    include = [include ' -IWindows'] ;
else
    obj_extension = '.o' ;
end

% compile each library source file
obj = '' ;

source = [amd_src colamd_src ccolamd_src camd_src cholmod_src cholmod_matlab] ;
if (have_metis)
    source = [metis_src source] ;
end

kk = 0 ;

for f = source
    ff = strrep (f {1}, '/', filesep) ;
    slash = strfind (ff, filesep) ;
    if (isempty (slash))
        slash = 1 ;
    else
        slash = slash (end) + 1 ;
    end
    o = ff (slash:end) ;
    obj = [obj  ' ' o obj_extension] ;					    %#ok
    s = sprintf ('mex %s -DDLONG -O %s -c %s.c', flags, include, ff) ;
    kk = do_cmd (s, kk, details) ;
end

% compile each mexFunction
for f = cholmod_mex_src
    s = sprintf ('mex %s -DDLONG -O %s %s.c', flags, include, f{1}) ;
    s = [s obj ' ' lapack] ;						    %#ok
    kk = do_cmd (s, kk, details) ;
end

% clean up
s = ['delete ' obj] ;
do_cmd (s, kk, details) ;
fprintf ('\nCHOLMOD successfully compiled\n') ;

%-------------------------------------------------------------------------------
function kk = do_cmd (s, kk, details)
%DO_CMD: evaluate a command, and either print it or print a "."
if (details)
    fprintf ('%s\n', s) ;
else
    if (mod (kk, 60) == 0)
	fprintf ('\n') ;
    end
    kk = kk + 1 ;
    fprintf ('.') ;
end
eval (s) ;
