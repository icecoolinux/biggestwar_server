todo: serverWorld serverLogin serverAdmin

serverLogin: obj/mainLogin.o obj/bd.o obj/barraca.o obj/base.o obj/building.o obj/buildingsoldier.o obj/mineral.o obj/object.o obj/recolector.o obj/soldado.o obj/soldadoentrenado.o obj/soldadoraso.o obj/tanque.o obj/tanquepesado.o obj/torreta.o obj/unit.o obj/action.o obj/collision.o obj/minimap.o obj/engine.o obj/ids.o obj/map.o obj/player.o obj/players.o obj/terrain.o obj/vec2.o obj/world.o obj/worldresult.o obj/playerview.o obj/objectview.o obj/actionview.o obj/playerviewupdate.o obj/user.o obj/net.o obj/sock.o obj/commandnet.o obj/sockserver.o obj/replayuser.o obj/replayworld.o obj/semaphore.o obj/time.o obj/compress_image.o obj/pathfinder.o
	g++ -g obj/mainLogin.o obj/bd.o obj/barraca.o obj/base.o obj/building.o obj/buildingsoldier.o obj/mineral.o obj/object.o obj/recolector.o obj/soldado.o obj/soldadoentrenado.o obj/soldadoraso.o obj/tanque.o obj/tanquepesado.o obj/torreta.o obj/unit.o obj/action.o obj/collision.o obj/minimap.o obj/engine.o obj/ids.o obj/map.o obj/player.o obj/players.o obj/terrain.o obj/vec2.o obj/world.o obj/worldresult.o obj/playerview.o obj/playerviewupdate.o obj/objectview.o obj/actionview.o obj/user.o obj/net.o obj/sock.o obj/commandnet.o obj/sockserver.o obj/replayuser.o obj/replayworld.o obj/semaphore.o obj/time.o obj/area.o obj/compress_image.o obj/pathfinder.o -o serverLogin -lpthread -lboost_filesystem -lboost_system -lpng -ljpeg -lpq
	
serverWorld: obj/mainWorld.o obj/bd.o obj/barraca.o obj/base.o obj/building.o obj/buildingsoldier.o obj/mineral.o obj/object.o obj/recolector.o obj/soldado.o obj/soldadoentrenado.o obj/soldadoraso.o obj/tanque.o obj/tanquepesado.o obj/torreta.o obj/unit.o obj/action.o obj/collision.o obj/minimap.o obj/engine.o obj/ids.o obj/map.o obj/player.o obj/players.o obj/terrain.o obj/vec2.o obj/world.o obj/worldresult.o obj/playerview.o obj/objectview.o obj/actionview.o obj/playerviewupdate.o obj/user.o obj/net.o obj/sock.o obj/commandnet.o obj/sockserver.o obj/replayuser.o obj/replayworld.o obj/semaphore.o obj/time.o obj/area.o obj/compress_image.o obj/pathfinder.o
	g++ -g obj/mainWorld.o obj/bd.o obj/barraca.o obj/base.o obj/building.o obj/buildingsoldier.o obj/mineral.o obj/object.o obj/recolector.o obj/soldado.o obj/soldadoentrenado.o obj/soldadoraso.o obj/tanque.o obj/tanquepesado.o obj/torreta.o obj/unit.o obj/action.o obj/collision.o obj/minimap.o obj/engine.o obj/ids.o obj/map.o obj/player.o obj/players.o obj/terrain.o obj/vec2.o obj/world.o obj/worldresult.o obj/playerview.o obj/playerviewupdate.o obj/objectview.o obj/actionview.o obj/user.o obj/net.o obj/sock.o obj/commandnet.o obj/sockserver.o obj/replayuser.o obj/replayworld.o obj/semaphore.o obj/time.o obj/area.o obj/compress_image.o obj/pathfinder.o -o serverWorld -lpthread -lboost_filesystem -lboost_system -lpng -ljpeg -lpq

