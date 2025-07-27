#include <Windows.h>
#include "icbytes.h"
#include "icb_gui.h"
#include "ic_media.h"
#include <limits.h>
#include <stdio.h>

// Labirent tanımlamaları
ICBYTES labirent = {
	{2704, 50, 20, 80}, {210, 110, 140, 20}, {270, 170, 20, 80},
	{210, 230, 140, 20}, {270, 410, 20, 80}, {210, 470, 140, 20},
	{150, 170, 80, 20}, {330, 170, 80, 20}, {50, 470, 60, 20},
	{50, 530, 60, 40}, {150, 530, 80, 40}, {150, 230, 20, 80},
	{150, 350, 20, 140}, {150, 410, 80, 20}, {450, 470, 60, 20},
	{450, 530, 60, 40}, {330, 530, 80, 40}, {390, 230, 20, 80},
	{390, 350, 20, 140}, {330, 410, 80, 20}, {50, 50, 180, 20},
	{150, 50, 20, 80}, {330, 50, 180, 20}, {390, 50, 20, 80},
	{90, 110, 20, 80}, {50, 170, 60, 20}, {450, 110, 20, 80},
	{450, 170, 60, 20}
};

// Kafes tanımlamaları
ICBYTES cage = {
	{210, 290, 350, 290}, {350, 290, 350, 370}, {350,370,300,370},
	{300,370,300,360}, {300,360,340,360}, {340,360,340,300},
	{340,300,220,300}, {220,300,220,360}, {220,360,260,360},
	{260,360,260,370}, {260,370,210,370}, {210,370,210,290}
};

// Dış duvar tanımlamaları
ICBYTES cepheic = {
	{10,10,550,10}, {550,10,550,110}, {550,110,510,110},
	{510,110,510,130}, {510,130,550,130}, {550,130,550,230},
	{550,230,450,230}, {450,230,450,310}, {450,310,550,310},
	{550,310,550,350}, {550,350,450,350}, {450,350,450,430},
	{450,430,550,430}, {550,430,550,610}, {550,610,290,610},
	{290,610,290,530}, {290,530,270,530}, {270,530,270,610},
	{270,610,10,610}, {10,610,10,430}, {10,430,110,430},
	{110,430,110,350}, {110,350,10,350}, {10,350,10,310},
	{10,310,110,310}, {110,310,110,230}, {110,230,10,230},
	{10,230,10,130}, {10,130,50,130}, {50,130,50,110},
	{50, 110, 10, 110}, {10, 110, 10, 10}
};

// İç duvar tanımlamaları
ICBYTES cephedis = {
	{5,5,555,5}, {555,5,555,235}, {555,235,455,235},
	{455,235,455,305}, {455,305,555,305}, {555,305,555,355},
	{555,355,455,355}, {455,355,455,425}, {455,425,555,425},
	{555,425,555,615}, {555,615,5,615}, {5,615,5,425},
	{5,425,105,425}, {105,425,105,355}, {105,355,5,355},
	{5,355,5,305}, {5,305,105,305}, {105,305,105,235},
	{105,235,5,235}, {5,235,5,5}
};

// OYUN DEĞİŞKENLERİ ve GÖRÜNTÜ VERİLERİ
int FRM1, BTN;

// Skor ve can etiketlerinin tanıtıcıları
int SCORE_LABEL;
int LIVES_LABEL;
int madeBY;

// Ana görüntü ve arka plan
ICBYTES m;
ICBYTES bg;

// Pac-Man açık ağız resimleri
ICBYTES pacmanRight, pacmanLeft, pacmanUp, pacmanDown;
// Pac-Man kapalı ağız resimleri (animasyon için)
ICBYTES pacmanRightClosed, pacmanLeftClosed, pacmanUpClosed, pacmanDownClosed;
// Global ağız animasyon durumu; başlangıçta açık
bool mouthOpen = true;
// Ağız animasyon zamanlama kontrolü
DWORD lastMouthToggleTime = 0;
const DWORD MOUTH_TOGGLE_INTERVAL = 300; // ms

