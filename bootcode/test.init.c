#include "init.h"
#include <assert.h>
int main()
{
	ENV env;
	Initialize(&env);
	assert(env.pfnWSAStartup);
	assert(env.pfnWSASocketA);
	assert(env.pfnWSAConnect);
	assert(env.pfnWSARecv);
	return 0;
}
