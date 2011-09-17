//
// TextEncoding.cpp
//
// $Id$
//
// Library: Foundation
// Package: Text
// Module:  TextEncoding
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/TextEncoding.h"
#include "Poco/Exception.h"
#include "Poco/String.h"
#include "Poco/ASCIIEncoding.h"
#include "Poco/Latin1Encoding.h"
#include "Poco/Latin9Encoding.h"
#include "Poco/UTF16Encoding.h"
#include "Poco/UTF8Encoding.h"
#include "Poco/Windows1252Encoding.h"
#include "Poco/RWLock.h"
#include "Poco/SingletonHolder.h"
#include <map>


namespace Poco {


//
// TextEncodingManager
//


class TextEncodingManager
{
public:
	TextEncodingManager()
	{
		TextEncoding::Ptr pUtf8Encoding(new UTF8Encoding);
		add(pUtf8Encoding, TextEncoding::GLOBAL); 

		add(new ASCIIEncoding);
		add(new Latin1Encoding);
		add(new Latin9Encoding);
		add(pUtf8Encoding);
		add(new UTF16Encoding);
		add(new Windows1252Encoding);
	}
	
	~TextEncodingManager()
	{
	}
	
	void add(TextEncoding::Ptr pEncoding)
	{
		add(pEncoding, pEncoding->canonicalName());
	}
	
	void add(TextEncoding::Ptr pEncoding, const std::string& name)
	{
		RWLock::ScopedLock lock(_lock, true);
	
		_encodings[name] = pEncoding;
	}
	
	void remove(const std::string& name)
	{
		RWLock::ScopedLock lock(_lock, true);
	
		_encodings.erase(name);
	}
	
	TextEncoding::Ptr find(const std::string& name) const
	{
		RWLock::ScopedLock lock(_lock);
		
		EncodingMap::const_iterator it = _encodings.find(name);
		if (it != _encodings.end())
			return it->second;
		
		for (it = _encodings.begin(); it != _encodings.end(); ++it)
		{
			if (it->second->isA(name))
				return it->second;
		}
		return TextEncoding::Ptr();
	}

private:
	TextEncodingManager(const TextEncodingManager&);
	TextEncodingManager& operator = (const TextEncodingManager&);
	
	struct ILT
	{
		bool operator() (const std::string& s1, const std::string& s2) const
		{
			return Poco::icompare(s1, s2) < 0;
		}
	};
	
	typedef std::map<std::string, TextEncoding::Ptr, ILT> EncodingMap;
	
	EncodingMap    _encodings;
	mutable RWLock _lock;
};


//
// TextEncoding
//


const std::string TextEncoding::GLOBAL;


TextEncoding::~TextEncoding()
{
}


int TextEncoding::convert(const unsigned char* bytes) const
{
	return (int) *bytes;
}


int TextEncoding::convert(int ch, unsigned char* bytes, int length) const
{
	return 0;
}


TextEncoding& TextEncoding::byName(const std::string& encodingName)
{
	TextEncoding* pEncoding = manager().find(encodingName);
	if (pEncoding)
		return *pEncoding;
	else
		throw NotFoundException(encodingName);
}

	
TextEncoding::Ptr TextEncoding::find(const std::string& encodingName)
{
	return manager().find(encodingName);
}


void TextEncoding::add(TextEncoding::Ptr pEncoding)
{
	manager().add(pEncoding, pEncoding->canonicalName());
}


void TextEncoding::add(TextEncoding::Ptr pEncoding, const std::string& name)
{
	manager().add(pEncoding, name);
}


void TextEncoding::remove(const std::string& encodingName)
{
	manager().remove(encodingName);
}


TextEncoding::Ptr TextEncoding::global(TextEncoding::Ptr encoding)
{
	TextEncoding::Ptr prev = find(GLOBAL);
	add(encoding, GLOBAL);
	return prev;
}


TextEncoding& TextEncoding::global()
{
	return byName(GLOBAL);
}


TextEncodingManager& TextEncoding::manager()
{
	static SingletonHolder<TextEncodingManager> sh;
	return *sh.get();
}


} // namespace Poco
