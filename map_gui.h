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

#pragma once

#include "util.h"
#include "gui_elem.h"
#include "view_id.h"
#include "unique_entity.h"
#include "view_index.h"
#include "entity_map.h"
#include "view_object.h"

class MapMemory;
class MapLayout;
class Renderer;
class CreatureView;
class Clock;
class Creature;
class Options;
class TutorialInfo;

class MapGui : public GuiElem {
  public:
  struct Callbacks {
    function<void(Vec2)> continuousLeftClickFun;
    function<void(Vec2)> leftClickFun;
    function<void(Vec2)> rightClickFun;
    function<void(UniqueEntity<Creature>::Id)> creatureClickFun;
    function<void()> refreshFun;
    function<void(UniqueEntity<Creature>::Id, ViewId, Vec2)> creatureDragFun;
    function<void(UniqueEntity<Creature>::Id, Vec2)> creatureDroppedFun;
    function<void(TeamId, Vec2)> teamDroppedFun;
  };
  MapGui(Callbacks, Clock*, Options*, GuiFactory*);

  virtual void render(Renderer&) override;
  virtual bool onLeftClick(Vec2) override;
  virtual bool onRightClick(Vec2) override;
  virtual bool onMouseMove(Vec2) override;
  virtual void onMouseGone() override;
  virtual void onMouseRelease(Vec2) override;
  virtual bool onKeyPressed2(SDL::SDL_Keysym) override;

  void updateObjects(CreatureView*, MapLayout*, bool smoothMovement, bool mouseUI, const optional<TutorialInfo>&);
  void setSpriteMode(bool);
  optional<Vec2> getHighlightedTile(Renderer& renderer);
  void addAnimation(PAnimation animation, Vec2 position);
  void setCenter(double x, double y);
  void setCenter(Vec2 pos);
  void clearCenter();
  void resetScrolling();
  bool isCentered() const;
  Vec2 getScreenPos() const;
  optional<Vec2> projectOnMap(Vec2 screenCoord);
  void highlightTeam(const vector<UniqueEntity<Creature>::Id>&);
  void unhighlightTeam(const vector<UniqueEntity<Creature>::Id>&);
  void setButtonViewId(ViewId);
  void clearButtonViewId();
  bool highlightMorale = true;
  bool highlightEnemies = true;
  bool displayAllHealthBars = true;
  bool hideFullHealthBars = true;
  bool colorWoundedRed = false;
  struct HighlightedInfo {
    optional<Vec2> creaturePos;
    optional<Vec2> tilePos;
    optional<ViewObject> object;
  };
  const HighlightedInfo& getLastHighlighted();
  bool isCreatureHighlighted(UniqueEntity<Creature>::Id);

  private:
  void updateObject(Vec2, CreatureView*, milliseconds currentTime);
  void drawObjectAbs(Renderer&, Vec2 pos, const ViewObject&, Vec2 size, Vec2 tilePos, milliseconds currentTimeReal);
  void drawCreatureHighlights(Renderer&, const ViewObject&, Vec2 pos, Vec2 sz, milliseconds currentTimeReal);
  void drawCreatureHighlight(Renderer&, Vec2 pos, Vec2 size, Color);
  void drawSquareHighlight(Renderer&, Vec2 pos, Vec2 size);
  void considerRedrawingSquareHighlight(Renderer&, milliseconds currentTimeReal, Vec2 pos, Vec2 size);
 // void drawFloorBorders(Renderer& r, DirSet borders, int x, int y);
  void drawFoWSprite(Renderer&, Vec2 pos, Vec2 size, DirSet dirs);
  void renderExtraBorders(Renderer&, milliseconds currentTimeReal);
  void renderHighlights(Renderer&, Vec2 size, milliseconds currentTimeReal, bool lowHighlights);
  optional<Vec2> getMousePos();
  void softScroll(double x, double y);
  void setSoftCenter(Vec2);
  void setSoftCenter(double x, double y);
  HighlightedInfo lastHighlighted;
  void renderMapObjects(Renderer&, Vec2 size, milliseconds currentTimeReal);
  HighlightedInfo getHighlightedInfo(Vec2 size, milliseconds currentTimeReal);
  void renderAnimations(Renderer&, milliseconds currentTimeReal);

