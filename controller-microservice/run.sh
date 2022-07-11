#!/bin/bash

docker rm -f irrigation-controller

docker run -d \
	--name irrigation-controller \
	-p 32001:80 \
	-v $(pwd)/src:/var/www/html \
	irrigation-controller:1.4
				