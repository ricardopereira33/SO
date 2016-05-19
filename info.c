#include "info.h"

INFO initInfo(){
	INFO info = malloc (sizeof(struct info));
	return info;
}

INFO_PIPE initInfoPipe(){
	INFO_PIPE info = malloc(sizeof(struct info));
	return info;
}