  Vec2 getMovementOffset(const ViewObject&, Vec2 size, double time, milliseconds curTimeReal, bool verticalMovement);
  Vec2 projectOnScreen(Vec2 wpos);
  bool considerCreatureClick(Vec2 mousePos);
  struct CreatureInfo {
    UniqueEntity<Creature>::Id id;
    ViewId viewId;
  };
  optional<CreatureInfo> getCreature(Vec2 mousePos);
  void considerContinuousLeftClick(Vec2 mousePos);
  MapLayout* layout;
  Table<optional<ViewIndex>> objects;
  bool spriteMode;
  Rectangle levelBounds = Rectangle(1, 1);
  Callbacks callbacks;
  optional<milliseconds> lastScrollUpdate;
  Clock* clock;
  optional<Vec2> mouseHeldPos;
  optional<CreatureInfo> draggedCandidate;
  optional<Vec2> lastMapLeftClick;
  struct AnimationInfo {
    PAnimation animation;
    Vec2 position;
  };
  vector<AnimationInfo> animations;
  Options* options;
  DirtyTable<bool> fogOfWar;
  DirtyTable<vector<ViewId>> extraBorderPos;
  bool isFoW(Vec2 pos) const;
  struct Coords {
    double x;
    double y;
    COMPARE_ALL(x, y)
  } mouseOffset, center;
  WConstLevel previousLevel = nullptr;
  const CreatureView* previousView = nullptr;
  Table<optional<milliseconds>> lastSquareUpdate;
  optional<Coords> softCenter;
  Vec2 lastMousePos;
  optional<Vec2> lastMouseMove;
  bool isScrollingNow = false;
  double currentTimeGame = 0;
  struct ScreenMovement {
    milliseconds startTimeReal;
    milliseconds endTimeReal;
    double startTimeGame;
  };
  optional<ScreenMovement> screenMovement;
  class ViewIdMap {
    public:
    ViewIdMap(Rectangle bounds);
    void add(Vec2 pos, ViewId);
    void remove(Vec2 pos);
    bool has(Vec2 pos, ViewId);
    void clear();

    private:
    DirtyTable<EnumSet<ViewId>> ids;
  };
  bool keyScrolling = false;
  ViewIdMap connectionMap;
  bool mouseUI = false;
  bool lockedView = true;
  optional<milliseconds> lastRightClick;
  EntityMap<Creature, int> teamHighlight;
  optional<ViewId> buttonViewId;
  set<Vec2> shadowed;
  bool isRenderedHighlight(const ViewIndex&, HighlightType);
  bool isRenderedHighlightLow(const ViewIndex&, HighlightType);
  optional<ViewId> getHighlightedFurniture();
  Color getHighlightColor(const ViewIndex&, HighlightType);
  void renderHighlight(Renderer& renderer, Vec2 pos, Vec2 size, const ViewIndex& index, HighlightType highlight);
  void renderTexturedHighlight(Renderer&, Vec2 pos, Vec2 size, Color);
  void processScrolling(milliseconds);
  void considerScrollingToCreature();
  GuiFactory* guiFactory;
  optional<UniqueEntity<Creature>::Id> getDraggedCreature() const;
  void setDraggedCreature(UniqueEntity<Creature>::Id, ViewId, Vec2 origin);
  vector<Vec2> tutorialHighlightLow;
  vector<Vec2> tutorialHighlightHigh;
  void drawHealthBar(Renderer&, Vec2 pos, Vec2 size, double health);
  double lastEndTimeGame = -1000;
  double getDistanceToEdgeRatio(Vec2);
  struct CenteredCreatureInfo {
    Vec2 pos;
    bool softScroll;
  };
  optional<CenteredCreatureInfo> centeredCreaturePosition;
};
