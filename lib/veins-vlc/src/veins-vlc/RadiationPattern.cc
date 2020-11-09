//
// Copyright (C) 2018 Julien Jahneke <julien.jahneke@ccs-labs.org>
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

#include "veins-vlc/RadiationPattern.h"
/**
   RadiationPattern::RadiationPattern(std::string m_id, std::vector<double> m_patternLeft, std::vector<double> m_patternRight, std::vector<double> m_anglesLeft, std::vector<double> m_anglesRight, std::vector<double> m_spectralEmission) {
    id = m_id;
    patternLeft = m_patternLeft;
    patternRight = m_patternRight;
    anglesLeft = m_anglesLeft;
    anglesRight = m_anglesRight;
    spectralEmission = m_spectralEmission;
   }

   RadiationPattern::~RadiationPattern(){}
 */
std::vector<double>& RadiationPattern::getSpectralEmission()
{
    return spectralEmission;
}

double RadiationPattern::getPatternLeftFromIndex(int index)
{
    return patternLeft[index];
}

double RadiationPattern::getPatternRightFromIndex(int index)
{
    return patternRight[index];
}

double RadiationPattern::getAnglesLeftFromIndex(int index)
{
    return anglesLeft[index];
}

double RadiationPattern::getAnglesRightFromIndex(int index)
{
    return anglesRight[index];
}
