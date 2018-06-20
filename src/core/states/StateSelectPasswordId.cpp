/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateSelectPasswordId.h"


using namespace governikus;


StateSelectPasswordId::StateSelectPasswordId(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateSelectPasswordId::run()
{
	const bool canAllowed = getContext()->isCanAllowedMode();
	qDebug() << "CAN allowed:" << canAllowed;
	if (canAllowed)
	{
		Q_EMIT firePasswordIdCAN();
		return;
	}

	Q_EMIT fireContinue();
}
