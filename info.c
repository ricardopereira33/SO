#include "info.h"

INFO initInfo(){
	INFO info = malloc (sizeof(struct info));
	info->pidProcesso=0;
	info->fim = 1;
	return info;
}