TARGET = "default"
STATE = "default"
VERBOSE = 0

help :
	$(info Displaying commands for make :)
	$(info ------------------------------)

	$(info )
	$(info -How to compil a .as file :)
	$(info make compil TARGET=name.as)
	$(info Give a bin to be executed by the core.)

	$(info )
	$(info -How to compil the core :)
	$(info make core)
	$(info Give an executable core usable with bin.)

	$(info )
	$(info -How to execute a .bin file :)
	$(info make exec TARGET=name.bin STATE=state.txt VERBOSE(0/1))

	$(info )
	$(info -How to clean bin and exec files :)
	$(info make clean)
	$(info delete every bin files and the core executable)

	$(info )
	$(info -How to execute something faster :)
	$(info make fast TARGET=name)
	$(info Requires a core)

	$(info )

compil :
	python3 compilateur.py -f $(TARGET)
	$(info Bin file created)

core :
	rm -rf core
	gcc coeur.c -lm -o core
	$(info Core compiled.)

exec :
	./core $(TARGET) $(STATE) $(VERBOSE)

clean :
	rm -rf *.bin
	rm -rf core
fast :
	python3 compilateur.py -f $(TARGET)_test.s
	./core $(TARGET)_test.bin $(TARGET)_state.txt
