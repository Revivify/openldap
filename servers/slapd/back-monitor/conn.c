/* conn.c - deal with connection subsystem */
/*
 * Copyright 1998-2000 The OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */
/*
 * Copyright 2001 The OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 * 
 * Copyright 2001, Pierangelo Masarati, All rights reserved. <ando@sys-net.it>
 * 
 * This work has beed deveolped for the OpenLDAP Foundation 
 * in the hope that it may be useful to the Open Source community, 
 * but WITHOUT ANY WARRANTY.
 * 
 * Permission is granted to anyone to use this software for any purpose
 * on any computer system, and to alter it and redistribute it, subject
 * to the following restrictions:
 * 
 * 1. The author and SysNet s.n.c. are not responsible for the consequences
 *    of use of this software, no matter how awful, even if they arise from
 *    flaws in it.
 * 
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits should appear in the documentation.
 * 
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits should appear in the documentation.
 *    SysNet s.n.c. cannot be responsible for the consequences of the
 *    alterations.
 * 
 * 4. This notice may not be removed or altered.
 */

#include "portable.h"

#include <stdio.h>

#include "slap.h"
#include "back-monitor.h"

int
monitor_subsys_conn_init(
	BackendDB		*be
)
{
	struct monitorinfo	*mi;
	
	Entry			*e, *e_tmp, *e_conn;
	struct monitorentrypriv	*mp;
	char			buf[1024];
	struct berval		val, *bv[2] = { &val, NULL };

	assert( be != NULL );

	mi = ( struct monitorinfo * )be->be_private;

	if ( monitor_cache_get( mi,
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn, &e_conn ) ) {
#ifdef NEW_LOGGING
		LDAP_LOG(( "operation", LDAP_LEVEL_CRIT,
			"monitor_subsys_conn_init: "
			"unable to get entry '%s'\n",
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn->bv_val ));
#else
		Debug( LDAP_DEBUG_ANY,
			"monitor_subsys_conn_init: "
			"unable to get entry '%s'\n%s%s",
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn->bv_val, 
			"", "" );
#endif
		return( -1 );
	}

	e_tmp = NULL;

	/*
	 * Total conns
	 */
	snprintf( buf, sizeof( buf ),
		"dn: cn=Total,%s\n"
		"objectClass: top\n"
		"objectClass: LDAPsubEntry\n"
#ifdef SLAPD_MONITORSUBENTRY
		"objectClass: monitorSubEntry\n"
#else /* !SLAPD_MONITORSUBENTRY */
		"objectClass: extensibleObject\n"
#endif /* !SLAPD_MONITORSUBENTRY */
		"cn: Total\n",
		monitor_subsys[SLAPD_MONITOR_CONN].mss_dn->bv_val );
	
	e = str2entry( buf );
	if ( e == NULL ) {
#ifdef NEW_LOGGING
		LDAP_LOG(( "operation", LDAP_LEVEL_CRIT,
			"monitor_subsys_conn_init: "
			"unable to create entry 'cn=Total,%s'\n",
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn->bv_val ));
#else
		Debug( LDAP_DEBUG_ANY,
			"monitor_subsys_conn_init: "
			"unable to create entry 'cn=Total,%s'\n%s%s",
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn->bv_val,
			"", "" );
#endif
		return( -1 );
	}
	
	val.bv_val = "0";
	val.bv_len = 1;
	attr_merge( e, monitor_ad_desc, bv );
	
	mp = ( struct monitorentrypriv * )ch_calloc( sizeof( struct monitorentrypriv ), 1 );
	e->e_private = ( void * )mp;
	mp->mp_next = e_tmp;
	mp->mp_children = NULL;
	mp->mp_info = &monitor_subsys[SLAPD_MONITOR_CONN];
	mp->mp_flags = monitor_subsys[SLAPD_MONITOR_CONN].mss_flags \
		| MONITOR_F_SUB | MONITOR_F_PERSISTENT;
	mp->mp_flags &= ~MONITOR_F_VOLATILE_CH;

	if ( monitor_cache_add( mi, e ) ) {
#ifdef NEW_LOGGING
		LDAP_LOG(( "operation", LDAP_LEVEL_CRIT,
			"monitor_subsys_conn_init: "
			"unable to add entry 'cn=Total,%s'\n",
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn->bv_val ));
#else
		Debug( LDAP_DEBUG_ANY,
			"monitor_subsys_conn_init: "
			"unable to add entry 'cn=Total,%s'\n%s%s",
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn->bv_val,
			"", "" );
#endif
		return( -1 );
	}
	
	e_tmp = e;

	/*
	 * Current conns
	 */
	snprintf( buf, sizeof( buf ),
		"dn: cn=Current,%s\n"
		"objectClass: top\n"
		"objectClass: LDAPsubEntry\n"
#ifdef SLAPD_MONITORSUBENTRY
		"objectClass: monitorSubEntry\n"
#else /* !SLAPD_MONITORSUBENTRY */
		"objectClass: extensibleObject\n"
#endif /* !SLAPD_MONITORSUBENTRY */
		"cn: Current\n",
		monitor_subsys[SLAPD_MONITOR_CONN].mss_dn->bv_val );
	
	e = str2entry( buf );
	if ( e == NULL ) {
#ifdef NEW_LOGGING
		LDAP_LOG(( "operation", LDAP_LEVEL_CRIT,
			"monitor_subsys_conn_init: "
			"unable to create entry 'cn=Current,%s'\n",
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn->bv_val ));
#else
		Debug( LDAP_DEBUG_ANY,
			"monitor_subsys_conn_init: "
			"unable to create entry 'cn=Current,%s'\n%s%s",
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn->bv_val,
			"", "" );
#endif
		return( -1 );
	}
	
	mp = ( struct monitorentrypriv * )ch_calloc( sizeof( struct monitorentrypriv ), 1 );
	e->e_private = ( void * )mp;
	mp->mp_next = e_tmp;
	mp->mp_children = NULL;
	mp->mp_info = &monitor_subsys[SLAPD_MONITOR_CONN];
	mp->mp_flags = monitor_subsys[SLAPD_MONITOR_CONN].mss_flags \
		| MONITOR_F_SUB | MONITOR_F_PERSISTENT;
	mp->mp_flags &= ~MONITOR_F_VOLATILE_CH;

	if ( monitor_cache_add( mi, e ) ) {
#ifdef NEW_LOGGING
		LDAP_LOG(( "operation", LDAP_LEVEL_CRIT,
			"monitor_subsys_conn_init: "
			"unable to add entry 'cn=Current,%s'\n",
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn->bv_val ));
#else
		Debug( LDAP_DEBUG_ANY,
			"monitor_subsys_conn_init: "
			"unable to add entry 'cn=Current,%s'\n%s%s",
			monitor_subsys[SLAPD_MONITOR_CONN].mss_ndn->bv_val,
			"", "" );
#endif
		return( -1 );
	}
	
	e_tmp = e;

	mp = ( struct monitorentrypriv * )e_conn->e_private;
	mp->mp_children = e_tmp;

	monitor_cache_release( mi, e_conn );

	return( 0 );
}

