/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QtGlobal>

namespace governikus
{

class DpiCalculator
{
	private:
		DpiCalculator()
		{
		}


		static qreal getDpi();

	public:
		static qreal getDpiScale()
		{
			return getDpi() / 160;
		}


};

} // namespace governikus
