all:
	$(MAKE) -C module
	$(MAKE) -C server
	$(MAKE) -C client

clean:
	$(MAKE) -C module clean
	$(MAKE) -C server clean
	$(MAKE) -C client clean
