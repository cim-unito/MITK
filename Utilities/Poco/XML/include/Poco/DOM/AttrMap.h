//
// AttrMap.h
//
// $Id$
//
// Library: XML
// Package: DOM
// Module:  DOM
//
// Definition of the AttrMap class.
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


#ifndef DOM_AttrMap_INCLUDED
#define DOM_AttrMap_INCLUDED


#include "Poco/XML/XML.h"
#include "Poco/DOM/NamedNodeMap.h"


namespace Poco {
namespace XML {


class Element;


class XML_API AttrMap: public NamedNodeMap
	// This implementation of NamedNodeMap is
	// returned by Element::attributes()
{
public:
	Node* getNamedItem(const XMLString& name) const;
	Node* setNamedItem(Node* arg);
	Node* removeNamedItem(const XMLString& name);
	Node* item(unsigned long index) const;
	unsigned long length() const;

	Node* getNamedItemNS(const XMLString& namespaceURI, const XMLString& localName) const;
	Node* setNamedItemNS(Node* arg);
	Node* removeNamedItemNS(const XMLString& namespaceURI, const XMLString& localName);

	void autoRelease();

protected:
	AttrMap(Element* pElement);
	~AttrMap();

private:
	AttrMap();

	Element* _pElement;
	
	friend class Element;
};


} } // namespace Poco::XML


#endif // DOM_AttrMap_INCLUDED
