INC=-Iheader/
FLAGS_DEBUG=-O0 -g -Wall -Wextra
FLAGS_RELEASE=-O3 -DNDEBUG -Wall -Wextra

default: app

app: unity.c
	gcc $(INC) unity.c -o bin/app $(FLAGS_DEBUG)
run: unity.c
	gcc $(INC) unity.c -o bin/app $(FLAGS_DEBUG) && ./bin/app

release: unity.c
	gcc $(INC) unity.c -o bin/app_release $(FLAGS_RELEASE)
