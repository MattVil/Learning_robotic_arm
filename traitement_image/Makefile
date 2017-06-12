all:
	g++ mouvement.cpp -o mouvement `pkg-config --cflags opencv` `pkg-config --libs opencv`
	g++ mouvColor.cpp -o mouvColor `pkg-config --cflags opencv` `pkg-config --libs opencv`
	g++ smoothImage.cpp -o smoothImage `pkg-config --cflags opencv` `pkg-config --libs opencv`

mouvement:
	g++ mouvement.cpp -o mouvement `pkg-config --cflags opencv` `pkg-config --libs opencv`

mouvColor:
	g++ mouvColor.cpp -o mouvColor `pkg-config --cflags opencv` `pkg-config --libs opencv`

smoothImage:
	g++ smoothImage.cpp -o smoothImage `pkg-config --cflags opencv` `pkg-config --libs opencv`

clean:
	rm mouvement