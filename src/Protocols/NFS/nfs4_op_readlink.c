// SPDX-License-Identifier: LGPL-3.0-or-later
/*
 * vim:noexpandtab:shiftwidth=8:tabstop=8:
 *
 * Copyright CEA/DAM/DIF  (2008)
 * contributeur : Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * ---------------------------------------
 */

/**
 * @file nfs4_op_readlink.c
 * @brief Routines used for managing the NFS4 COMPOUND functions.
 *
 * Routines used for managing the NFS4 COMPOUND functions.
 */
#include "config.h"
#include "log.h"
#include "nfs4.h"
#include "nfs_core.h"
#include "nfs_proto_functions.h"
#include "nfs_proto_tools.h"
#include "nfs_convert.h"
#include "nfs_file_handle.h"
#include "gsh_types.h"

#include "gsh_lttng/gsh_lttng.h"
#if defined(USE_LTTNG) && !defined(LTTNG_PARSING)
#include "gsh_lttng/generated_traces/nfs4.h"
#endif

/**
 * @brief The NFS4_OP_READLINK operation.
 *
 * This function implements the NFS4_OP_READLINK operation.
 *
 * @param[in]     op   Arguments for nfs4_op
 * @param[in,out] data Compound request's data
 * @param[out]    resp Results for nfs4_op
 *
 * @return per RFC5661, p. 372
 *
 * @see nfs4_Compound
 *
 */

enum nfs_req_result nfs4_op_readlink(struct nfs_argop4 *op,
				     compound_data_t *data,
				     struct nfs_resop4 *resp)
{
	READLINK4res *const res_READLINK4 = &resp->nfs_resop4_u.opreadlink;
	fsal_status_t fsal_status = { 0, 0 };
	struct gsh_buffdesc link_buffer = { .addr = NULL, .len = 0 };
	uint32_t resp_size;

	GSH_AUTO_TRACEPOINT(nfs4, op_readlink_start, TRACE_INFO,
			    "READLINK start");

	resp->resop = NFS4_OP_READLINK;
	res_READLINK4->status = NFS4_OK;

	/*
	 * Do basic checks on a filehandle You can readlink only on a link
	 * ...
	 */
	res_READLINK4->status =
		nfs4_sanity_check_FH(data, SYMBOLIC_LINK, false);

	if (res_READLINK4->status != NFS4_OK)
		return NFS_REQ_ERROR;

	fsal_status = fsal_readlink(data->current_obj, &link_buffer);
	if (FSAL_IS_ERROR(fsal_status)) {
		res_READLINK4->status = nfs4_Errno_status(fsal_status);
		return NFS_REQ_ERROR;
	}

	res_READLINK4->READLINK4res_u.resok4.link.utf8string_val =
		link_buffer.addr;

	/* NFSv4 does not require the \NUL terminator. */
	res_READLINK4->READLINK4res_u.resok4.link.utf8string_len =
		link_buffer.len - 1;

	/* Response size is space for nfsstat4, length, pointer, and the
	 * link itself.
	 */
	resp_size = RNDUP(link_buffer.len) + 3 * sizeof(uint32_t);

	res_READLINK4->status = check_resp_room(data, resp_size);

	if (res_READLINK4->status != NFS4_OK) {
		/* No room for response, free link. */
		gsh_free(res_READLINK4->READLINK4res_u.resok4.link
				 .utf8string_val);
	}

	data->op_resp_size = resp_size;

	GSH_AUTO_TRACEPOINT(
		nfs4, op_readlink_end, TRACE_INFO,
		"READLINK res: status={} len={}", res_READLINK4->status,
		res_READLINK4->READLINK4res_u.resok4.link.utf8string_len);
	return nfsstat4_to_nfs_req_result(res_READLINK4->status);
} /* nfs4_op_readlink */

/**
 * @brief Free memory allocated for READLINK result
 *
 * This function frees the memory allocated for the resutl of the
 * NFS4_OP_READLINK operation.
 *
 * @param[in,out] resp nfs4_op results
*/
void nfs4_op_readlink_Free(nfs_resop4 *res)
{
	READLINK4res *resp = &res->nfs_resop4_u.opreadlink;

	if (resp->status == NFS4_OK)
		gsh_free(resp->READLINK4res_u.resok4.link.utf8string_val);
}