serverAdmin: obj/mainAdmin.o obj/bd.o
	g++ -g obj/mainAdmin.o obj/bd.o obj/semaphore.o -o serverAdmin -lpq


	
	
obj/mainWorld.o: src/mainWorld.cpp
	g++ -g -c src/mainWorld.cpp -o obj/mainWorld.o

obj/mainAdmin.o: src/mainAdmin.cpp
	g++ -g -c src/mainAdmin.cpp -o obj/mainAdmin.o
	
obj/bd.o: src/bd.cpp
	g++ -g -c src/bd.cpp -o obj/bd.o

obj/mainLogin.o: src/mainLogin.h src/mainLogin.cpp
	g++ -g -c src/mainLogin.cpp -o obj/mainLogin.o
	
obj/barraca.o: src/engine/objects/barraca.h src/engine/objects/barraca.cpp
	g++ -g -c src/engine/objects/barraca.cpp -o obj/barraca.o

obj/base.o: src/engine/objects/base.h src/engine/objects/base.cpp
	g++ -g -c src/engine/objects/base.cpp -o obj/base.o
	
obj/building.o: src/engine/objects/building.h src/engine/objects/building.cpp
	g++ -g -c src/engine/objects/building.cpp -o obj/building.o

obj/buildingsoldier.o: src/engine/objects/buildingsoldier.h src/engine/objects/buildingsoldier.cpp
	g++ -g -c src/engine/objects/buildingsoldier.cpp -o obj/buildingsoldier.o
	
obj/mineral.o: src/engine/objects/mineral.h src/engine/objects/mineral.cpp
	g++ -g -c src/engine/objects/mineral.cpp -o obj/mineral.o
	
obj/object.o: src/engine/objects/object.h src/engine/objects/object.cpp
	g++ -g -c src/engine/objects/object.cpp -o obj/object.o
	
obj/recolector.o: src/engine/objects/recolector.h src/engine/objects/recolector.cpp
	g++ -g -c src/engine/objects/recolector.cpp -o obj/recolector.o
	
obj/soldado.o: src/engine/objects/soldado.h src/engine/objects/soldado.cpp
	g++ -g -c src/engine/objects/soldado.cpp -o obj/soldado.o
	
obj/soldadoentrenado.o: src/engine/objects/soldadoentrenado.h src/engine/objects/soldadoentrenado.cpp
	g++ -g -c src/engine/objects/soldadoentrenado.cpp -o obj/soldadoentrenado.o
	
obj/soldadoraso.o: src/engine/objects/soldadoraso.h src/engine/objects/soldadoraso.cpp
	g++ -g -c src/engine/objects/soldadoraso.cpp -o obj/soldadoraso.o
	
obj/tanque.o: src/engine/objects/tanque.h src/engine/objects/tanque.cpp
	g++ -g -c src/engine/objects/tanque.cpp -o obj/tanque.o
	
obj/tanquepesado.o: src/engine/objects/tanquepesado.h src/engine/objects/tanquepesado.cpp
	g++ -g -c src/engine/objects/tanquepesado.cpp -o obj/tanquepesado.o
	
obj/torreta.o: src/engine/objects/torreta.h src/engine/objects/torreta.cpp
	g++ -g -c src/engine/objects/torreta.cpp -o obj/torreta.o
	
obj/unit.o: src/engine/objects/unit.h src/engine/objects/unit.cpp
	g++ -g -c src/engine/objects/unit.cpp -o obj/unit.o
	
obj/action.o: src/engine/action.h src/engine/action.cpp
	g++ -g -c src/engine/action.cpp -o obj/action.o
	
obj/collision.o: src/engine/collision/collision.h src/engine/collision/collision.cpp
	g++ -g -c src/engine/collision/collision.cpp -o obj/collision.o

obj/pathfinder.o: src/engine/collision/pathfinder.h src/engine/collision/pathfinder.cpp
	g++ -g -c src/engine/collision/pathfinder.cpp -o obj/pathfinder.o