// Hayalet resimleri
ICBYTES ghostImages[4];
ICBYTES blueGhost;

// Oyun Bitti resmi
ICBYTES gameOver;

// Pac-Man ölüm animasyonu kareleri (11 kare)
ICBYTES pacmanDeathFrames[11];

// Oyun başlangıç ekranı resmi
ICBYTES gameStart;

// Pac-Man durumu
int keyPressed = 0, lastkeyPressed = 0;
int pacmanX = 345;
int pacmanY = 380;
bool pacmanIsDead = false;
int lives = 3;
const int INITIAL_PACMAN_X = 345;
const int INITIAL_PACMAN_Y = 380;

// Hayalet durumu
int ghostX[4] = { 344, 246, 275, 304 };
int ghostY[4] = { 250, 320, 320, 320 };
bool isGhostInCage[4] = { false, true, true, true };
int ghostDirectionY[4] = { 0, -1, -1, -1 };
bool ghostIsReturning[4] = { false, false, false, false }; // Hayaletler kafese geri dönüyor mu?
bool ghostIsBlue[4] = { false, false, false, false }; // Hayalet mavi mi kontrolü

bool isPowerUpActive = false;
DWORD powerUpStartTime = 0;
const DWORD POWER_UP_DURATION = 10000; // 10 saniye

// Yem durumu 28x30 ızgara
bool pelletMatrix[28][30];
int score = 0;

// Güçlendirici Yem yapısı ve dizisi
struct PowerPellet {
	int x;
	int y;
	int radius;
	bool eaten;
};

PowerPellet powerPellets[4] = {
	{30, 30, 8, false},
	{530, 30, 8, false},
	{30, 570, 8, false},
	{530, 570, 8, false}
};

bool powerPelletVisible[4] = { true, true, true, true };

// Zamanlayıcılar
HANDLE hTimerQueue = NULL;
HANDLE hTimer = NULL;
HANDLE hGhostTimer = NULL;
HANDLE hReleaseGhostsTimer = NULL;
HANDLE hCageReleaseTimer[4] = { NULL, NULL, NULL, NULL };
HANDLE hPowerPelletTimer = NULL;

// Fonksiyon Prototİpleri
void ICGUI_Create();
void WhenKeyPressed(int k);
void LoadGhostImages();
void LoadDeadPacmanAnımatıon();
void LoadSounds();
bool canMovePacman(int x, int y, int direction);
bool canMoveGhost(int x, int y, int direction);
bool isCollision(int x, int y, int w, int h);
bool isInsideCage(int x, int y);
void UpdateScene();
void DrawPellets(ICBYTES& m);
void CheckAndEatPellets();
void PlayDeathAnimation();
VOID CALLBACK pacmanMove(PVOID lpParam, BOOLEAN TimerOrWaitFired);
VOID CALLBACK GhostMoveCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);
VOID CALLBACK ReleaseGhostsCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);
VOID CALLBACK CageReleaseCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);
VOID CALLBACK PowerPelletBlinkCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);
void MoveGhosts();
void StartGhostMovement();
void Play();
void ICGUI_main();
void UpdateScoreLabel();
void UpdateLivesLabel();
void UpdatePowerPellets();
void ResetPacmanPosition();
void RestartGame();
void HandlePacmanDeath();
void ShowSplashScreen();

// GUI ve Oyun Başlatma Fonksiyonları
void ICGUI_Create() {
	ICG_MWTitle("Pac-Man");
	ICG_MWSize(1460, 700);
	ICG_SetFont(30, 12, "");
}

// Tuşa basıldığında yapılacaklar
void WhenKeyPressed(int k) {
	keyPressed = k;
}

// Hayalet resimlerini yükle
void LoadGhostImages() {
	ReadImage("ghost1.bmp", ghostImages[0]);
	ReadImage("ghost2.bmp", ghostImages[1]);
	ReadImage("ghost3.bmp", ghostImages[2]);
	ReadImage("ghost4.bmp", ghostImages[3]);
	ReadImage("blueghost.bmp", blueGhost);
}

