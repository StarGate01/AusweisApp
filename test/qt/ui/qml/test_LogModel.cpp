/*!
 * \brief Unit tests for \ref LogModel
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;

class test_LogModel
	: public QObject
{
	Q_OBJECT
	QSharedPointer<LogModel> mModel;

	private Q_SLOTS:
		void init()
		{
			mModel.reset(new LogModel());
			// The test only covers the LogModel functionality, LogHandler will return an
			// error message to the LogModel if not properly initialised; discard that.
			mModel->mLogEntries.clear();
		}


		void cleanup()
		{
			mModel.clear();
		}


		void test_AddLogEntry_data()
		{
			QTest::addColumn<QString>("input");
			QTest::addColumn<QString>("origin");
			QTest::addColumn<QString>("message");

			QTest::newRow("validEntry") << QString("input : test") << QString("input") << QString("test");
			QTest::newRow("empty") << QString(" : ") << QString() << QString();
			QTest::newRow("leftEmpty") << QString(" : test") << QString() << QString("test");
			QTest::newRow("rightEmpty") << QString("input : ") << QString("input") << QString();
			QTest::newRow("NoSpaceRight") << QString("input :test") << QString("input :test") << QString();
			QTest::newRow("NoSpaceLeft") << QString("input: test") << QString("input: test") << QString();
			QTest::newRow("invalidEntry") << QString("inputTest") << QString("inputTest") << QString();
			QTest::newRow("NoSpaces") << QString("input:test") << QString("input:test") << QString();
			QTest::newRow("emptyString") << QString() << QString() << QString();
			QTest::newRow("::") << QString("::") << QString("::") << QString();
			QTest::newRow("a : b : c") << QString("a : b : c") << QString("a") << QString("b : c");
		}


		void test_AddLogEntry()
		{
			QFETCH(QString, input);
			QFETCH(QString, origin);
			QFETCH(QString, message);

			mModel->addLogEntry(input);
			QCOMPARE(mModel->mLogEntries.at(0), input);
			const QModelIndex index = mModel->index(0);
			QCOMPARE(mModel->data(index, LogModel::OriginRole), origin);
			QCOMPARE(mModel->data(index, LogModel::MessageRole), message);
			QCOMPARE(mModel->data(index, Qt::DisplayRole), input);
		}


		void test_SetLogEntries_data()
		{
			QTest::addColumn<QString>("fileName");
			QTest::addColumn<int>("logEntriesSize");

			QTest::newRow("empty") << QString(":/logfiles/empty.txt") << 0;
			QTest::newRow("size1") << QString(":/logfiles/size1.txt") << 1;
			QTest::newRow("size42") << QString(":/logfiles/size42.txt") << 42;
		}


		void test_SetLogEntries()
		{
			QFETCH(QString, fileName);
			QFETCH(int, logEntriesSize);

			QFile file(fileName);
			file.open(QIODevice::ReadOnly);
			QTextStream stream(&file);

			mModel->setLogEntries(stream);
			QCOMPARE(mModel->mLogEntries.size(), logEntriesSize);
		}


		void test_OnNewLogMsg_data()
		{
			QTest::addColumn<QString>("msg");
			QTest::addColumn<QString>("fileName");
			QTest::addColumn<int>("selectedFile");
			QTest::addColumn<int>("newLogMsgCounter");
			QTest::addColumn<int>("logEntriesSizeChange");

			QTest::newRow("emptyFile_MsgAdded") << QString("test : input") << QString(":/logfiles/empty.txt") << 0 << 1 << 1;
			QTest::newRow("emptyFile_MsgNotAdded") << QString(" : ") << QString(":/logfiles/empty.txt") << 1 << 0 << 0;

			QTest::newRow("size1_MsgAdded") << QString("test : input") << QString(":/logfiles/size1.txt") << 0 << 1 << 1;
			QTest::newRow("size1_MsgNotAdded") << QString(" : ") << QString(":/logfiles/size1.txt") << 1 << 0 << 0;
		}


		void test_OnNewLogMsg()
		{
			QFETCH(QString, msg);
			QFETCH(QString, fileName);
			QFETCH(int, selectedFile);
			QFETCH(int, newLogMsgCounter);
			QFETCH(int, logEntriesSizeChange);

			QFile file(fileName);
			file.open(QIODevice::ReadOnly);
			QTextStream stream(&file);
			mModel->setLogEntries(stream);
			int oldSize = mModel->mLogEntries.size();
			mModel->mSelectedLogFile = selectedFile;
			QSignalSpy spyNewLogMsg(mModel.data(), &LogModel::fireNewLogMsg);

			mModel->onNewLogMsg(msg);
			QCOMPARE(spyNewLogMsg.count(), newLogMsgCounter);
			QCOMPARE(mModel->mLogEntries.size(), oldSize + logEntriesSizeChange);
		}


};

QTEST_GUILESS_MAIN(test_LogModel)
#include "test_LogModel.moc"