obj/minimap.o: src/engine/minimap.h src/engine/minimap.cpp
	g++ -g -c src/engine/minimap.cpp -o obj/minimap.o
	
obj/engine.o: src/engine/engine.h src/engine/engine.cpp
	g++ -g -c src/engine/engine.cpp -o obj/engine.o
	
obj/ids.o: src/engine/ids.h src/engine/ids.cpp
	g++ -g -c src/engine/ids.cpp -o obj/ids.o
	
obj/map.o: src/engine/map.h src/engine/map.cpp
	g++ -g -c src/engine/map.cpp -o obj/map.o
	
obj/player.o: src/engine/player.h src/engine/player.cpp
	g++ -g -c src/engine/player.cpp -o obj/player.o
	
obj/players.o: src/engine/players.h src/engine/players.cpp
	g++ -g -c src/engine/players.cpp -o obj/players.o
	
obj/terrain.o: src/engine/terrain.h src/engine/terrain.cpp
	g++ -g -c src/engine/terrain.cpp -o obj/terrain.o
	
obj/vec2.o: src/engine/vec2.h src/engine/vec2.cpp
	g++ -g -c src/engine/vec2.cpp -o obj/vec2.o

obj/area.o: src/engine/area.h src/engine/area.cpp
	g++ -g -c src/engine/area.cpp -o obj/area.o
	
obj/world.o: src/engine/world.h src/engine/world.cpp
	g++ -g -c src/engine/world.cpp -o obj/world.o

obj/worldresult.o: src/engine/worldresult.h src/engine/worldresult.cpp
	g++ -g -c src/engine/worldresult.cpp -o obj/worldresult.o
	
obj/playerview.o: src/view/playerview.h src/view/playerview.cpp
	g++ -g -c src/view/playerview.cpp -o obj/playerview.o

obj/objectview.o: src/view/objectview.h src/view/objectview.cpp
	g++ -g -c src/view/objectview.cpp -o obj/objectview.o

obj/actionview.o: src/view/actionview.h src/view/actionview.cpp
	g++ -g -c src/view/actionview.cpp -o obj/actionview.o
	
obj/playerviewupdate.o: src/view/playerviewupdate.h src/view/playerviewupdate.cpp
	g++ -g -c src/view/playerviewupdate.cpp -o obj/playerviewupdate.o

obj/user.o: src/user.h src/user.cpp
	g++ -g -c src/user.cpp -o obj/user.o

obj/net.o: src/net/net.h src/net/net.cpp
	g++ -g -c src/net/net.cpp -o obj/net.o

obj/sock.o: src/net/sock.h src/net/sock.cpp
	g++ -g -c src/net/sock.cpp -o obj/sock.o

obj/commandnet.o: src/net/commandnet.h src/net/commandnet.cpp
	g++ -g -c src/net/commandnet.cpp -o obj/commandnet.o
	
obj/sockserver.o: src/net/sockserver.h src/net/sockserver.cpp
	g++ -g -c src/net/sockserver.cpp -o obj/sockserver.o

obj/replayuser.o: src/net/replayuser.h src/net/replayuser.cpp
	g++ -g -c src/net/replayuser.cpp -o obj/replayuser.o

obj/replayworld.o: src/engine/replayworld.h src/engine/replayworld.cpp
	g++ -g -c src/engine/replayworld.cpp -o obj/replayworld.o
	
obj/semaphore.o: src/semaphore.h src/semaphore.cpp
	g++ -g -c src/semaphore.cpp -o obj/semaphore.o

obj/time.o: src/time.h src/time.cpp
	g++ -g -c src/time.cpp -o obj/time.o

obj/compress_image.o: src/compress_image.h src/compress_image.cpp
	g++ -g -c src/compress_image.cpp -o obj/compress_image.o

clean:
	rm -rf obj/* serverWorld serverLogin serverAdmin
