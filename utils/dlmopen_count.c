/*
 * $PIP_license: <Simplified BSD License>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *     Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 * $
 * $RIKEN_copyright: Riken Center for Computational Sceience (R-CCS),
 * System Software Development Team, 2016-2021
 * $
 * $PIP_TESTSUITE: Version 1.1.0$
 *
 * $Author: Atsushi Hori (R-CCS) mailto: ahori@riken.jp or ahori@me.com
 * $
 */

#include <test.h>

#include <dlfcn.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX 1024

int
main(int argc, char **argv)
{
	void *so;
	char path[PATH_MAX], *p;
	int option_pip_mode = 1;
	int option_verbose  = 0;
	int c, n = 0, m = MAX;

	while ((c = getopt(argc, argv, "pvm:")) != -1) {
		switch (c) {
		case 'p':
			option_pip_mode = 1;
			break;
		case 'v':
			option_verbose = 1;
			break;
		case 'm':
		  	m = strtol( optarg, NULL, 10 );
			break;
		default:
			fprintf(stderr, "Usage: dlmopen_count [-p] [-v]\n");
			exit(2);
		}
	}
	p = path;
	p = stpcpy( p, CWD );
	p = stpcpy( p, "/" );
	p = stpcpy( p, basename( argv[0] ) );
	for (;;) {
		so = dlmopen(LM_ID_NEWLM, path, RTLD_NOW | RTLD_LOCAL);
		if (so == NULL) {
		  if( option_verbose ) {
		    fprintf( stderr, "dlmopen(): %s\n", dlerror() );
		  }
		  break;
		}
		if (++n >= MAX)
			break;
	}
	if( option_verbose ) {
	  fprintf( stderr, "DL_NNS=%d  PIP_NATSKS_MAX=%d\n",
		   n, PIP_NTASKS_MAX );
	}
	if( option_pip_mode && n > PIP_NTASKS_MAX )
		n = PIP_NTASKS_MAX;
	n = ( n > m ) ? m : n;
	printf("%d\n", n);

	if( !n ) return 1;

	return 0;
}
