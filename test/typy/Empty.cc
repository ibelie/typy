// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "Empty.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(Empty);

Empty::Empty() : Message() {
	PyObject_INIT(this, &Object<Empty>::_Type);
	ZERO(Empty, _cached_size, _cached_size);
}

void Empty::Clear() {
	
}

void Empty::MergeFrom(const Empty& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
	
}

void Empty::SerializeWithCachedSizes(CodedOutputStream* output) const {
	
}

int Empty::ByteSize() const {
	int total_size = 0;
	
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool Empty::MergePartialFromCodedStream(CodedInputStream* input) {
	input->ExpectAtEnd();
	return true;
}

// ===================================================================

const int Empty::PropertyCount = 0;
char* Empty::Properties[] = {
	""
};

int Empty::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
		case 0: break;
	}
	return size;
}

void Empty::SerializeProperty(CodedOutputStream* output, int tag) const {
	switch(tag) {
	case 0: break;
	}
}

int Empty::DeserializeProperty(CodedInputStream* input) {
	const void* data;
	int size;
	input->GetDirectBufferPointerInline(&data, &size);
	CodedInputStream tagInput(reinterpret_cast<const uint8*>(data), size);
	::std::pair<uint32, bool> p = tagInput.ReadTagWithCutoff(_MAXTAG(25)
		<= 0x7F ? 0x7F : (_MAXTAG(25) <= 0x3FFF ? 0x3FFF : _MAXTAG(25)));
	uint32 tag = p.first;
	if (!p.second) { return 0; }

	int index = WireFormatLite::GetTagFieldNumber(tag);
	switch(index) {
	case 0: break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

// ===================================================================



template <> PyGetSetDef Object<Empty>::GetSet[] = {
	
	{NULL}
};

TypyObjectEnd(Empty);
