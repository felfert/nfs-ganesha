/* SPDX-License-Identifier: LGPL-3.0-or-later */
/*
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
 * @defgroup idmapper ID Mapper
 *
 * The ID Mapper module provides mapping between numerical user and
 * group IDs and NFSv4 style owner and group strings.
 *
 * @{
 */

/**
 * @file idmapper.c
 * @brief Id mapping functions
 */

#ifndef IDMAPPER_H
#define IDMAPPER_H
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include "gsh_rpc.h"
#include "gsh_types.h"

/* Arbitrary string buffer lengths */
#define PWENT_BEST_GUESS_LEN 1024
/* Maintain 64MB as the max buffer length for passwd and group entries */
#define PWENT_MAX_SIZE (64 * 1024 * 1024)
#define GROUP_MAX_SIZE PWENT_MAX_SIZE

/**
 * @brief Shared between idmapper.c and idmapper_cache.c.  If you
 * aren't in idmapper.c, leave these symbols alone.
 *
 * @{
 */

extern pthread_rwlock_t idmapper_user_lock;
extern pthread_rwlock_t idmapper_group_lock;
extern pthread_rwlock_t idmapper_negative_cache_user_lock;
extern pthread_rwlock_t idmapper_negative_cache_group_lock;

void idmapper_cache_init(void);
void idmapper_cache_reap(void);
bool idmapper_add_user(const struct gsh_buffdesc *, uid_t, const gid_t *, bool);
bool idmapper_add_group(const struct gsh_buffdesc *, gid_t);
bool idmapper_lookup_by_uname(const struct gsh_buffdesc *, uid_t *,
			      const gid_t **, bool);
bool idmapper_lookup_by_uid(const uid_t, const struct gsh_buffdesc **,
			    const gid_t **);
bool idmapper_lookup_by_gname(const struct gsh_buffdesc *, uid_t *);
bool idmapper_lookup_by_gid(const gid_t, const struct gsh_buffdesc **);

void idmapper_negative_cache_init(void);
void idmapper_negative_cache_add_user_by_name(const struct gsh_buffdesc *);
void idmapper_negative_cache_add_group_by_name(const struct gsh_buffdesc *);
bool idmapper_negative_cache_lookup_user_by_name(const struct gsh_buffdesc *);
bool idmapper_negative_cache_lookup_group_by_name(const struct gsh_buffdesc *);
void idmapper_negative_cache_clear(void);
void idmapper_negative_cache_destroy(void);
void idmapper_negative_cache_reap(void);

/** @} */

bool idmapper_init(void);
void idmapper_clear_cache(void);
void idmapper_destroy_cache(void);

bool set_idmapping_status(bool status_enabled);

bool xdr_encode_nfs4_owner(XDR *, uid_t);
bool xdr_encode_nfs4_group(XDR *, gid_t);

bool name2uid(const struct gsh_buffdesc *, uid_t *, const uid_t);
bool name2gid(const struct gsh_buffdesc *, gid_t *, const gid_t);

void winbind_stats_update(struct timespec *, struct timespec *);
void gc_stats_update(struct timespec *, struct timespec *);
void dns_stats_update(struct timespec *, struct timespec *);

#ifdef _HAVE_GSSAPI
#ifdef _MSPAC_SUPPORT
bool principal2uid(char *, uid_t *, gid_t *, struct svc_rpc_gss_data *);
#else
bool principal2uid(char *, uid_t *, gid_t *);
#endif
#endif

#ifdef USE_DBUS
extern struct gsh_dbus_method cachemgr_show_idmapper;
extern struct gsh_dbus_method auth_statistics;
#endif

#endif /* IDMAPPER_H */
/** @} */
