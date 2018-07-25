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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

 #ifndef HEART
 #define HEART

 #include "opendlv-standard-message-set.hpp"


class Heart {
 public:
  Heart();
  ~Heart();
  void nextContainer(cluon::data::Envelope &);
  bool body();
  bool setMission(int32_t);

 private:
  void setUp();
  void tearDown();

 private:
   std::map<int32_t, int32_t> m_activeList;
   std::mutex m_listMutex;
   
};
#endif
