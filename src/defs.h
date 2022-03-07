#ifndef _defs_h_
#define _defs_h_

enum ObjectType {
	OT_MINERAL = 1,
	OT_BASE,
	OT_BARRACA,
	OT_TORRETA,
	OT_RECOLECTOR,
	OT_SOLDADO_RASO,
	OT_SOLDADO_ENTRENADO,
	OT_TANQUE,
	OT_TANQUE_PESADO,
	OT_ALL,
	OT_NONE
};

enum ActionType{
	AT_MOVE = 1,
	AT_RECOLLECT,
	AT_BUILD,
	AT_ATACK,
	AT_NEWUNIT
};

enum ActionSubType{
	AST_MOVE_MOVE = 1,
	AST_RECOLLECT_GORESOURCE,
	AST_RECOLLECT_RECOLLECTING,
	AST_RECOLLECT_GOBASE,
	AST_RECOLLECT_PUTTOBASE,
	AST_BUILD_GOTOBUILD,
	AST_BUILD_BUILDING,
	AST_ATACK_GOTOATACK,
	AST_ATACK_ATACKING,
	AST_NEWUNIT_MAKEUNIT
};

class WorldResult;
class Engine;
class PlayerView;
class ActionView;
class ObjectView;
class PlayerViewUpdate;
class Object;
class World;
class vec2;
struct ParamUnit;
class Unit;
struct ParamBuilding;
class Building;
class BuildingSoldier;
struct ParamAction;
class Action;
class Player;
class Players;
class Map;
class IDS;
class Mineral;
class Recolector;
class Soldado;
class SoldadoRaso;
class SoldadoEntrenado;
class Tanque;
class TanquePesado;
class Base;
class Barraca;
class Torreta;
class Collision;
class MiniMap;
class PathFinder;
class Semaphore;
class Terrain;
class Net;
class Sock;
class CommandNet;
class SockServer;
class User;
class Time;
class Area;

#endif
