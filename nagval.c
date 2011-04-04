#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <getopt.h>
#include <resolv.h>

#include <arpa/nameser.h>
#include <validator/resolver.h>
#include <validator/validator.h>

#define DNSVALCONF	"dnsval.conf"
#define RESOLVCONF	"resolv.conf"
#define ROOTHINTS	"root.hints"

#define STATUS_OK	0
#define STATUS_WARN	1
#define STATUS_CRIT	2
#define STATUS_UNKNOWN	3

#define BUFLEN		512

int query(val_context_t *ctx, char *domain, u_int16_t type, val_status_t *status)
{
	int ret, flags = VAL_QUERY_NO_AC_DETAIL;
#if 0
	struct val_result_chain *results, *vc;

	ret = val_resolve_and_check(ctx, domain, ns_c_in, type, flags, &results);
	if (ret != VAL_NO_ERROR) {
		return ret;
	}

	*status = 0;

	for (vc = results; vc; vc = vc->val_rc_next) {
			*status = vc->val_rc_status;

		/* I think first result suffices */
		break;
	}
	
	val_free_result_chain(results);
#endif
	struct val_answer_chain *answers, *ac;

	flags = 0;
	ret = val_get_rrset(ctx, domain, ns_c_in, type, flags, &answers);
	if (ret != VAL_NO_ERROR) {
		return ret;
	}

	*status = 0;
	for (ac = answers; ac; ac = ac->val_ans_next) {
		*status = ac->val_ans_status;

		/* I think first result suffices */
		break;
	}

	val_free_answer_chain(answers);

	return ret;
}

u_int16_t fixtype(char *type_s)
{
	u_int16_t type = ns_t_any;
	int success;
	char *p;

	type = res_nametotype(type_s, &success);
	if (!success) {
		fprintf(stderr, "Unrecognized type %s\n", type_s);
		return STATUS_UNKNOWN;
	}

	/* convert string to uppercase for output */
	for (p = type_s; p && *p; p++) {
		*p = (islower(*p)) ? toupper(*p) : *p;
	}

	return (type);
}

void usage(char *prog)
{
	fprintf(stderr, "Usage: %s [-f file] [domain type]\n", prog);
	exit(STATUS_UNKNOWN);
}

int main(int argc, char *argv[])
{
	char *domain, *type_s, *infile = NULL, *progname = *argv;
	u_int16_t type = ns_t_a;
	int c, ret, rc = STATUS_WARN;
	val_context_t *ctx;
	val_status_t status;
	FILE *fp;

	while ((c = getopt(argc, argv, "f:")) != EOF) {
		switch (c) {
			case 'f':
				infile = strdup(optarg);
				break;
			default:
				usage(progname);
				break;
		}
	}

	argc -= optind;
	argv += optind;

	if (!infile && argc != 2) {
		usage(progname);
	}

	/* create context for validator; note this routine FIRST loads
	 * defaults before using our files */

	ret = val_create_context_with_conf("nagval",
		DNSVALCONF,
		RESOLVCONF,
		ROOTHINTS,
		&ctx);
	if (ret != VAL_NO_ERROR) {
		printf("Cannot create context: %s\n", p_val_error(ret));
		exit(STATUS_UNKNOWN);
	}

	if (!infile) {
		domain = argv[0];

		type_s = strdup(argv[1]);
		type = fixtype(type_s);

		ret = query(ctx, domain, type, &status);
		if (ret != VAL_NO_ERROR) {
			printf("val_resolve_and_check returns %s\n", p_val_error(ret));
			exit(STATUS_UNKNOWN);
		}

		printf("%s/%s: %s\n", 
			domain,
			type_s,
			p_val_status(status) + strlen("VAL_"));

		rc = (status == VAL_SUCCESS) ? STATUS_OK : STATUS_CRIT;
	} else {
		char buf[BUFLEN];

		if ((fp = fopen(infile, "r")) == NULL) {
			perror(infile);
			exit(STATUS_UNKNOWN);
		}
		while (fgets(buf, BUFLEN - 1, fp) != NULL) {
			if (buf[strlen(buf) - 1] == '\n')
				buf[strlen(buf) - 1] = 0;

			if ((domain = strtok(buf, "/")) == NULL) {
				fprintf(stderr, "No domain on line\n");
				continue;
			}
			if (*(buf + strlen(domain) + 1) == 0) {
				type_s = strdup("SOA");
			} else {
				type_s = strdup(buf + strlen(domain) + 1);
			}

			type = fixtype(type_s);

			ret = query(ctx, domain, type, &status);
			if (ret != VAL_NO_ERROR) {
				printf("val_resolve_and_check returns %s\n", p_val_error(ret));
				exit(STATUS_UNKNOWN);
			}

			printf("%s/%s: %s\n", 
				domain,
				type_s,
				p_val_status(status) + strlen("VAL_"));

			free(type_s);

		}
		fclose(fp);
	}
	return (rc);
}