// Oyun seslerini yükle
void LoadSounds() {
	// Başlangıç müziği Play() fonksiyonunda çalınıyor
}

// Pacman ölüm animasyonunu yükle
void LoadDeadPacmanAnımatıon() {
	ReadImage("pacmanDeadAnimation1.bmp", pacmanDeathFrames[0]);
	ReadImage("pacmanDeadAnimation2.bmp", pacmanDeathFrames[1]);
	ReadImage("pacmanDeadAnimation3.bmp", pacmanDeathFrames[2]);
	ReadImage("pacmanDeadAnimation4.bmp", pacmanDeathFrames[3]);
	ReadImage("pacmanDeadAnimation5.bmp", pacmanDeathFrames[4]);
	ReadImage("pacmanDeadAnimation6.bmp", pacmanDeathFrames[5]);
	ReadImage("pacmanDeadAnimation7.bmp", pacmanDeathFrames[6]);
	ReadImage("pacmanDeadAnimation8.bmp", pacmanDeathFrames[7]);
	ReadImage("pacmanDeadAnimation9.bmp", pacmanDeathFrames[8]);
	ReadImage("pacmanDeadAnimation10.bmp", pacmanDeathFrames[9]);
	ReadImage("pacmanDeadAnimation11.bmp", pacmanDeathFrames[10]);
}

// Çarpışma kontrolü
bool isCollision(int x, int y, int w, int h) {
	for (int i = x; i < x + w; i++) {
		for (int j = y; j < y + h; j++) {
			if (m.U(i, j) == 0x3080ff) {
				return true;
			}
		}
	}
	return false;
}

// Kafesin içinde mi kontrolü
bool isInsideCage(int x, int y) {
	const int n = 12;
	int polyX[12] = { 210, 350, 350, 300, 300, 340, 340, 220, 220, 260, 260, 210 };
	int polyY[12] = { 290, 290, 370, 370, 360, 360, 300, 300, 360, 360, 370, 370 };

	bool inside = false;
	for (int i = 0, j = n - 1; i < n; j = i++) {
		if (((polyY[i] > y) != (polyY[j] > y)) &&
			(x < (polyX[j] - polyX[i]) * (y - polyY[i]) / double(polyY[j] - polyY[i]) + polyX[i]))
		{
			inside = !inside;
		}
	}
	return inside;
}

// Skor etiketini güncelle
void UpdateScoreLabel() {
	char buffer[64];
	sprintf_s(buffer, "Skor: %d", score);
	ICG_SetWindowText(SCORE_LABEL, buffer);
}

// Can etiketini güncelle
void UpdateLivesLabel() {
	char buffer[64];
	sprintf_s(buffer, "Can: %d", lives);
	ICG_SetWindowText(LIVES_LABEL, buffer);
}

// Pac-Man hareket edebilir mi kontrolü
bool canMovePacman(int x, int y, int direction) {
	int newX = x, newY = y;
	switch (direction) {
	case 1: newX -= 3; break; // Sol
	case 2: newX += 3; break; // Sağ
	case 3: newY -= 3; break; // Yukarı
	case 4: newY += 3; break; // Aşağı
	}
	// Duvar çarpışma kontrolü
	if (isCollision(newX, newY, 25, 25)) {
		return false;
	}
	// Kafese giriş engeli
	if (newX >= 210 && newX <= 350 && newY >= 290 && newY <= 370) {
		return false;
	}
	return true;
}

// Hayalet hareket edebilir mi kontrolü
bool canMoveGhost(int x, int y, int direction) {
	int newX = x, newY = y;
	switch (direction) {
	case 1: newX -= 3; break;
	case 2: newX += 3; break;
	case 3: newY -= 3; break;
	case 4: newY += 3; break;
	}
	return !isCollision(newX, newY, 25, 25);
}

