#ifndef AisWriter_h
#define AisWriter_h

#include <AisMessage.h>
#include <AisDebug.h>

class AisWriter{
public:
	virtual bool isReady() = 0;
	virtual bool writeEntry(const AisMessage& message) = 0;
	virtual ~AisWriter() = 0;
};
AisWriter::~AisWriter(){}

#endif
