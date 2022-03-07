docker run -d  \
 --name postgres_biggestwar \
 -e POSTGRES_PASSWORD=postgrespassword \
 -e PGDATA=/var/lib/postgresql/data/pgdata \
 -v /media/icecool/datos/Proyectos/BiggestWar/server/data/postgres_docker/data:/var/lib/postgresql/data \
 -v /media/icecool/datos/Proyectos/BiggestWar/server/data/postgres_docker/scripts:/docker-entrypoint-initdb.d \
 -p 5400:5432 \
 postgres:9.6
