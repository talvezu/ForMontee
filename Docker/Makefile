docker_setup:
	./docker_setup.sh

docker_build:
	docker build -t for_montee .

docker_run:
	docker run -v /home/nlevy/talvezu/ForMontee/Docker:/home/external -it for_montee /bin/bash

docker_reattach:
ifdef PID
	printf "trying to reattach to container: ${PID}"
	docker start ${PID} #restart it in the background
	docker attach ${PID} # reattach the terminal & stdin
else
	printf "docker_reattach requires PID, e.g:\n\t'make docker_reattach PID=123'\n"
endif

docker_clean_containers:
	docker container prune -f

docker_clean_images:
	docker image ls | grep -v 'ubuntu\|IMAGE' | awk '{print $$3}' | xargs docker image rm

docker_clean: docker_clean_containers docker_clean_images
