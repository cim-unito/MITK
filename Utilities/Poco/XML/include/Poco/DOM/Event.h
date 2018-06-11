//
// Event.h
//
// $Id$
//
// Library: XML
// Package: DOM
// Module:  DOMEvents
//
// Definition of the DOM Event class.
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


#ifndef DOM_Event_INCLUDED
#define DOM_Event_INCLUDED


#include "Poco/XML/XML.h"
#include "Poco/XML/XMLString.h"
#include "Poco/DOM/DOMObject.h"


namespace Poco {
namespace XML {


class EventTarget;
class Document;


class XML_API Event: public DOMObject
	/// The Event interface is used to provide contextual information about an event
	/// to the handler processing the event. An object which implements the Event
	/// interface is generally passed as the first parameter to an event handler.
	/// More specific context information is passed to event handlers by deriving
	/// additional interfaces from Event which contain information directly relating
	/// to the type of event they accompany. These derived interfaces are also implemented
	/// by the object passed to the event listener.
{
public:
	enum PhaseType
	{
		CAPTURING_PHASE = 1, /// The event is currently being evaluated at the target EventTarget. 
		AT_TARGET       = 2, /// The current event phase is the bubbling phase. 
		BUBBLING_PHASE  = 3  /// The current event phase is the capturing phase. 
	};

	const XMLString& type() const;
		/// The name of the event (case-insensitive). The name must be an XML name.

	EventTarget* target() const;
		/// Used to indicate the EventTarget to which the event was originally dispatched.

	EventTarget* currentTarget() const;
		/// Used to indicate the EventTarget whose EventListeners are currently being 
		/// processed. This is particularly useful during capturing and bubbling.

	PhaseType eventPhase() const;
		/// Used to indicate which phase of event flow is currently being evaluated.

	bool bubbles() const;
		/// Used to indicate whether or not an event is a bubbling event. 
		/// If the event can bubble the value is true, else the value is false.

	bool cancelable() const;
		/// Used to indicate whether or not an event can have its default action 
		/// prevented. If the default action can be prevented the value is
		/// true, else the value is false.

	Poco::UInt64 timeStamp() const;
		/// Used to specify the time (in milliseconds relative to the epoch) at 
		/// which the event was created. Due to the fact that some
		/// systems may not provide this information the value of timeStamp may 
		/// be not available for all events. When not available, a
		/// value of 0 will be returned. Examples of epoch time are the time of the 
		/// system start or 0:0:0 UTC 1st January 1970.
		/// This implementation always returns 0.

	void stopPropagation();
		/// The stopPropagation method is used prevent further propagation of an 
		/// event during event flow. If this method is called by
		/// any EventListener the event will cease propagating through the tree. 
		/// The event will complete dispatch to all listeners on the
		/// current EventTarget before event flow stops. This method may be used 
		/// during any stage of event flow. 

	void preventDefault();
		/// If an event is cancelable, the preventDefault method is used to signify 
		/// that the event is to be canceled, meaning any default
		/// action normally taken by the implementation as a result of 
		/// the event will not occur. If, during any stage of event flow, the
		/// preventDefault method is called the event is canceled. Any default 
		/// action associated with the event will not occur. Calling
		/// this method for a non-cancelable event has no effect. Once 
		/// preventDefault has been called it will remain in effect throughout
		/// the remainder of the event's propagation. This method may be 
		/// used during any stage of event flow. 

	void initEvent(const XMLString& eventType, bool canBubble, bool isCancelable);
		/// The initEvent method is used to initialize the value of an 
		/// Event created through the DocumentEvent interface. This method
		/// may only be called before the Event has been dispatched via the 
		/// dispatchEvent method, though it may be called multiple
		/// times during that phase if necessary. If called multiple 
		/// times the final invocation takes precedence. If called from 
		/// a subclass of Event interface only the values specified in the 
		/// initEvent method are modified, all other attributes are left unchanged. 

	void autoRelease();

protected:
	Event(Document* pOwnerDocument, const XMLString& type);
	Event(Document* pOwnerDocument, const XMLString& type, EventTarget* pTarget, bool canBubble, bool isCancelable);
	~Event();

	bool isCanceled() const;
		/// returns true if and only if the event has been cancelled.

	bool isStopped() const;
		/// returns true if and only if propagation of the event has been stopped.

	void setTarget(EventTarget* pTarget);
		/// sets the target

	void setCurrentPhase(PhaseType phase);
		/// sets the current phase

	void setCurrentTarget(EventTarget* pTarget);
		/// sets the current target

private:
	Document*    _pOwner;
	XMLString    _type;
	EventTarget* _pTarget;
	EventTarget* _pCurrentTarget;
	PhaseType    _currentPhase;
	bool         _bubbles;
	bool         _cancelable;
	bool         _canceled;
	bool         _stopped;
	
	friend class AbstractNode;
};


//
// inlines
//
inline const XMLString& Event::type() const
{
	return _type;
}


inline EventTarget* Event::target() const
{
	return _pTarget;
}


inline EventTarget* Event::currentTarget() const
{
	return _pCurrentTarget;
}


inline Event::PhaseType Event::eventPhase() const
{
	return _currentPhase;
}


inline bool Event::bubbles() const
{
	return _bubbles;
}


inline bool Event::cancelable() const
{
	return _cancelable;
}


inline Poco::UInt64 Event::timeStamp() const
{
	return 0;
}


inline bool Event::isCanceled() const
{
	return _canceled;
}


inline bool Event::isStopped() const
{
	return _stopped;
}


} } // namespace Poco::XML


#endif // DOM_Event_INCLUDED