// Yemleri çiz
void DrawPellets(ICBYTES& m) {
	memset(pelletMatrix, false, sizeof(pelletMatrix));
	for (int x = 10; x < 560; x += 20) {
		for (int y = 10; y < 600; y += 20) {
			bool isInsideCageFlag = (x >= 210 && x <= 350 && y >= 290 && y <= 370);
			bool isInsideRect1 = (x >= 50 && x <= 110 && y >= 530 && y <= 570);
			bool isInsideRect2 = (x >= 150 && x <= 230 && y >= 530 && y <= 570);
			bool isInsideRect3 = (x >= 450 && x <= 510 && y >= 530 && y <= 570);
			bool isInsideRect4 = (x >= 330 && x <= 410 && y >= 530 && y <= 570);

			bool isInsideNewArea1 = (x >= 470 && x <= 550 && y >= 370 && y <= 410);
			bool isInsideNewArea2 = (x >= 470 && x <= 550 && y >= 240 && y <= 300);
			bool isInsideNewArea3 = (x >= 10 && x <= 100 && y >= 240 && y <= 300);
			bool isInsideNewArea4 = (x >= 10 && x <= 100 && y >= 370 && y <= 410);

			if (!isCollision(x, y, 5, 5) && !isInsideCageFlag &&
				!isInsideRect1 && !isInsideRect2 && !isInsideRect3 && !isInsideRect4 &&
				!isInsideNewArea1 && !isInsideNewArea2 && !isInsideNewArea3 && !isInsideNewArea4) {
				FillCircle(m, x, y, 3, 0xFFFF00);
				int gridX = (x - 10) / 20;
				int gridY = (y - 10) / 20;
				pelletMatrix[gridX][gridY] = true;
			}
		}
	}
	for (int i = 0; i < 4; i++) {
		if (!powerPellets[i].eaten) {
			FillCircle(m, powerPellets[i].x, powerPellets[i].y, powerPellets[i].radius, 0xFFFF00);
		}
	}
}

