all:
	$(MAKE) -C module
	$(MAKE) -C server

clean:
	$(MAKE) -C module clean
	$(MAKE) -C server clean
