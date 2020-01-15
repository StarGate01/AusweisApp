/*!
 * \brief Unit tests for \ref StateWriteHistory
 *
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateWriteHistory.h"

#include "AppSettings.h"
#include "Env.h"
#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"
#include "TestAuthContext.h"

#include <QByteArrayList>
#include <QtTest>


using namespace governikus;

class test_StateWriteHistory
	: public QObject
{
	Q_OBJECT
	QSharedPointer<TestAuthContext> mContext;
	QSharedPointer<StateWriteHistory> mState;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1_2.xml"));
			mState.reset(StateBuilder::createState<StateWriteHistory>(mContext));
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mContext.clear();
			mState.clear();
		}


		void test_RunHistoryDisabled()
		{
			QSignalSpy spyContinue(mState.data(), &StateWriteHistory::fireContinue);
			Env::getSingleton<AppSettings>()->getHistorySettings().setEnabled(false);

			QTest::ignoreMessage(QtDebugMsg, "History disabled");
			mContext->setStateApproved();
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_RunNoEffectiveAccessRights()
		{
			QSignalSpy spyAbort(mState.data(), &StateWriteHistory::fireAbort);
			Env::getSingleton<AppSettings>()->getHistorySettings().setEnabled(true);
			mContext->setEffectiveAccessRights(QSet<AccessRight>());

			QTest::ignoreMessage(QtWarningMsg, "No EAC1 structure or effective CHAT in model.");
			mContext->setStateApproved();
			QCOMPARE(spyAbort.count(), 1);
		}


		void test_RunNoError()
		{
			auto& historySettings = Env::getSingleton<AppSettings>()->getHistorySettings();
			QSignalSpy spyContinue(mState.data(), &StateWriteHistory::fireContinue);
			historySettings.setEnabled(true);
			const QSet<AccessRight> rights = {AccessRight::READ_DG01, AccessRight::READ_DG02};
			mContext->setEffectiveAccessRights(rights);

			mContext->setStatus(GlobalStatus::Code::No_Error);
			mContext->setStateApproved();
			const auto historyInfo = historySettings.getHistoryInfos().at(0);
			QCOMPARE(historyInfo.getSubjectName(), mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getSubjectName());
			QCOMPARE(historyInfo.getSubjectUrl(), mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getSubjectUrl());
			QCOMPARE(historyInfo.getPurpose(), mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getPurpose());
			QVERIFY(historyInfo.getTermOfUsage().contains(mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getTermsOfUsage()));
			qDebug() << historyInfo.getRequestedData();
			QStringList list = {"DocumentType", "IssuingCountry"};
			QCOMPARE(historyInfo.getRequestedData(), list);
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateWriteHistory)
#include "test_StateWriteHistory.moc"
