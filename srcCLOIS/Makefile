flags=-Wno-deprecated -O3 -g -ggdb -std=c++11
prog=CLOIS
comp=g++
Seed=28
config_file=conf-AK.config
InstancesFolder=instances
ExecutableBashFile=AKsh.sh
NSeeds=3
MaxM=10
MaxEval=1000
MaxTime=0
Min=1 #Objective in Tuning: 1=minimize, 0=maximize
tabulistsize=9

$(prog): main.o globales.o
	$(comp) globales.o main.o -o $(prog) $(flags)
	
globales.o: globales.cpp includes.h globales.h 
	$(comp) -c globales.cpp $(flags)	
	
main.o: main.cpp includes.h globales.h 
	$(comp) -c main.cpp $(flags)

clean:
	rm -f *~
	rm -f *.o
	rm -f *.conv

exe:$(prog)
	./$(prog) $(ExecutableBashFile) $(config_file) $(InstancesFolder) $(Seed) $(NSeeds) $(MaxM) $(MaxEval) $(MaxTime) $(Min) $(tabulistsize)
