#include "MapperFactory.h"

Mapper* CreateMapper(BYTE no, NES* p) {
	switch (no)
	{
	case 0:
		return new Mapper000(p);
		break;
	case 1:
		return new Mapper001(p);
		break;
	case 2:
		return new Mapper002(p);
		break;
	case 3:
		return new Mapper003(p);
		break;
	default:
		break;
	}
	return NULL;
}