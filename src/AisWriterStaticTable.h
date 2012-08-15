#ifndef AisWriterStaticTable_h
#define AisWriterStaticTable_h

#include <AisMessage.h>
#include <AisDebug.h>

class AisWriterStaticTable{
public:
	virtual bool isReady() = 0;
	virtual bool writeEntry(const AisMessage& message) = 0;
	virtual ~AisWriterStaticTable() = 0;
};
AisWriterStaticTable::~AisWriterStaticTable(){}

#endif
