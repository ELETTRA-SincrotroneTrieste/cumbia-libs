.DEFAULT_GOAL := build-all

build-all: build-param1


build-param1:
	./scripts/cubuild.sh build tango random http --no-interactive

install:
	./scripts/cubuild.sh install  --no-interactive

clean:
	./scripts/cubuild.sh clean --no-interactive