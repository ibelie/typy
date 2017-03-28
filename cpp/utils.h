// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_UTILS_H__
#define TYPY_UTILS_H__

#include "typy.h"

namespace typy {

class ScopedPyObjectPtr {
	public:
	// Constructor.  Defaults to initializing with NULL.
	// There is no way to create an uninitialized ScopedPyObjectPtr.
	explicit ScopedPyObjectPtr(PyObject* p = NULL) : ptr_(p) { }

	// Destructor.  If there is a PyObject object, delete it.
	~ScopedPyObjectPtr() {
		Py_XDECREF(ptr_);
	}

	// Reset.  Deletes the current owned object, if any.
	// Then takes ownership of a new object, if given.
	// This function must be called with a reference that you own.
	//   this->reset(this->get()) is wrong!
	//   this->reset(this->release()) is OK.
	PyObject* reset(PyObject* p = NULL) {
		Py_XDECREF(ptr_);
		ptr_ = p;
		return ptr_;
	}

	// Releases ownership of the object.
	// The caller now owns the returned reference.
	PyObject* release() {
		PyObject* p = ptr_;
		ptr_ = NULL;
		return p;
	}

	PyObject* operator->() const  {
		assert(ptr_ != NULL);
		return ptr_;
	}

	PyObject* get() const { return ptr_; }

	Py_ssize_t refcnt() const { return Py_REFCNT(ptr_); }

	void inc() const { Py_INCREF(ptr_); }

	// Comparison operators.
	// These return whether a ScopedPyObjectPtr and a raw pointer
	// refer to the same object, not just to two different but equal
	// objects.
	bool operator==(const PyObject* p) const { return ptr_ == p; }
	bool operator!=(const PyObject* p) const { return ptr_ != p; }

	private:
	PyObject* ptr_;

	GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(ScopedPyObjectPtr);
};

} // namespace typy
#endif // TYPY_UTILS_H__
