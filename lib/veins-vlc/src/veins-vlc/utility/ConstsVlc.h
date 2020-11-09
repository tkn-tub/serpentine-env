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

namespace veins {

/*
 * Reference power used for the Empirical Light Model
 */
const int FIXED_REFERENCE_POWER_MW = 10; // unit mW; During empirical measurements 19W (?)

/*
 * Consts for distinguishing the heading of the sending module
 * in the Empirical Light Model. Values are chosen such that
 * they comply with the geometric calculations. Not only as
 * identifiers
 */
const int HEAD = 1;
const int TAIL = -1;

/*
 * Consts for distinguishing between the left and right headlight
 * module in the Hella Light Model. Values solely serve as
 * identifiers
 */
const int LEFT = 1;
const int RIGHT = -1;

/*
 * End-to-end span of a light module relative to the x-axis.
 * See: "Characterizing link asymmetry in vehicle-to-vehicle
 * Visible Light Communications", Fig 6
 */
const int HEAD_MAX_X_SPAN = 50;
const int TAIL_MAX_X_SPAN = 20;

/*
 * @brief Bitlength of the Phy PREAMBLE, i.e., sync header (SHR)
 * see: IEEE Std 802.15.7-2011, Section 4.4.1.1, Section 8.6, Fig 124
 *
 */
const double PHY_VLC_SHR = 124; // bits; Minimum allowed length

/*
 * @brief Bitlength of the phy header.
 * It consists of PHR, HCS and optional fields.
 * see: IEEE Std 802.15.7-2011, Section 8.6, Fig 124
 */
const double PHY_VLC_HEADER = 32 + 26 + 0; // bits; Minimum possible length

// XXX: the values below are not used
/*
 * @brief Bitlength of the Phy Service Data Unit (PSDU)
 * It is specified between 0 - `aMaxPHYFrameSize` where aMaxPHyFrameSize
 * is given as 1023 for PHY I, and 65535 for PHY II, III in IEEE Std 802.15.7-2011,
 *
 * If MHR is modeled in the MAC, its size should be substracted from PSDU, because
 * the PSDU includes the MHR in the standard.
 */
const double PHY_VLC_PSDU = 1023; // bits; Minimum possible length based on PHYs

/*
 * @brief The Mac header (MHR).
 * According to Figure 44 from IEEE Std 802.15.7-2011 the value
 * ranges between [3, 34] octets. Here we choose 21 arbitrarily
 */
const double PHY_VLC_MHR = 21 * 8; // 168

// SHR, HEADER and PSDU form the Phy layer data unit (PPDU)

} // namespace veins
