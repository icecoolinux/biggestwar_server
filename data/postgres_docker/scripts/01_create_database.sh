
#!/bin/bash
set -e

psql -v ON_ERROR_STOP=1 --username "$POSTGRES_USER" --dbname "$POSTGRES_DB" <<-EOSQL
    CREATE USER biggestwaruser with encrypted password 'biggestwarpass';
    CREATE DATABASE biggestwar;
    GRANT ALL PRIVILEGES ON DATABASE biggestwar TO biggestwaruser;
EOSQL

psql -v ON_ERROR_STOP=1 --username "$POSTGRES_USER" --dbname biggestwar -f /docker-entrypoint-initdb.d/notexecute/modelo.sql
