#include "Scenario.h"
#ifdef __linux__ 
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define wcscat_s(x,y,z) wcscat(x,z)
#endif

Scenario::Scenario(Camera *cam) {
    glm::vec3 translate;
	glm::vec3 scale;
    Model* model = new Model("models/Cube/Cube.obj", cam);
	translate = glm::vec3(0.0f, 0.0f, 3.0f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);	// it's a bit too big for our scene, so scale it down
	model->setScale(&scale);
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	InitGraph(model);
}
Scenario::Scenario(Model *camIni) {
    InitGraph(camIni);
}
void Scenario::InitGraph(Model *main) {
    float matAmbient[] = { 1,1,1,1 };
	float matDiff[] = { 1,1,1,1 };
	angulo = 0;
	camara = main;
	//creamos el objeto skydome
	sky = new SkyDome(32, 32, 20, (WCHAR*)L"skydome/CIELO2.jpg", main->cameraDetails);
	//creamos el terreno
	terreno = new Terreno((WCHAR*)L"skydome/PISO2.jpg", (WCHAR*)L"skydome/coast_sand_rocks_02_diff.jpg", 2000, 2000, main->cameraDetails);
	water = new Water((WCHAR*)L"textures/terreno.bmp", (WCHAR*)L"textures/water.bmp", 20, 20, camara->cameraDetails);
	glm::vec3 translate;
	glm::vec3 scale;
	glm::vec3 rotation;
	translate = glm::vec3(0.0f, 20.0f, 30.0f);
	water->setTranslate(&translate);
	// load models
	// -----------
	ourModel.emplace_back(main);
	Model* model;

	ModelAttributes m;


	model = new Model("models/dancing_vampire/dancing_vampire.dae", main->cameraDetails);
	translate = glm::vec3(0.0f, terreno->Superficie(0.0f, 60.0f), 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotY(90);
	ourModel.emplace_back(model);
	try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/dancing_vampire/dancing_vampire.dae", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::vector<Animation> animation = Animation::loadAllAnimations("models/dancing_vampire/dancing_vampire.dae", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::move(animation.begin(), animation.end(), std::back_inserter(animations));
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(1);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}

	Model* silly = new Model("models/Silly_Dancing/Silly_Dancing.fbx", main->cameraDetails);
	translate = glm::vec3(10.0f, terreno->Superficie(10.0f, 60.0f) , 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	silly->setTranslate(&translate);
	silly->setNextTranslate(&translate);
	silly->setScale(&scale);
	silly->setNextRotY(180);
	ourModel.emplace_back(silly);
	try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/Silly_Dancing/Silly_Dancing.fbx", silly->GetBoneInfoMap(), silly->getBonesInfo(), silly->GetBoneCount());
		for (Animation animation : animations)
			silly->setAnimator(Animator(animation));
		silly->setAnimation(0);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}
	model = CollitionBox::GenerateAABB(translate, silly->AABBsize, main->cameraDetails);
	m.hitbox = model; // Le decimos al ultimo ModelAttribute que tiene un hitbox asignado
	silly->getModelAttributes()->push_back(m);
	translate.x += 10;
	silly->setTranslate(&translate, silly->getModelAttributes()->size()-1);
	silly->setNextTranslate(&translate, silly->getModelAttributes()->size()-1);
	silly->setScale(&scale, silly->getModelAttributes()->size()-1);
	silly->setNextRotY(180, silly->getModelAttributes()->size()-1);
	silly->setRotY(180, silly->getModelAttributes()->size()-1);
	// Import model and clone with bones and animations
	model = new Model("models/Silly_Dancing/Silly_Dancing.fbx", main->cameraDetails);
	translate = glm::vec3(30.0f, terreno->Superficie(30.0f, 60.0f) , 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	model->name = "Silly_Dancing1";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotY(180);
	ourModel.emplace_back(model);
	// Para clonar la animacion se eliminan los huesos del modelo actual y se copian los modelos y animators
	model->GetBoneInfoMap()->clear();
	model->getBonesInfo()->clear();
	*model->GetBoneInfoMap() = *silly->GetBoneInfoMap();
	*model->getBonesInfo() = *silly->getBonesInfo();
	model->setAnimator(silly->getAnimator());

    /*
    model = new Model("models/Villano/Zombie.fbx", main->cameraDetails);
    translate = glm::vec3(100.0f, terreno->Superficie(100.0f, 60.0f), 0.0f);
    scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    model->setNextRotY(90);
    ourModel.emplace_back(model);
    try {
        std::vector<Animation> animations = Animation::loadAllAnimations("models/Villano/Walking.fbx", silly->GetBoneInfoMap(), silly->getBonesInfo(), silly->GetBoneCount());
        for (Animation animation : animations)
            silly->setAnimator(Animator(animation));
        silly->setAnimation(0);
    }
    catch (...) {
        ERRORL("Could not load animation!", "ANIMACION");
    }
    */

	model = new Model("models/Arbol1/pine_tree.glb", main->cameraDetails);
	translate = glm::vec3(100.0f, terreno->Superficie(100.0f, 30.0f), 30.0f);
	scale = glm::vec3(0.1f, 0.1f, 0.1f);	// it's a bit too big for our scene, so scale it down
	model->name = "Arbol1";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	ourModel.emplace_back(model);

    model = new Model("models/Ambiente/pine_tree_single_01.glb", main->cameraDetails);
    translate = glm::vec3(50.0f, terreno->Superficie(150.0f, 30.0f), 90.0f);
    scale = glm::vec3(0.08f, 0.08f, 0.08f);	// it's a bit too big for our scene, so scale it down
    model->name = "Arbol3";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    //model->setNextRotX(270);
    ourModel.emplace_back(model);

    model = new Model("models/Estructuras/FABRICAGLB.glb", main->cameraDetails);
    translate = glm::vec3(50.0f, terreno->Superficie(200.0f, 30.0f), 600.0f);
    scale = glm::vec3(8.0f, 8.0f, 8.0f);	// it's a bit too big for our scene, so scale it down
    model->name = "Arbol3";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    //model->setNextRotX(270);
    ourModel.emplace_back(model);

    // ------------------ VEHICULOS -----------------------

    model = new Model("models/Objetos/CAR1.glb", main->cameraDetails);
    translate = glm::vec3(200.0f, terreno->Superficie(200.0f, 30.0f), 150.0f);
    scale = glm::vec3(0.15f, 0.15f, 0.15f);	// it's a bit too big for our scene, so scale it down
    model->name = "car1";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    //model->setNextRotX(270);
    ourModel.emplace_back(model);

    model = new Model("models/Objetos/CAR2.glb", main->cameraDetails);
    translate = glm::vec3(150.0f, terreno->Superficie(150.0f, 30.0f), 150.0f);
    scale = glm::vec3(0.025f, 0.025f, 0.025f);	// it's a bit too big for our scene, so scale it down
    model->name = "car2";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    model->setNextRotZ(90);
    model->setNextRotX(270);
    ourModel.emplace_back(model);

    model = new Model("models/Objetos/CAR3.glb", main->cameraDetails);
    translate = glm::vec3(100.0f, terreno->Superficie(100.0f, 30.0f), 150.0f);
    scale = glm::vec3(0.06f, 0.06f, 0.06f);	// it's a bit too big for our scene, so scale it down
    model->name = "car3";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    model->setNextRotZ(90);
    model->setNextRotX(270);
    ourModel.emplace_back(model);

    // -------------------------------------------------------

    model = new Model("models/Objetos/BARREL.glb", main->cameraDetails);
    translate = glm::vec3(70.0f, terreno->Superficie(70.0f, 30.0f) + 2.0f, 150.0f);
    scale = glm::vec3(0.6f, 0.6f, 0.6f);	// it's a bit too big for our scene, so scale it down
    model->name = "Barril";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    //model->setNextRotZ(90);
    //model->setNextRotX(270);
    ourModel.emplace_back(model);

    model = new Model("models/Objetos/NOTEBOOK.glb", main->cameraDetails);
    translate = glm::vec3(50.0f, terreno->Superficie(50.0f, 30.0f) + 2.0f, 150.0f);
    scale = glm::vec3(0.1f, 0.1f, 0.1f);	// it's a bit too big for our scene, so scale it down
    model->name = "Libreta";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    //model->setNextRotZ(180);
    model->setNextRotY(180);
    ourModel.emplace_back(model);

    model = new Model("models/Objetos/SENAL.glb", main->cameraDetails);
    translate = glm::vec3(30.0f, terreno->Superficie(30.0f, 30.0f) + 0.0f, 150.0f);
    scale = glm::vec3(3.0f, 3.0f, 3.0f);	// it's a bit too big for our scene, so scale it down
    model->name = "Libreta";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    model->setNextRotX(270);
    model->setNextRotZ(90);
    ourModel.emplace_back(model);

    model = new Model("models/Objetos/MEDIDOR.glb", main->cameraDetails);
    translate = glm::vec3(10.0f, terreno->Superficie(10.0f, 30.0f) + 2.0f, 150.0f);
    scale = glm::vec3(0.0001f, 0.0001f, 0.0001f);	// it's a bit too big for our scene, so scale it down
    model->name = "Medidor";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    //model->setNextRotX(270);
    //model->setNextRotZ(90);
    ourModel.emplace_back(model);

    model = new Model("models/Objetos/MEDIDOR2.glb", main->cameraDetails);
    translate = glm::vec3(-10.0f, terreno->Superficie(-10.0f, 30.0f) + 2.0f, 150.0f);
    scale = glm::vec3(0.01f, 0.01f, 0.01f);	// it's a bit too big for our scene, so scale it down
    model->name = "Medidor";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    //model->setNextRotX(270);
    //model->setNextRotZ(90);
    ourModel.emplace_back(model);

    model = new Model("models/Objetos/STONES.glb", main->cameraDetails);
    translate = glm::vec3(-30.0f, terreno->Superficie(-30.0f, 30.0f) + 2.0f, 150.0f);
    scale = glm::vec3(4.f, 4.f, 4.f);	// it's a bit too big for our scene, so scale it down
    model->name = "Stones";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    //model->setNextRotX(270);
    //model->setNextRotZ(90);
    ourModel.emplace_back(model);

    model = new Model("models/Objetos/RECORDER.glb", main->cameraDetails);
    translate = glm::vec3(-50.0f, terreno->Superficie(-50.0f, 30.0f) + 2.0f, 150.0f);
    scale = glm::vec3(0.1f, 0.1f, 0.1f);	// it's a bit too big for our scene, so scale it down
    model->name = "Stones";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    model->setNextRotX(-90);
    model->setNextRotZ(90);
    ourModel.emplace_back(model);

    // ------------------ BOSQUE ALEATORIO -----------------------

    // Generar bosque aleatorio (árboles modelos reducidos y cerca del pasillo)
    srand(time(NULL));  // Semilla para aleatoriedad
    const int numArboles = 20;  // Reducido para menos lag
    const float minDistancia = 15.0f;  // Distancia mínima entre árboles
    std::vector<glm::vec3> posicionesArboles;  // Lista de posiciones para evitar empalmes

    for (int i = 0; i < numArboles; ++i) {
        bool posicionValida = false;
        glm::vec3 translate;
        int intentos = 0;  // Límite de intentos para evitar bucles infinitos
        while (!posicionValida && intentos < 100) {
            // Generar cerca del pasillo: x en (-150 a -50) o (50 a 150), z en (-1000 a 0)
            float x;
            int ladoX = rand() % 2;  // 0: izquierda, 1: derecha
            if (ladoX == 0) {
                x = -150.0f + (rand() % 100);  // -150 a -50
            }
            else {
                x = 50.0f + (rand() % 100);  // 50 a 150
            }
            float z = -1000.0f + (rand() % 1000);  // -1000 a 0
            translate = glm::vec3(x, 0.0f, z);

            // Calcular y en el terreno
            translate.y = terreno->Superficie(x, z);

            // Verificar distancia mínima con árboles existentes
            posicionValida = true;
            for (const auto& pos : posicionesArboles) {
                float distancia = glm::distance(glm::vec2(translate.x, translate.z), glm::vec2(pos.x, pos.z));
                if (distancia < minDistancia) {
                    posicionValida = false;
                    break;
                }
            }
            intentos++;
        }

        if (!posicionValida) continue;  // Saltar si no se encontró posición válida

        posicionesArboles.push_back(translate);

        // Seleccionar modelo aleatoriamente (solo los dos que usas)
        int modeloSeleccionado = rand() % 2;  // 0 o 1
        Model* model = nullptr;
        glm::vec3 scale;
        float rotX = 0.0f;

        if (modeloSeleccionado == 0) {
            model = new Model("models/Arbol1/pine_tree.glb", main->cameraDetails);
            scale = glm::vec3(0.1f, 0.1f, 0.1f);
        }
        else {
            model = new Model("models/Ambiente/pine_tree_single_01.glb", main->cameraDetails);
            scale = glm::vec3(0.08f, 0.08f, 0.08f);
        }

        // Variaciones aleatorias en escala (±20%)
        float factorEscala = 0.8f + (rand() % 41) / 100.0f;  // 0.8 a 1.2
        scale *= factorEscala;

        // Rotación aleatoria en Y
        float rotY = (rand() % 360) * 1.0f;  // 0 a 360 grados

        model->setTranslate(&translate);
        model->setNextTranslate(&translate);
        model->setScale(&scale);
        if (rotX != 0.0f) model->setNextRotX(rotX);
        model->setNextRotY(rotY);
        model->name = "ArbolBosque" + std::to_string(i);

        ourModel.emplace_back(model);
    }

    model = new Model("models/Ambiente/rock_low-poly.glb", main->cameraDetails);
    translate = glm::vec3(50.0f, terreno->Superficie(200.0f, 30.0f), 60.0f);
    scale = glm::vec3(4.0f, 4.0f, 4.0f);	// it's a bit too big for our scene, so scale it down
    model->name = "Arbol3";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    //model->setNextRotX(270);
    ourModel.emplace_back(model);

    model = new Model("models/Ambiente/bush_test.glb", main->cameraDetails);
    translate = glm::vec3(80.0f, terreno->Superficie(80.0f, 30.0f), 30.0f);
    scale = glm::vec3(5.0f, 5.0f, 5.0f);	// it's a bit too big for our scene, so scale it down
    model->name = "Arbol2";
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setScale(&scale);
    model->setNextRotX(270);
    ourModel.emplace_back(model);


	model = new Model("models/Villano/zombie_mutant.glb", main->cameraDetails);
	translate = glm::vec3(20.0f, terreno->Superficie(20.0f, 30.0f), 30.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	model->name = "Arbol1";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	ourModel.emplace_back(model);
	

	inicializaBillboards();
	std::wstring prueba(L"Esta es una prueba");
	ourText.emplace_back(new Texto(prueba, 20, 0, 0, SCR_HEIGHT, 0, camara));
	billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/awesomeface.png", 6, 6, 100, 200, 0, camara->cameraDetails));
	scale = glm::vec3(100.0f, 100.0f, 0.0f);	// it's a bit too big for our scene, so scale it down
	billBoard2D.back()->setScale(&scale);
	}

void Scenario::inicializaBillboards() {
	float ye = terreno->Superficie(0, 0);
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol.png", 6, 6, 0, ye - 1, 0, camara->cameraDetails));

	ye = terreno->Superficie(-9, -15);
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol3.png", 8, 8, -9, ye - 1, -15, camara->cameraDetails));

	BillboardAnimation *billBoardAnimated = new BillboardAnimation();
	ye = terreno->Superficie(5, -5);
	for (int frameArbol = 1; frameArbol < 4; frameArbol++){
		wchar_t textura[50] = {L"billboards/Arbol"};
		if (frameArbol != 1){
			wchar_t convert[25];
			swprintf(convert, 25, L"%d", frameArbol);
			wcscat_s(textura, 50, convert);
		}
		wcscat_s(textura, 50, L".png");
		billBoardAnimated->pushFrame(new Billboard((WCHAR*)textura, 6, 6, 5, ye - 1, -5, camara->cameraDetails));		
	}
	billBoardAnim.emplace_back(billBoardAnimated);

    // Generar billboards aleatorios en el fondo del bosque
    const int numBillboards = 200;  // Número de billboards para densidad
    std::vector<std::string> imagenes = { "billboards/Arbol.png", "billboards/Pino1.png" };

    for (int i = 0; i < numBillboards; ++i) {
        // Posición aleatoria en áreas de bosque: z < 0, x fuera del pasillo (-100 a 100)
        float x;
        int ladoX = rand() % 2;
        if (ladoX == 0) {
            x = -1000.0f + (rand() % 900);  // -1000 a -100
        }
        else {
            x = 100.0f + (rand() % 900);  // 100 a 1000
        }
        float z = -1000.0f + (rand() % 1000);  // -1000 a 0
        float ye = terreno->Superficie(x, z) - 1;  // Altura ajustada sobre el terreno

        // Seleccionar imagen aleatoria
        int imgIdx = rand() % imagenes.size();
        std::string imgPath = imagenes[imgIdx];

        // Convertir a WCHAR para el constructor
        std::wstring wImgPath(imgPath.begin(), imgPath.end());
        WCHAR* textura = (WCHAR*)wImgPath.c_str();

        // Crear billboard (ancho y alto fijos, ajusta si quieres variaciones)
        billBoard.emplace_back(new Billboard(textura, 40.0f, 80.0f, x, ye, z, camara->cameraDetails));
    }
}

	//el metodo render toma el dispositivo sobre el cual va a dibujar
	//y hace su tarea ya conocida
Scene* Scenario::Render() {
	//borramos el biffer de color y el z para el control de profundidad a la 
	//hora del render a nivel pixel.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
//	glClearColor(255.0f, 255.0f, 255.0f, 255.0f);


	if (this->animacion > 10) { // Timer se ejecuta cada 1000/30 = 33.333 ms
		for (BillboardAnimation *b : billBoardAnim){
			b->nextAnimation();
		}
		this->animacion = 0;
	} else {
		animacion = animacion + (1 * gameTime.deltaTime/100);
	}
	// Decimos que dibuje la media esfera
	sky->Draw();
	// Ahora el terreno
	terreno->Draw();
	water->Draw();


	// Dibujamos cada billboard que este cargado en el arreglo de billboards.
	for (int i = 0; i < billBoard.size(); i++)
		billBoard[i]->Draw();
	for (int i = 0; i < billBoardAnim.size(); i++)
		billBoardAnim[i]->Draw();
	for (int i = 0; i < billBoard2D.size(); i++)
		billBoard2D[i]->Draw();
	// Dibujamos cada modelo que este cargado en nuestro arreglo de modelos
	for (int i = 0; i < ourModel.size(); i++) {
			ourModel[i]->Draw();
	}
	for (int i = 0; i < ourText.size(); i++) {
		ourText[i]->Draw();
	}
		// Le decimos a winapi que haga el update en la ventana
	return this;
}
	
std::vector<Model*> *Scenario::getLoadedModels() {
	return &ourModel;
}
std::vector<Billboard*> *Scenario::getLoadedBillboards() {
	return &billBoard;
}
std::vector<Billboard2D*> *Scenario::getLoadedBillboards2D(){
	return &billBoard2D;
}
std::vector<Texto*> *Scenario::getLoadedText(){
	return &ourText;
}
std::vector<BillboardAnimation*> *Scenario::getLoadedBillboardsAnimation(){
	return &billBoardAnim;
}

Model* Scenario::getMainModel() {
	return this->camara;
}
void Scenario::setMainModel(Model* mainModel){
	this->camara = mainModel;
}
float Scenario::getAngulo() {
	return this->angulo;
}
void Scenario::setAngulo(float angulo) {
	this->angulo = angulo;
}
SkyDome* Scenario::getSky() {
	return sky;
}
Terreno* Scenario::getTerreno() {
	return terreno;
}

Scenario::~Scenario() {
	if (this->sky != NULL) {
		delete this->sky;
		this->sky = NULL;
	}
	if (this->terreno != NULL) {
		delete this->terreno;
		this->terreno = NULL;
	}
	if (billBoard.size() > 0)
		for (int i = 0; i < billBoard.size(); i++)
			delete billBoard[i];
	if (billBoardAnim.size() > 0)
		for (int i = 0; i < billBoardAnim.size(); i++)
			delete billBoardAnim[i];
	if (billBoard2D.size() > 0)
		for (int i = 0; i < billBoard2D.size(); i++)
			delete billBoard2D[i];
	this->billBoard.clear();
	if (ourText.size() > 0)
		for (int i = 0; i < ourText.size(); i++)
			if (!(ourText[i]->name.compare("FPSCounter") || ourText[i]->name.compare("Coordenadas")))
				delete ourText[i];
	this->ourText.clear();
	if (ourModel.size() > 0)
		for (int i = 0; i < ourModel.size(); i++)
			if (ourModel[i] != camara)
			delete ourModel[i];
	this->ourModel.clear();
}
