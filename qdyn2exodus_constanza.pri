# This file is to provide system-dependent variables.

# Set either EXODUS_LIBMESH or EXODUS_LIBRARY to true if you want the exodus export option.
EXODUS_LIBMESH = true
EXODUS_LIBRARY = false

# By default the LIBMESH takes the environment variable LIBMESHDIR defined in compile.sh:
# LIBMESH = $$(LIBMESHDIR)
# Alternatively, comment the line above and replace below the path to the root directory of a libmesh installation:
# LIBMESH= /home/mauro/projects/moose_libmesh/libmesh/installed
LIBMESH= /Users/crpiceda/mambaforge3/envs/moose/libmesh

# If EXODUS_LIBRARY is true, insert below the path to the root directory of the exodusII library and netcdf.
NETCDF_PATH = /Users/crpiceda/seacas/TPL/netcdf/netcdf-c
EXODUS_PATH = /Users/crpiceda/seacas/