// Yemleri kontrol et ve ye
void CheckAndEatPellets() {
	int centerX = pacmanX + 12;
	int centerY = pacmanY + 12;

	int gridX = (centerX - 10) / 20;
	int gridY = (centerY - 10) / 20;
	for (int i = gridX - 1; i <= gridX + 1; i++) {
		for (int j = gridY - 1; j <= gridY + 1; j++) {
			if (i >= 0 && i < 28 && j >= 0 && j < 30 && pelletMatrix[i][j]) {
				int pelletX = i * 20 + 10;
				int pelletY = j * 20 + 10;
				int dx = centerX - pelletX;
				int dy = centerY - pelletY;
				if (dx * dx + dy * dy < 225) {
					pelletMatrix[i][j] = false;
					FillCircle(bg, pelletX, pelletY, 3, 0x000000);
					FillCircle(m, pelletX, pelletY, 3, 0x000000);
					score += 10;
					PlaySound("pacman_chomp.wav", NULL, SND_ASYNC);
				}
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		if (!powerPellets[i].eaten) {
			int dx = centerX - powerPellets[i].x;
			int dy = centerY - powerPellets[i].y;
			if (dx * dx + dy * dy < 225) {
				powerPellets[i].eaten = true;
				FillCircle(bg, powerPellets[i].x, powerPellets[i].y, powerPellets[i].radius, 0x000000);
				FillCircle(m, powerPellets[i].x, powerPellets[i].y, powerPellets[i].radius, 0x000000);
				score += 50;
				isPowerUpActive = true;
				powerUpStartTime = GetTickCount();

				PlaySound("pacman_intermission.wav", NULL, SND_SYNC); // Ara sahne sesi
			}
		}
	}
	UpdateScoreLabel();
}

// Ölüm animasyonunu oynat
void PlayDeathAnimation() {
	for (int i = 0; i < 11; i++) {
		m = bg;
		PasteNon0(pacmanDeathFrames[i], pacmanX, pacmanY, m);
		DisplayImage(FRM1, m);
		Sleep(100);
	}
}

// Sahneyi güncelle
void UpdateScene() {
	m = bg;
	// Yön tuşuna göre Pac-Man yönü ve ağız animasyonu
	if (lastkeyPressed == 37) {
		if (mouthOpen)
			PasteNon0(pacmanLeft, pacmanX, pacmanY, m);
		else
			PasteNon0(pacmanLeftClosed, pacmanX, pacmanY, m);
	}
	else if (lastkeyPressed == 39) {
		if (mouthOpen)
			PasteNon0(pacmanRight, pacmanX, pacmanY, m);
		else
			PasteNon0(pacmanRightClosed, pacmanX, pacmanY, m);
	}
	else if (lastkeyPressed == 38) {
		if (mouthOpen)
			PasteNon0(pacmanUp, pacmanX, pacmanY, m);
		else
			PasteNon0(pacmanUpClosed, pacmanX, pacmanY, m);
	}
	else if (lastkeyPressed == 40) {
		if (mouthOpen)
			PasteNon0(pacmanDown, pacmanX, pacmanY, m);
		else
			PasteNon0(pacmanDownClosed, pacmanX, pacmanY, m);
	}
	else {
		if (mouthOpen)
			PasteNon0(pacmanLeft, pacmanX, pacmanY, m);
		else
			PasteNon0(pacmanLeftClosed, pacmanX, pacmanY, m);
	}

	for (int i = 0; i < 4; i++) {
		if (isPowerUpActive && !isGhostInCage[i]) {
			PasteNon0(blueGhost, ghostX[i], ghostY[i], m);
		}
		else {
			PasteNon0(ghostImages[i], ghostX[i], ghostY[i], m);
		}
	}
	DisplayImage(FRM1, m);
}

// Pac-Man başlangıç pozisyonuna sıfırla
void ResetPacmanPosition() {
	pacmanX = INITIAL_PACMAN_X;
	pacmanY = INITIAL_PACMAN_Y;
	keyPressed = 0;
	lastkeyPressed = 0;
}

// Oyunu yeniden başlat
void RestartGame() {
	ResetPacmanPosition();

	ghostX[0] = 344; ghostY[0] = 250;
	ghostX[1] = 246; ghostY[1] = 320;
	ghostX[2] = 275; ghostY[2] = 320;
	ghostX[3] = 304; ghostY[3] = 320;

	isGhostInCage[0] = false;
	for (int i = 1; i < 4; i++) {
		isGhostInCage[i] = true;
		ghostDirectionY[i] = -1;
	}

	isPowerUpActive = false;

	if (hTimerQueue != NULL) {
		CreateTimerQueueTimer(&hTimer, hTimerQueue, pacmanMove, NULL, 1000, 30, 0);
		StartGhostMovement();
	}

	UpdateScene();
}

// Pac-Man ölümüyle ilgilen
void HandlePacmanDeath() {
	// Ölüm sesini çal
	PlaySound("pacman_death.wav", NULL, SND_ASYNC);
	PlayDeathAnimation();
	// Canı azalt ve etiketi güncelle
	lives--;
	UpdateLivesLabel();

	if (lives > 0) {
		RestartGame();
		pacmanIsDead = false;
	}
	else {
		m = gameOver;
		DisplayImage(FRM1, m);
	}
}

// Güçlendirici yem yanıp sönme animasyonu callback fonksiyonu
VOID CALLBACK PowerPelletBlinkCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired) {
	static bool pelletState = true; // Yem görünürlük durumu

	for (int i = 0; i < 4; i++) {
		if (!powerPellets[i].eaten) {
			powerPelletVisible[i] = pelletState;
		}
	}

	pelletState = !pelletState; // Durumu değiştir (true <-> false)

	UpdatePowerPellets(); // Ekranı güncelle
}

// Güçlendirici yemlerin görünürlüğünü güncelle
void UpdatePowerPellets() {
	for (int i = 0; i < 4; i++) {
		if (!powerPellets[i].eaten) {
			if (powerPelletVisible[i]) {
				FillCircle(m, powerPellets[i].x, powerPellets[i].y, powerPellets[i].radius, 0xFFFF00);
			}
			else {
				FillCircle(m, powerPellets[i].x, powerPellets[i].y, powerPellets[i].radius, 0x000000);
			}
		}
	}
	DisplayImage(FRM1, m);
}

// Pac-Man hareket zamanlayıcısı için callback
VOID CALLBACK pacmanMove(PVOID lpParam, BOOLEAN TimerOrWaitFired) {
	if (pacmanIsDead) return;
	if (isPowerUpActive && GetTickCount() - powerUpStartTime >= POWER_UP_DURATION) {
		isPowerUpActive = false;
		PlaySound(NULL, 0, 0); // Çalan tüm sesleri durdur
	}

	if (keyPressed != 0) {
		lastkeyPressed = keyPressed;
		keyPressed = 0;
	}
	if (lastkeyPressed == 0) return;

	int newX = pacmanX, newY = pacmanY;
	if (lastkeyPressed == 37) {
		if (canMovePacman(pacmanX, pacmanY, 1))
			newX = pacmanX - 3;
	}
	else if (lastkeyPressed == 39) {
		if (canMovePacman(pacmanX, pacmanY, 2))
			newX = pacmanX + 3;
	}
	else if (lastkeyPressed == 38) {
		if (canMovePacman(pacmanX, pacmanY, 3))
			newY = pacmanY - 3;
	}
	else if (lastkeyPressed == 40) {
		if (canMovePacman(pacmanX, pacmanY, 4))
			newY = pacmanY + 3;
	}

	pacmanX = newX;
	pacmanY = newY;

	CheckAndEatPellets();

	for (int i = 0; i < 4; i++) {
		if (abs(pacmanX - ghostX[i]) < 25 && abs(pacmanY - ghostY[i]) < 25) {
			if (isPowerUpActive && !isGhostInCage[i]) {
				PlaySound("pacman_eatghost.wav", NULL, SND_ASYNC);
				ghostIsReturning[i] = true;
				ghostIsBlue[i] = false; // Normal moda geri dön
				isGhostInCage[i] = false;
				ghostX[i] = 280;
				ghostY[i] = 320;
				score += 200;
				UpdateScoreLabel();
				CreateTimerQueueTimer(&hCageReleaseTimer[i], hTimerQueue,
					CageReleaseCallback, (PVOID)(intptr_t)i, 5000, 0, 0);
			}
			else {
				pacmanIsDead = true;
				DeleteTimerQueueTimer(hTimerQueue, hTimer, NULL);
				DeleteTimerQueueTimer(hTimerQueue, hGhostTimer, NULL);
				DeleteTimerQueueTimer(hTimerQueue, hReleaseGhostsTimer, NULL);
				HandlePacmanDeath();
				return;
			}
		}
	}

	// Ağız animasyonu: Belirli aralıklarla ağız durumunu değiştir
	DWORD currentTime = GetTickCount();
	if (currentTime - lastMouthToggleTime >= MOUTH_TOGGLE_INTERVAL) {
		mouthOpen = !mouthOpen;
		lastMouthToggleTime = currentTime;
	}

	UpdateScene();
}

// Hayaletleri hareket ettirme fonksiyonu
void MoveGhosts() {
	// Mesafe hesaplama fonksiyonu
	auto calculateDistance = [](int x1, int y1, int x2, int y2) -> int {
		return abs(x2 - x1) + abs(y2 - y1);
		};

	for (int i = 0; i < 4; i++) {
		int targetX, targetY;

		if (ghostIsReturning[i]) {
			// Hayalet kafese dönüyorsa hedef kafesin merkezi
			targetX = 280;
			targetY = 320;

			if (calculateDistance(ghostX[i], ghostY[i], targetX, targetY) < 10) {
				// Kafese ulaştığında yeniden doğmalı
				ghostIsReturning[i] = false;
				isGhostInCage[i] = true;
				ghostX[i] = 280;
				ghostY[i] = 320;

				// Kafese girdikten sonra belli bir süre bekleyip çıkmalı
				CreateTimerQueueTimer(&hCageReleaseTimer[i], hTimerQueue,
					CageReleaseCallback, (PVOID)(intptr_t)i, 3000, 0, 0);
				continue;
			}
		}
		else if (isGhostInCage[i]) {
			// Kafeste olan hayalet sadece yukarı-aşağı hareket eder
			if (ghostDirectionY[i] == -1) {
				if (ghostY[i] - 2 >= 300)
					ghostY[i] -= 2;
				else
					ghostDirectionY[i] = 1;
			}
			else {
				if (ghostY[i] + 2 <= 340)
					ghostY[i] += 2;
				else
					ghostDirectionY[i] = -1;
			}
			continue;
		}
		else {
			// Normal hayalet hareketi (Pac-Man'i kovalama)
			targetX = pacmanX;
			targetY = pacmanY;
		}

		int bestDirection = -1;
		int minDistance = INT_MAX;
		const int directions[4] = { 1, 2, 3, 4 };
		const int moveSpeed = 3;

		for (int dir : directions) {
			int newX = ghostX[i], newY = ghostY[i];

			switch (dir) {
			case 1: newX -= moveSpeed; break;
			case 2: newX += moveSpeed; break;
			case 3: newY -= moveSpeed; break;
			case 4: newY += moveSpeed; break;
			}

			if (!canMoveGhost(newX, newY, dir)) continue;

			int newDistance = calculateDistance(targetX, targetY, newX, newY);
			if (newDistance < minDistance) {
				minDistance = newDistance;
				bestDirection = dir;
			}
		}

		if (bestDirection != -1) {
			switch (bestDirection) {
			case 1: ghostX[i] -= moveSpeed; break;
			case 2: ghostX[i] += moveSpeed; break;
			case 3: ghostY[i] -= moveSpeed; break;
			case 4: ghostY[i] += moveSpeed; break;
			}
		}
	}

	UpdateScene();
}

// Hayalet hareket callback fonksiyonu
VOID CALLBACK GhostMoveCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired) {
	MoveGhosts();
}

