/**
* Copyright (C) 2017 Chalmers Revere
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-
1301,
* USA.
*/

#include <iostream>
#include "cluon-complete.hpp"
#include "logic-heart.hpp"



Heart::Heart(int32_t mission)
  : m_activeList()
{
  setUp(mission);
}

Heart::~Heart()
{
  tearDown();
}


void Heart::nextContainer(cluon::data::Envelope &a_container)
{
    if (a_container.dataType() == opendlv::system::SignalStatusMessage::ID()) {
      auto  heartBeat = cluon::extractMessage<opendlv::logic::action::AimPoint>(std::move(a_container));
    }
}

void Heart::body() {
  // bool allOk = true;
  // cluon::data::TimeStamp currentTime = cluon::time::now();
}

/*
  UKF - 114         acceleration, track
  Attention - 116   acceleration, track
  detectcone - 118  acceleration, track
  SLAM - 120        acceleration, track
  detectconelane - 211  acceleration, track
  track - 221
  acceleration - 222  acceleration,
  skidpad - 223
  (braketest) - 224
  vehiclemodel - 232
  motion - 313      acceleration, track
  steering - 314    acceleration, track
  brakes - 315      acceleration, track
  technical-inspection - 316, inspection,
*/

void Heart::setUp(int32_t mission)
{
  std::cout << "Setting up heart" << std::endl;
  if (mission == 1) {
    // acceleration
    m_activeList = {114, 116, 118, 120, 211, 222, 313, 314, 315};
  }
  if (mission == 2) {
    // braketest
    m_activeList = {};
  }
  if (mission == 3) {
    // inspection
    m_activeList = {316};
  }
  if (mission == 4) {
    // safety check
    m_activeList = {};
  }
  if (mission == 5) {
    // skidpad
    m_activeList = {};
  }
  if (mission == 6) {
    // track drive
    m_activeList = {114, 116, 118, 120, 211, 313, 314, 315};
  }
}

void Heart::tearDown()
{
}
