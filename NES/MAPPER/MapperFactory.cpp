#include "MapperFactory.h"

Mapper* CreateMapper(BYTE no) {
	switch (no)
	{
	case 0:
		return new Mapper000;
		break;
	case 1:
		return new Mapper001;
		break;
	case 2:
		return new Mapper002;
		break;
	case 3:
		return new Mapper003;
		break;
	default:
		break;
	}
	return NULL;
}