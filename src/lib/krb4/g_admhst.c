/*
 * g_admhst.c
 *
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology.
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 */

#include "mit-copyright.h"
#include <stdio.h>
#include "krb.h"
#include <string.h>

/*
 * Given a Kerberos realm, find a host on which the Kerberos database
 * administration server can be found.
 *
 * krb_get_admhst takes a pointer to be filled in, a pointer to the name
 * of the realm for which a server is desired, and an integer n, and
 * returns (in h) the nth administrative host entry from the configuration
 * file (KRB_CONF, defined in "krb.h") associated with the specified realm.
 * If ATHENA_CONF_FALLBACK is defined, also look in old location.
 *
 * On error, get_admhst returns KFAILURE. If all goes well, the routine
 * returns KSUCCESS.
 *
 * For the format of the KRB_CONF file, see comments describing the routine
 * krb_get_krbhst().
 *
 * This is a temporary hack to allow us to find the nearest system running
 * a Kerberos admin server.  In the long run, this functionality will be
 * provided by a nameserver.
 */

KRB5_DLLIMP int KRB5_CALLCONV
krb_get_admhst(h, r, n)
    char FAR *h;
    char FAR *r;
    int n;
{
    FILE *cnffile, *krb__get_cnffile();
    char tr[REALM_SZ];
    char linebuf[BUFSIZ];
    char scratch[64];
    register int i;

    cnffile = krb__get_cnffile();
    if (!cnffile)
            return(KFAILURE);
    if (fgets(linebuf, BUFSIZ, cnffile) == NULL) {
	/* error reading */
	(void) fclose(cnffile);
	return(KFAILURE);
    }
    if (!strchr(linebuf, '\n')) {
	/* didn't all fit into buffer, punt */
	(void) fclose(cnffile);
	return(KFAILURE);
    }
    for (i = 0; i < n; ) {
	/* run through the file, looking for admin host */
	if (fgets(linebuf, BUFSIZ, cnffile) == NULL) {
            (void) fclose(cnffile);
            return(KFAILURE);
        }
	/* need to scan for a token after 'admin' to make sure that
	   admin matched correctly */
	if (sscanf(linebuf, "%s %s admin %s", tr, h, scratch) != 3)
	    continue;
        if (!strcmp(tr,r))
            i++;
    }
    (void) fclose(cnffile);
    return(KSUCCESS);
}
