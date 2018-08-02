/*
 * Copyright (C) 2018  Christian Berger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"

#include "logic-heart.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <cmath>
#include <ctime>
#include <chrono>

int32_t main(int32_t argc, char **argv) {
  int32_t retCode{0};
  int32_t mission;
  bool readyState = false;
  bool missionSet = false;

  auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
  if ((0 == commandlineArguments.count("cid")) || (0 == commandlineArguments.count("verbose"))) {
    std::cout << "You didn't specify all command inputs" << std::endl;
  }

  const uint32_t ID{(commandlineArguments["id"].size() != 0) ? static_cast<uint32_t>(std::stoi(commandlineArguments["id"])) : 1};
  // const bool VERBOSE{(commandlineArguments["verbose"].size() != 0) ? commandlineArguments.count("verbose") != 0 : 1};
  const uint16_t cid{(commandlineArguments["cid"].size() != 0) ? static_cast<uint16_t>(std::stoi(commandlineArguments["cid"])) : (uint16_t) 48};
  const uint16_t cidBB{(commandlineArguments["cidBB"].size() != 0) ? static_cast<uint16_t>(std::stoi(commandlineArguments["cidBB"])) : (uint16_t) 219};
  //const float FREQ{std::stof(commandlineArguments["freq"])};

  std::cout << "Micro-Service ID:" << ID << std::endl;

  // Interface to a running OpenDaVINCI session.

  cluon::OD4Session od4{cid};
  cluon::OD4Session od4StateMachine{cidBB};

  std::cout << "Cid bb: " << cidBB << std::endl;
  std::cout << "Cid: " << cid << std::endl;

  Heart heart;

  auto catchState{[&mission, &readyState, &missionSet, &heart](cluon::data::Envelope &&envelope) {
    if (envelope.senderStamp() == 1406) {
      auto message = cluon::extractMessage<opendlv::proxy::SwitchStateReading>(std::move(envelope));
      int32_t tmpMission = message.state();
      if (mission!=tmpMission){
        missionSet = heart.setMission(tmpMission);
        mission = tmpMission;
      }
    }
    if (envelope.senderStamp() == 1401) {

      auto message = cluon::extractMessage<opendlv::proxy::SwitchStateReading>(std::move(envelope));
      if (message.state()==2) {
        readyState = true;
      }
      if (message.state() != 2) {
        readyState = false;
      }
    }
  }};

  od4StateMachine.dataTrigger(opendlv::proxy::SwitchStateReading::ID(), catchState);

  auto catchContainer{[&heart](cluon::data::Envelope &&envelope) {
    heart.nextContainer(envelope);
  }};

  od4.dataTrigger(opendlv::system::SignalStatusMessage::ID(), catchContainer);

  using namespace std::literals::chrono_literals;

  // Just sleep as this microservice is data driven.
  while (od4.isRunning()) {
    std::this_thread::sleep_for(0.05s);

    if (readyState && missionSet) {
      std::vector<int32_t> failedBeats = heart.body();
      if (failedBeats.empty()) {
        std::cout << "Heart beat success" << std::endl;
      } else {
        std::cout << "Heart beats failed" << std::endl;
      }
    }
  }

  return retCode;
}