// Hayaletleri serbest bırakma callback fonksiyonu
VOID CALLBACK ReleaseGhostsCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired) {
	if (isGhostInCage[0])
		isGhostInCage[0] = false;
}

// Kafes serbest bırakma callback fonksiyonu
VOID CALLBACK CageReleaseCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired) {
	int ghostIndex = (int)(intptr_t)lpParam;
	if (isGhostInCage[ghostIndex]) {
		FillRect(m, ghostX[ghostIndex], ghostY[ghostIndex], 25, 25, 0x000000);
		ghostX[ghostIndex] = 344;
		ghostY[ghostIndex] = 250;
		isGhostInCage[ghostIndex] = false;
		ghostIsBlue[ghostIndex] = false;
		PasteNon0(ghostImages[ghostIndex], ghostX[ghostIndex], ghostY[ghostIndex], m);
		DisplayImage(FRM1, m);
		hCageReleaseTimer[ghostIndex] = NULL;
	}
}

// Hayalet hareketini başlatma fonksiyonu
void StartGhostMovement() {
	isGhostInCage[0] = false;
	isGhostInCage[1] = true;
	isGhostInCage[2] = true;
	isGhostInCage[3] = true;

	CreateTimerQueueTimer(&hGhostTimer, hTimerQueue, GhostMoveCallback, NULL, 1000, 100, 0);

	for (int i = 1; i < 4; i++) {
		int delay = i * 5000;
		CreateTimerQueueTimer(&hReleaseGhostsTimer, hTimerQueue, [](PVOID lpParam, BOOLEAN TimerOrWaitFired) {
			int ghostIndex = (int)(intptr_t)lpParam;
			FillRect(m, ghostX[ghostIndex], ghostY[ghostIndex], 25, 25, 0x000000);
			ghostX[ghostIndex] = 344;
			ghostY[ghostIndex] = 250;
			isGhostInCage[ghostIndex] = false;
			PasteNon0(ghostImages[ghostIndex], ghostX[ghostIndex], ghostY[ghostIndex], m);
			DisplayImage(FRM1, m);
			}, (PVOID)(intptr_t)i, delay, 0, 0);
	}
}

