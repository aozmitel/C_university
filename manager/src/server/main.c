#include <signal.h>
#include <sys/resource.h>
#include "main.h"
#include "getopt1.h"
#include "server.h"

struct programStatus cfg;

void daemonize(void);
void initProgram(int, char **, char **);

void mySignal(int);

int main (int argc, char ** argv, char ** envp)
{
	initProgram(argc, argv, envp);

	for(;;)
		sleep(10);

	endWork(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

void initProgram(int argc, char ** argv, char ** envp)
{
	cfg.argc = argc;
	cfg.argv = argv;
	cfg.envp = envp;
	cfg.help = 0;
	cfg.debug = 0;
	cfg.daemon = 0;
	cfg.version = 0;
	cfg.syslog = 0;

	cfg.port = 0;
	cfg.passkey = NULL;

	initOptions();

	printDebug();
	printHelp();
	printVersion();

	signal(SIGINT, mySignal);

	daemonize();
	initLogging();

	initServer();
}

void daemonize()
{
	int fd;
	struct rlimit flim;

	if (!cfg.daemon)
		return;

	if (getppid() != 1)
	{
		signal(SIGTTOU, SIG_IGN);
		signal(SIGTTIN, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);

		if (fork() != 0)
			exit(EXIT_SUCCESS);

		setsid();
	}

	getrlimit(RLIMIT_NOFILE, &flim);
	for (fd = 0; fd < flim.rlim_max; fd++)
		close(fd);

	chdir("/");
}


void endWork(int status)
{
	shutdownServer();
	if (status == EXIT_SUCCESS)
		info("Server is switching off. Thanks for using!\n");
	else
		error("Server made fatal error. Aborting by problem.\n");
	exit(status);
}

void mySignal(int sig)
{
	switch (sig)
	{
		case SIGINT:
			info("Server got SIGINT.\n");
			endWork(EXIT_SUCCESS);
			break;
	}
}
