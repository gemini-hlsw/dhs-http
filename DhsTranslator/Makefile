CODE_DIR := src

.PHONY: all

all:
	$(MAKE) -C $(CODE_DIR)
       
.PHONY: clean

clean:
	$(MAKE) -C $(CODE_DIR) clean
	rm -Rf ./server
	
$(CODE_DIR)/libDhsTranslator.so: all	
	
.PHONY: build-test

build-test: $(CODE_DIR)/libDhsTranslator.so resources/services.xml
	[ -d ./server/services/dhs ] || mkdir -p ./server/services/dhs
	[ -d ./server/logs ] || mkdir -p ./server/logs
	[ -e ./server/axis2.xml ] || cp ${AXIS2C_HOME}/axis2.xml ./server
	[ -h ./server/lib ] || ln -s ${AXIS2C_HOME}/lib ./server/lib
	[ -h ./server/modules ] || ln -s ${AXIS2C_HOME}/modules ./server/modules
	cp $(CODE_DIR)/libDhsTranslator.so resources/services.xml ./server/services/dhs
