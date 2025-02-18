# SPDX-License-Identifier: LGPL-3.0-or-later
#-------------------------------------------------------------------------------
#
# Copyright Panasas, 2012
# Contributor: Jim Lieb <jlieb@panasas.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
#-------------------------------------------------------------------------------

########### next target ###############

SET(avltree_STAT_SRCS
   avl.c
   bst.c
   rb.c
   splay.c
)

add_library(avltree OBJECT ${avltree_STAT_SRCS})
add_sanitizers(avltree)
set_target_properties(avltree PROPERTIES COMPILE_FLAGS "-fPIC")

if (USE_LTTNG)
add_dependencies(avltree gsh_trace_header_generate)
include("${CMAKE_BINARY_DIR}/gsh_lttng_generation_file_properties.cmake")
endif (USE_LTTNG)

########### install files ###############
