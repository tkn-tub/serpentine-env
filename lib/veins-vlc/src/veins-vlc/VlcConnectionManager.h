//
// Copyright (C) 2017 Agon Memedi <memedi@ccs-labs.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "veins-vlc/veins-vlc.h"

#include "veins/base/connectionManager/BaseConnectionManager.h"

namespace veins {

/**
 * @brief BaseConnectionManager implementation which only defines a
 * specific max interference distance.
 *
 * Calculates the maximum interference distance based on the transmitter
 * power, wavelength, pathloss coefficient and a threshold for the
 * minimal receive Power.
 *
 * @ingroup connectionManager
 */
class VEINS_VLC_API VlcConnectionManager : public BaseConnectionManager {
protected:
    /**
     * @brief Calculate interference distance
     *
     * Calculation of the interference distance based on the transmitter
     * power, wavelength, pathloss coefficient and a threshold for the
     * minimal receive Power
     *
     * You may want to overwrite this function in order to do your own
     * interference calculation
     */
    virtual double calcInterfDist();
};

} // namespace veins
