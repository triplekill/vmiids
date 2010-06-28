/*
 * vmistop.cpp
 *
 *  Created on: Jun 28, 2010
 *      Author: kittel
 */

#include "vmiids_rpc.h"

int
main (int argc, char *argv[])
{
	const char *host;

	//if (argc < 2) {
	//	printf ("usage: %s server_host\n", argv[0]);
	//	exit (1);
	//}
	host = "localhost"; //argv[1];

	CLIENT *clnt;
	enum clnt_stat retval;
	int result;

	clnt = clnt_create (host, SIMP_PROG, SIMP_VERSION, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	retval = stopids_1(&result, clnt);
	if (retval != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}

	clnt_destroy (clnt);

	exit (0);
}
