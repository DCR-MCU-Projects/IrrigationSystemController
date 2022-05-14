#!/bin/bash

docker run \
	--name irrigation-controller \
	-p 32001:80 \
	-v $(pwd)/src:/var/www/html \
	irrigation-controller:1.0
				