int
monitor_subsys_conn_update(
	struct monitorinfo      *mi,
	Entry                   *e
)
{
	long 		n = -1;

	assert( mi );
	assert( e );
	
	if ( strncasecmp( e->e_ndn, "CN=TOTAL", 
				sizeof("CN=TOTAL")-1 ) == 0 ) {
		n = connections_nextid();

	} else if ( strncasecmp( e->e_ndn, "CN=CURRENT", 
				sizeof("CN=CURRENT")-1 ) == 0 ) {
		Connection	*c;
		int		connindex;

		for ( n = 0, c = connection_first( &connindex );
				c != NULL;
				n++, c = connection_next( c, &connindex ) ) {
			/* No Op */ ;
		}
		connection_done(c);
	}

	if ( n != -1 ) {
		Attribute	*a;
		char		buf[16];

		a = attr_find( e->e_attrs, monitor_ad_desc );
		if ( a == NULL ) {
			return( -1 );
		}

		snprintf( buf, sizeof( buf ), "%ld", n );
		ber_bvfree( a->a_vals[ 0 ] );
		a->a_vals[ 0 ] = ber_bvstrdup( buf );
	}

	return( 0 );
}

static int
conn_create(
	Connection		*c,
	Entry			**ep
)
{
	struct monitorentrypriv *mp;
	struct tm		*ltm;
	char			buf[1024];
	char			buf2[22];
	char			buf3[22];

	struct berval           val, *bv[2] = { &val, NULL };

	Entry			*e;

	assert( c != NULL );
	assert( ep != NULL );

	snprintf( buf, sizeof( buf ),
		"dn: cn=%ld,%s\n"
		"objectClass: top\n"
		"objectClass: LDAPsubEntry\n"
#ifdef SLAPD_MONITORSUBENTRY
		"objectClass: monitorSubEntry\n"
#else /* !SLAPD_MONITORSUBENTRY */
		"objectClass: extensibleObject\n"
#endif /* !SLAPD_MONITORSUBENTRY */
		"cn: %ld\n",
		c->c_connid, monitor_subsys[SLAPD_MONITOR_CONN].mss_dn->bv_val,
		c->c_connid );
	e = str2entry( buf );

	if ( e == NULL) {
#ifdef NEW_LOGGING
		LDAP_LOG(( "operation", LDAP_LEVEL_CRIT,
			"monitor_subsys_conn_create: "
			"unable to create entry "
			"'cn=%ld,%s' entry\n",
			c->c_connid, 
			monitor_subsys[SLAPD_MONITOR_CONN].mss_dn->bv_val ));
#else
		Debug( LDAP_DEBUG_ANY,
			"monitor_subsys_conn_create: "
			"unable to create entry "
			"'cn=%ld,%s' entry\n",
			c->c_connid, 
			monitor_subsys[SLAPD_MONITOR_CONN].mss_dn->bv_val, 0 );
#endif
		return( -1 );
	}

	ldap_pvt_thread_mutex_lock( &gmtime_mutex );
	
	ltm = gmtime( &c->c_starttime );
	strftime( buf2, sizeof(buf2), "%Y%m%d%H%M%SZ", ltm );
			
	ltm = gmtime( &c->c_activitytime );
	strftime( buf3, sizeof(buf2), "%Y%m%d%H%M%SZ", ltm );
			
	ldap_pvt_thread_mutex_unlock( &gmtime_mutex );

	sprintf( buf,
		"%ld : %ld "
		": %ld/%ld/%ld/%ld "
		": %ld/%ld/%ld "
		": %s%s%s%s%s%s "
		": %s : %s : %s "
		": %s : %s : %s : %s",
		c->c_connid,
		(long) c->c_protocol,
		c->c_n_ops_received, c->c_n_ops_executing,
		c->c_n_ops_pending, c->c_n_ops_completed,
		
		/* add low-level counters here */
		c->c_n_get, c->c_n_read, c->c_n_write,
		
		c->c_currentber ? "r" : "",
		c->c_writewaiter ? "w" : "",
		c->c_ops != NULL ? "x" : "",
		c->c_pending_ops != NULL ? "p" : "",
		connection_state2str( c->c_conn_state ),
		c->c_sasl_bind_in_progress ? "S" : "",
		
		c->c_cdn ? c->c_cdn : SLAPD_ANONYMOUS,
		
		c->c_listener_url,
		c->c_peer_domain,
		c->c_peer_name,
		c->c_sock_name,
		
		buf2,
		buf3
		);

	val.bv_val = buf;
	val.bv_len = strlen( buf );
	attr_merge( e, monitor_ad_desc, bv );

	mp = ( struct monitorentrypriv * )ch_calloc( sizeof( struct monitorentrypriv ), 1 );
	e->e_private = ( void * )mp;
	mp->mp_info = &monitor_subsys[ SLAPD_MONITOR_CONN ];
	mp->mp_children = NULL;
	mp->mp_flags = MONITOR_F_SUB | MONITOR_F_VOLATILE;

	*ep = e;

	return( 0 );
}

