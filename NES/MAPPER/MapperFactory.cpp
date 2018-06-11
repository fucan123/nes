#include "MAPPERFACTORY.h"

Mapper* CreateMapper(BYTE no) {
	switch (no)
	{
	case 0:
		return new Mapper0;
		break;
	case 1:
		return new Mapper1;
		break;
	case 2:
		return new Mapper2;
		break;
	default:
		break;
	}
	return NULL;
}