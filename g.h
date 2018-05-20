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

CString* explode(wchar_t* separator, CString str, int* count) {
	int len = wcslen(separator);
	*count = 1;
	for (int i = 0; i < str.GetLength(); i++) {
		int pos = str.Find(separator, i);
		if (pos == -1) {
			break;
		}
		(*count)++;
		i += pos + len;
	}

	int index = 0, slen = *count;
	CString* arr = new CString[slen];
	for (int i = 0, start = 0; i < slen; i++) {
		int pos = str.Find(separator, start);
		CString ts;
		ts.Format(L"start:%d, pos:%d", start, pos);
		//::MessageBox(NULL, ts, L"t", MB_OK);
		if (pos > -1) {
			arr[i] = str.Mid(start, pos - start);
			start = pos + len;
		}
		else {
			arr[i] = str.Mid(start, str.GetLength() - start);
			break;
		}
	}
	return arr;
}

#endif