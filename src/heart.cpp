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
    
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ((0 == commandlineArguments.count("cid")) || (0 == commandlineArguments.count("verbose"))) {
      std::cout << "You didn't specify all command inputs" << std::endl;
    }

      const uint32_t ID{(commandlineArguments["id"].size() != 0) ? static_cast<uint32_t>(std::stoi(commandlineArguments["id"])) : 1};
      const bool VERBOSE{(commandlineArguments["verbose"].size() != 0) ? commandlineArguments.count("verbose") != 0 : 1};
      const uint16_t cid{(commandlineArguments["cid"].size() != 0) ? static_cast<uint16_t>(std::stoi(commandlineArguments["cid"])) : (uint16_t) 48};
      const uint16_t cidBB{(commandlineArguments["cidBB"].size() != 0) ? static_cast<uint16_t>(std::stoi(commandlineArguments["cidBB"])) : (uint16_t) 219};

      //const float FREQ{std::stof(commandlineArguments["freq"])};
      std::cout << "Micro-Service ID:" << ID << std::endl;

      // Interface to a running OpenDaVINCI session.

      cluon::OD4Session od4{cid};
      cluon::OD4Session od4StateMachine{cidSM};

      Heart heart(VERBOSE, ID, od4StateMachine);

      using namespace std::literals::chrono_literals;


      auto catchContainer{[&heart](cluon::data::Envelope &&envelope)
      {
        heart.nextContainer(envelope);
      }};

      od4.dataTrigger(opendlv::system::SignalStatusMessage::ID(), catchContainer);

      // Just sleep as this microservice is data driven.
      while (od4.isRunning()) {
        std::this_thread::sleep_for(0.05s);
        opendlv::system::SignalStatusMessage heartBeat;
        heartBeat.code(1);

        std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
        cluon::data::TimeStamp sampleTime = cluon::time::convert(tp);

        od4.send(heartBeat,sampleTime,313);
      }

  return retCode;
}
