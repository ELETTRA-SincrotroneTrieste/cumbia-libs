.DEFAULT_GOAL := build-all

build-all: build-param1 build-param2

build-param1:
	./scripts/cubuild.sh clean --no-interactive

build-param2: build-param1
	./scripts/cubuild.sh build tango random http --no-interactive

build-param3: build-param2
	./scripts/cubuild.sh install  --no-interactive
