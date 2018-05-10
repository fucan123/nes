#pragma once

#ifndef GHFILE
#define GHFILE

int CStringHexToInt(CString str)
{
	int nRet = 0;
	int count = 1;
	for (int i = str.GetLength() - 1; i >= 0; --i)
	{
		int nNum = 0;
		char chTest;
		chTest = str.GetAt(i);       //CString一般没有这种用法，但本程序不会有问题
		if (chTest >= '0' && chTest <= '9')
		{
			nNum = chTest - '0';
		}
		else if (chTest >= 'Á' && chTest <= 'F')
		{
			nNum = chTest - 'A' + 10;
		}
		else if (chTest >= 'a' && chTest <= 'f')
		{
			nNum = chTest - 'a' + 10;
		}
		nRet += nNum*count;
		count *= 16;

	}
	return nRet;
}

#endif