int 
monitor_subsys_conn_create( 
	struct monitorinfo 	*mi,
	struct berval		*ndn,
	Entry 			*e_parent,
	Entry			**ep
)
{
	Connection		*c;
	int			connindex;
	struct monitorentrypriv *mp;

	assert( mi != NULL );
	assert( e_parent != NULL );
	assert( ep != NULL );

	*ep = NULL;

	if ( ndn == NULL ) {
		Entry *e, *e_tmp = NULL;

		/* create all the children of e_parent */
		for ( c = connection_first( &connindex );
				c != NULL;
				c = connection_next( c, &connindex )) {
			if ( conn_create( c, &e ) || e == NULL ) {
				connection_done(c);
				return( -1 );
			}
			mp = ( struct monitorentrypriv * )e->e_private;
			mp->mp_next = e_tmp;
			e_tmp = e;
		}
		connection_done(c);

		*ep = e;

	} else {
		char 		**values;
		struct berval	*rdn;
		unsigned long 	connid;
	       
		/* create exactly the required entry */

		if ( dnExtractRdn( ndn->bv_val, &rdn ) != LDAP_SUCCESS ) {
			return( -1 );
		}

		if ( rdn_attrs( rdn->bv_val, NULL, &values ) != LDAP_SUCCESS ) {
			ber_bvfree( rdn );
			return( -1 );
		}
		ber_bvfree( rdn );

		assert( values );
		assert( values[ 0 ] );

		connid = atol( values[ 0 ] );

		ldap_value_free( values );

		for ( c = connection_first( &connindex );
				c != NULL;
				c = connection_next( c, &connindex )) {
			if ( c->c_connid == connid ) {
				if ( conn_create( c, ep ) || *ep == NULL ) {
					connection_done(c);
					return( -1 );
				}
			}
		}
		
		connection_done(c);
	
	}

	return( 0 );
}

