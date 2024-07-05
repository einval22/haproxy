#ifndef _HAPROXY_LIMITS_H
#define _HAPROXY_LIMITS_H
#include <sys/resource.h>

/* Default connections limit.
 *
 * A system limit can be enforced at build time in order to avoid using haproxy
 * beyond reasonable system limits. For this, just define SYSTEM_MAXCONN to the
 * absolute limit accepted by the system. If the configuration specifies a
 * higher value, it will be capped to SYSTEM_MAXCONN and a warning will be
 * emitted. The only way to override this limit will be to set it via the
 * command-line '-n' argument. If SYSTEM_MAXCONN is not set, a minimum value
 * of 100 will be used for DEFAULT_MAXCONN which almost guarantees that a
 * process will correctly start in any situation.
 */
#ifdef SYSTEM_MAXCONN
#undef  DEFAULT_MAXCONN
#define DEFAULT_MAXCONN SYSTEM_MAXCONN
#elif !defined(DEFAULT_MAXCONN)
#define DEFAULT_MAXCONN 100
#endif

/* Default file descriptor limit.
 *
 * DEFAULT_MAXFD explicitly reduces the hard RLIMIT_NOFILE, which is used by the
 * process as the base value to calculate the default global.maxsock, if
 * global.maxconn, global.rlimit_memmax are not defined. This is useful in the
 * case, when hard nofile limit has been bumped to fs.nr_open (kernel max),
 * which is extremely large on many modern distros. So, we will also finish with
 * an extremely large default global.maxsock. The only way to override
 * DEFAULT_MAXFD, if defined, is to set fd_hard_limit in the config global
 * section. If DEFAULT_MAXFD is not set, a reasonable maximum of 1048576 will be
 * used as the default value, which almost guarantees that a process will
 * correctly start in any situation and will be not killed then by watchdog,
 * when it will loop over the allocated fdtab.
*/
#ifndef DEFAULT_MAXFD
#define DEFAULT_MAXFD 1048576
#endif

/* Define a maxconn which will be used in the master process once it re-exec to
 * the MODE_MWORKER_WAIT and won't change when SYSTEM_MAXCONN is set.
 *
 * 100 must be enough for the master since it only does communication between
 * the master and the workers, and the master CLI.
 */
#ifndef MASTER_MAXCONN
#define MASTER_MAXCONN 100
#endif

/* available memory estimate : count about 3% of overhead in various structures */
#ifndef MEM_USABLE_RATIO
#define MEM_USABLE_RATIO 0.97
#endif

/* if not 0, maximum allocatable memory per process in MB */
#ifndef HAPROXY_MEMMAX
#define HAPROXY_MEMMAX 0
#endif

struct rlimit;

/* handlers to compute internal process limits, so called ha_limits, if they are
 * not provided via cmd line or via configuration file
*/
int compute_ideal_maxpipes(struct proxy *proxies_list);
int compute_ideal_maxconn(struct rlimit *nofile_lim_at_boot,
						  struct proxy *proxies_list);
int compute_ideal_maxsock(int maxconn);

/* handlers to manipulate system resources limits provided by OS to process and
 * to tie them up with ha_limits */

int raise_rlim_nofile(struct rlimit *old_limit, struct rlimit *new_limit);

int check_if_maxsock_permitted(int maxsock);

#endif /* _HAPROXY_LIMITS_H */
