//
// NamedMutexTest.cpp
//
// $Id$
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


#include "NamedMutexTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/NamedMutex.h"
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/Timestamp.h"


using Poco::NamedMutex;
using Poco::Thread;
using Poco::Runnable;
using Poco::Timestamp;


static NamedMutex testMutex("TestMutex");


namespace
{
	class TestLock: public Runnable
	{
	public:
		void run()
		{
		
			testMutex.lock();
			_timestamp.update();
			testMutex.unlock();
		}

		const Timestamp& timestamp() const
		{
			return _timestamp;
		}

	private:
		Timestamp _timestamp;
	};

	class TestTryLock: public Runnable
	{
	public:
		TestTryLock(): _locked(false)
		{
		}
		
		void run()
		{
			if (testMutex.tryLock())
			{
				_locked = true;
				testMutex.unlock();
			}
		}

		bool locked() const
		{
			return _locked;
		}

	private:
		bool _locked;
	};
}


NamedMutexTest::NamedMutexTest(const std::string& name): CppUnit::TestCase(name)
{
}


NamedMutexTest::~NamedMutexTest()
{
}


void NamedMutexTest::testLock()
{
	testMutex.lock();
	Thread thr;
	TestLock tl;
	thr.start(tl);
	Timestamp now;
	Thread::sleep(2000);
	testMutex.unlock();
	thr.join();
	assert (tl.timestamp() > now);
}


void NamedMutexTest::testTryLock()
{
	Thread thr1;
	TestTryLock ttl1;
	thr1.start(ttl1);
	thr1.join();
	assert (ttl1.locked());
	
	testMutex.lock();
	Thread thr2;
	TestTryLock ttl2;
	thr2.start(ttl2);
	thr2.join();
	testMutex.unlock();
	assert (!ttl2.locked());
}


void NamedMutexTest::setUp()
{
}


void NamedMutexTest::tearDown()
{
}


CppUnit::Test* NamedMutexTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("NamedMutexTest");

	CppUnit_addTest(pSuite, NamedMutexTest, testLock);
	CppUnit_addTest(pSuite, NamedMutexTest, testTryLock);

	return pSuite;
}