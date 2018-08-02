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



Heart::Heart()
  : m_activeList()
  , m_listMutex()
{
  setUp();
}

Heart::~Heart()
{
  tearDown();
}


void Heart::nextContainer(cluon::data::Envelope &a_container)
{
    if (a_container.dataType() == opendlv::system::SignalStatusMessage::ID()) {
      std::lock_guard<std::mutex> listLock(m_listMutex);

      auto  heartBeat = cluon::extractMessage<opendlv::logic::action::AimPoint>(std::move(a_container));
      int32_t senderStamp = a_container.senderStamp();
      cluon::data::TimeStamp currentTime = cluon::time::now();
      m_activeList[senderStamp] = cluon::time::toMicroseconds(currentTime);
    }
}

std::vector<int32_t> Heart::body() {
  std::map<int32_t, int32_t>::iterator it = m_activeList.begin();
  std::vector<int32_t> failedBeats;

  std::lock_guard<std::mutex> listLock(m_listMutex);
  cluon::data::TimeStamp currentTime = cluon::time::now();
  while(it != m_activeList.end())
  {
      int32_t diff = cluon::time::toMicroseconds(currentTime) - (it->second);
      if (diff > 200000){
        failedBeats.push_back(it->first);
        std::cout << "Module [" << failedBeats.back() << "] failed to beat. Time since last message: " << diff << std::endl;
      }
      it++;
  }
  return failedBeats;
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

void Heart::setUp()
{
  std::cout << "Setting up heart" << std::endl;

}

void Heart::tearDown()
{
}

bool Heart::setMission(int32_t mission) {
  if (mission == 7) {
    // acceleration
    m_activeList[114] = 0;
    m_activeList[116] = 0;
    m_activeList[118] = 0;
    m_activeList[120] = 0;
    m_activeList[211] = 0;
    m_activeList[222] = 0;
    m_activeList[313] = 0;
    m_activeList[314] = 0;
    m_activeList[315] = 0;
    }
  if (mission == 2) {
    // braketest
    m_activeList = {};
  }
  if (mission == 3) {
    // inspection
    m_activeList[316] = 0;
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
    m_activeList[114] = 0;
    m_activeList[116] = 0;
    m_activeList[118] = 0;
    m_activeList[120] = 0;
    m_activeList[211] = 0;
    m_activeList[313] = 0;
    m_activeList[314] = 0;
    m_activeList[315] = 0;
  }
  if (mission > 0){
    return true;
  } else {
    return false;
  }
}
