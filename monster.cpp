/* Copyright (C) 2013-2014 Michal Brzozowski (rusolis@poczta.fm)

   This file is part of KeeperRL.

   KeeperRL is free software; you can redistribute it and/or modify it under the terms of the
   GNU General Public License as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   KeeperRL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.
   If not, see http://www.gnu.org/licenses/ . */

#include "stdafx.h"

#include "monster.h"

template <class Archive> 
void Monster::serialize(Archive& ar, const unsigned int version) {
  ar & SUBCLASS(Controller)
     & SVAR(actor)
     & SVAR(enemies);
  CHECK_SERIAL;
}

SERIALIZABLE(Monster);

SERIALIZATION_CONSTRUCTOR_IMPL(Monster);

Monster::Monster(Creature* c, MonsterAIFactory f) : Controller(c), actor(f.getMonsterAI(c)) {}

ControllerFactory Monster::getFactory(MonsterAIFactory f) {
  return ControllerFactory([=](Creature* c) { return new Monster(c, f);});
}

void Monster::makeMove() {
  actor->makeMove();
}

static string addName(const string& s, const string& n) {
  if (n.size() > 0)
    return s + " " + n;
  else
    return s;
}

bool Monster::isPlayer() const {
  return false;
}

const MapMemory& Monster::getMemory() const {
  return MapMemory::empty();
}

void Monster::onBump(Creature* c) {
  if (c->isEnemy(creature))
    c->attack(creature, Nothing(), false).perform();
  else if (auto action = c->move(creature->getPosition() - c->getPosition()))
    action.perform();
}

void Monster::you(MsgType type, const string& param) {
  string msg, msgNoSee;
  switch (type) {
    case MsgType::ARE: msg = creature->getName().the() + " is " + param; break;
    case MsgType::YOUR: msg = creature->getName().the() + "'s " + param; break;
    case MsgType::FEEL: msg = creature->getName().the() + " looks " + param; break;
    case MsgType::FALL_ASLEEP: msg = (creature->getName().the() + " falls asleep") + (param.size() > 0 ? " on the " + param : ".");
                               msgNoSee = "You hear snoring."; break;
    case MsgType::WAKE_UP: msg = creature->getName().the() + " wakes up."; break;
    case MsgType::DIE: msg = creature->getName().the() + " is killed!"; break;
    case MsgType::TELE_APPEAR: msg = creature->getName().the() + " appears out of nowhere!"; break;
    case MsgType::TELE_DISAPPEAR: msg = creature->getName().the() + " suddenly disappears!"; break;
    case MsgType::BLEEDING_STOPS: msg = creature->getName().the() + "'s bleeding stops."; break;
    case MsgType::DIE_OF: msg = creature->getName().the() +
                          " dies" + (param.empty() ? string(".") : " of " + param); break;
    case MsgType::FALL_APART: msg = creature->getName().the() + " falls apart."; break;
    case MsgType::MISS_ATTACK: msg = creature->getName().the() + addName(" misses", param); break;
    case MsgType::MISS_THROWN_ITEM: msg = param + " misses " + creature->getName().the(); break;
    case MsgType::MISS_THROWN_ITEM_PLURAL: msg = param + " miss " + creature->getName().the(); break;
    case MsgType::HIT_THROWN_ITEM: msg = param + " hits " + creature->getName().the(); break;
    case MsgType::HIT_THROWN_ITEM_PLURAL: msg = param + " hit " + creature->getName().the(); break;
    case MsgType::ITEM_CRASHES: msg = param + " crashes on " + creature->getName().the(); break;
    case MsgType::ITEM_CRASHES_PLURAL: msg = param + " crash on " + creature->getName().the(); break;
    case MsgType::GET_HIT_NODAMAGE: msg = "The " + param + " is harmless."; break;
    case MsgType::COLLAPSE: msg = creature->getName().the() + " collapses."; break;
    case MsgType::FALL: msg = creature->getName().the() + " falls on the " + param; break;
    case MsgType::TRIGGER_TRAP: msg = creature->getName().the() + " triggers something."; break;
    case MsgType::DISARM_TRAP: msg = creature->getName().the() + " disarms a trap."; break;
    case MsgType::PANIC: msg = creature->getName().the() + " panics."; break;
    case MsgType::RAGE: msg = creature->getName().the() + " is enraged."; break;
    case MsgType::SWING_WEAPON: msg = creature->getName().the() + " swings " + creature->getGender().his() + " " +
                                    param; break;
    case MsgType::THRUST_WEAPON: msg = creature->getName().the() + " thrusts " + creature->getGender().his() + " " +
                                 param; break;
    case MsgType::KICK: msg = creature->getName().the() + addName(" kicks", param); break;
    case MsgType::BITE: msg = creature->getName().the() + addName(" bites", param); break;
    case MsgType::PUNCH: msg = creature->getName().the() + addName(" punches", param); break;
    case MsgType::CRAWL: msg = creature->getName().the() + " is crawling"; break;
    case MsgType::STAND_UP: msg = creature->getName().the() + " is back on " + creature->getGender().his() + " feet ";
                            break;
    case MsgType::TURN_INVISIBLE: msg = creature->getName().the() + " disappears!"; break;
    case MsgType::TURN_VISIBLE: msg = creature->getName().the() + " appears out of nowhere!"; break;
    case MsgType::DROP_WEAPON: msg = creature->getName().the() + " drops " + creature->getGender().his() + " " + param;
                               break;
    case MsgType::ENTER_PORTAL: msg = creature->getName().the() + " disappears in the portal."; break;
    case MsgType::HAPPENS_TO: msg = param + " " + creature->getName().the(); break;
    case MsgType::BURN: msg = creature->getName().the() + " burns in the " + param; msgNoSee = "You hear a horrible shriek"; break;
    case MsgType::DROWN: msg = creature->getName().the() + " drowns in the " + param; msgNoSee = "You hear a loud splash" ;break;
    case MsgType::SET_UP_TRAP: msg = creature->getName().the() + " sets up the trap"; break;
    case MsgType::KILLED_BY: msg = creature->getName().the() + " is killed by " + param; break;
    case MsgType::TURN: msg = creature->getName().the() + " turns " + param; break;
    case MsgType::BECOME: msg = creature->getName().the() + " becomes " + param; break;
    case MsgType::COPULATE: msg = creature->getName().the() + " copulates " + param; break;
    case MsgType::CONSUME: msg = creature->getName().the() + " absorbs " + param; break;
    case MsgType::GROW: msg = creature->getName().the() + " grows " + param; break;
    case MsgType::BREAK_FREE: msg = creature->getName().the() + " breaks free from " + param; break;
    case MsgType::PRAY: msg = creature->getName().the() + " prays to " + param; break;
    case MsgType::SACRIFICE: msg = creature->getName().the() + " makes a sacrifice to " + param; break;
    case MsgType::HIT: msg = creature->getName().the() + addName(" hits", param); break;
    default: break;
  }
  if (!msg.empty())
    creature->monsterMessage(msg, msgNoSee);
}
  
void Monster::you(const string& param) {
  creature->monsterMessage(creature->getName().the() + " " + param);
}

