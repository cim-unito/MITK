//
// TaskTest.cpp
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


#include "TaskTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Task.h"
#include "Poco/Thread.h"
#include "Poco/Event.h"
#include "Poco/AutoPtr.h"


using Poco::Task;
using Poco::Thread;
using Poco::Event;
using Poco::AutoPtr;


namespace
{
	class TestTask: public Task
	{
	public:
		TestTask(): Task("TestTask")
		{
		}
		
		void runTask()
		{
			_event.wait();
			if (sleep(10))
				return;
			setProgress(0.5);
			_event.wait();
			if (isCancelled())
				return;
			setProgress(1.0);
			_event.wait();
		}
		
		void cont()
		{
			_event.set();
		}
		
	private:
		Event _event;
	};
}


TaskTest::TaskTest(const std::string& name): CppUnit::TestCase(name)
{
}


TaskTest::~TaskTest()
{
}


void TaskTest::testFinish()
{
	AutoPtr<TestTask> pTT = new TestTask;
	assert (pTT->state() == Task::TASK_IDLE);
	Thread thr;
	thr.start(*pTT);
	assert (pTT->progress() == 0);
	pTT->cont();
	while (pTT->progress() != 0.5) Thread::sleep(50);
	assert (pTT->state() == Task::TASK_RUNNING);
	pTT->cont();
	while (pTT->progress() != 1.0) Thread::sleep(50);
	pTT->cont();
	thr.join();
	assert (pTT->state() == Task::TASK_FINISHED);
}


void TaskTest::testCancel1()
{
	AutoPtr<TestTask> pTT = new TestTask;
	assert (pTT->state() == Task::TASK_IDLE);
	Thread thr;
	thr.start(*pTT);
	assert (pTT->progress() == 0);
	pTT->cont();
	while (pTT->progress() != 0.5) Thread::sleep(50);
	assert (pTT->state() == Task::TASK_RUNNING);
	pTT->cancel();
	assert (pTT->state() == Task::TASK_CANCELLING);
	pTT->cont();
	thr.join();
	assert (pTT->state() == Task::TASK_FINISHED);
}


void TaskTest::testCancel2()
{
	AutoPtr<TestTask> pTT = new TestTask;
	assert (pTT->state() == Task::TASK_IDLE);
	Thread thr;
	thr.start(*pTT);
	assert (pTT->progress() == 0);
	pTT->cancel();
	assert (pTT->state() == Task::TASK_CANCELLING);
	pTT->cont();
	thr.join();
	assert (pTT->state() == Task::TASK_FINISHED);
}


void TaskTest::setUp()
{
}


void TaskTest::tearDown()
{
}


CppUnit::Test* TaskTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("TaskTest");

	CppUnit_addTest(pSuite, TaskTest, testFinish);
	CppUnit_addTest(pSuite, TaskTest, testCancel1);
	CppUnit_addTest(pSuite, TaskTest, testCancel2);

	return pSuite;
}