// Başlangıç ekranını gösterme fonksiyonu
void ShowSplashScreen() {
	ReadImage("gameStart.bmp", gameStart);
	m = gameStart;
	DisplayImage(FRM1, m);
}

// Oyunu oynatma fonksiyonu
void Play() {
	// Başlangıç müziği
	PlaySound("pacman_beginning.wav", NULL, SND_SYNC);

	ReadImage("gameOver.bmp", gameOver);

	lives = 3;
	score = 0;
	pacmanIsDead = false;
	UpdateLivesLabel();
	UpdateScoreLabel();

	// Açık ağız Pac-Man resimleri
	ReadImage("pacmanRight.bmp", pacmanRight);
	ReadImage("pacmanLeft.bmp", pacmanLeft);
	ReadImage("pacmanUp.bmp", pacmanUp);
	ReadImage("pacmanDown.bmp", pacmanDown);
	// Kapalı ağız Pac-Man resimleri
	ReadImage("pacmanRightClosed.bmp", pacmanRightClosed);
	ReadImage("pacmanLeftClosed.bmp", pacmanLeftClosed);
	ReadImage("pacmanUpClosed.bmp", pacmanUpClosed);
	ReadImage("pacmanDownClosed.bmp", pacmanDownClosed);

	LoadGhostImages();
	LoadDeadPacmanAnımatıon();
	LoadSounds();

	CreateImage(m, 580, 630, ICB_UINT);
	m = 0;

	Line(m, cephedis, 0x3080ff);
	Line(m, cepheic, 0x3080ff);
	Line(m, cage, 0x3080ff);
	FillRect(m, labirent, 0x3080ff);
	FillRect(m, 260, 365, 40, 5, 0xFADBD8);

	DrawPellets(m);
	CreateImage(bg, 580, 630, ICB_UINT);
	bg = m;

	// İlk Pac-Man resmi
	PasteNon0(pacmanLeft, pacmanX, pacmanY, m);
	for (int i = 0; i < 4; i++) {
		PasteNon0(ghostImages[i], ghostX[i], ghostY[i], m);
	}
	DisplayImage(FRM1, m);

	hTimerQueue = CreateTimerQueue();
	CreateTimerQueueTimer(&hTimer, hTimerQueue, pacmanMove, NULL, 1000, 30, 0);
	StartGhostMovement();
	CreateTimerQueueTimer(&hPowerPelletTimer, hTimerQueue, PowerPelletBlinkCallback, NULL, 0, 200, WT_EXECUTEDEFAULT);

	SetFocus(ICG_GetMainWindow());
}

// ICGUI ana fonksiyonu
void ICGUI_main() {
	ICGUI_Create(); // GUI oluştur
	ICG_SetOnKeyPressed(WhenKeyPressed); // Tuşa basma olayını ayarla
	FRM1 = ICG_FrameMedium(5, 5, 580, 630); // Orta boy çerçeve oluştur
	ShowSplashScreen(); // Başlangıç ekranını göster
	BTN = ICG_Button(1100, 5, 200, 50, "Oyuna Başla", Play); // Oyuna Başla butonu
	SCORE_LABEL = ICG_Static(1100, 70, 150, 50, "Skor: 0"); // Skor etiketi
	LIVES_LABEL = ICG_Static(1100, 130, 150, 50, "Can: 3"); // Can etiketi
	madeBY = ICG_MLStatic(1100, 200, 500, 500, "Mert Büyüknisan 200101039\nİrem Seda Yılmaz 210101173\nÖykü Karakuzulu 200101059"); 
	UpdateLivesLabel(); // Can etiketini güncelle
}