
Pacman Clone with Parallel Programming

A multithreaded version of the classic Pacman game, developed using C++ and the WinAPI. This project utilizes parallel programming techniques to manage the concurrent movements of the characters.

🚀 About This Project
This project aims to recreate the fundamental mechanics of the legendary Pacman game using modern C++ standards. The most prominent feature of this project is that Pacman and each of the ghosts run on their own separate threads. This allows the characters to move independently and concurrently, providing a practical application of parallel programming concepts.

✨ Key Features
Classic Gameplay: Navigate through a maze, collect pellets, and avoid the ghosts.

Parallel Movement: Pacman and the four ghosts each operate on a separate thread, ensuring fluid and independent motion.

Power Pellets: Eat the large pellets to make the ghosts vulnerable for a short time, giving you a chance to eat them.

Graphics and Sound: The game is enhanced with bmp images displayed in the console and features classic wav sound effects.

Game States: Includes start, gameplay, and game over screens.

🛠️ Technical Details and Technologies Used
This project was developed to demonstrate the principles of parallel programming.

Language: C++

API: Windows API (WinAPI) - Used for console graphics, keyboard input, and sound operations.

Parallelism:

<thread>: Separate threads are created for the movement logic of Pacman and each ghost.

<mutex>: Mutexes are used to ensure data consistency and prevent race conditions when accessing shared resources like the game map and score.

📸 Screenshots

<img width="500" height="500" alt="Ekran görüntüsü" src="https://github.com/user-attachments/assets/e73371e4-bbae-440d-b6be-aa12d63a9e03" />

<img width="500" height="500" alt="Ekran görüntüsü" src="https://github.com/user-attachments/assets/20565f22-8c8f-4979-8254-03d0ccd70045" />

<img width="500" height="500" alt="Ekran görüntüsü" src="https://github.com/user-attachments/assets/726ebee4-b3b4-496a-a3e2-dc60268d6b7f" />

<img width="500" height="500" alt="Ekran görüntüsü" src="https://github.com/user-attachments/assets/ba1a3654-b6e1-4720-9e04-d967bf03ea5d" />
