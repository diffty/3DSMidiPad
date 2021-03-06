#include "linked_list.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

class CSVReader
{
private:
	LinkedList m_llFields;
	LinkedList m_llData;

	char* m_szFilename;

	int m_iNbFields;
	int m_iNbDataRows;

public:
	CSVReader(char* szFilename);
	~CSVReader();

	void destroyData();
	void readFile();
	void decodeBuffer(char* szBuffer);
	char* getData(const char* szFieldName, int iRowNum);
	int getFieldId(const char* szFieldName);
	void printContent();
	int countDataRows();
	int countFields();
	void fillArrayWithData(char*** pArrayToFill);
	void fillArrayWithDataPtr(char*** pArrayToFill);

	static void spliceString(const char* szSrcString, int iStartPos, int iEndPos, char* pSzDstString);
	static void convertReturnCharacters(char* szSrcString);
};
