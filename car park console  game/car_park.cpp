#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>

using namespace std;

class Car {
public:
    string id;
    int x, y;

    Car(string id, int x, int y) : id(id), x(x), y(y) {}
};

class ParkingLot {
private:
    vector<vector<string>> lot;
    vector<Car> cars;
    vector<pair<int, int>> obstacles;
    int rows, cols;
    int level;
    pair<int, int> exitPoint;
    pair<int, int> playerPosition;
    string selectedCarId;

    void placeCar(Car& car) {
        int x, y;
        do {
            x = rand() % rows;
            y = rand() % cols;
        } while (lot[x][y] != "." || (x == 0 && y == 0) || isObstacle(x, y));
        lot[x][y] = car.id;
        car.x = x;
        car.y = y;
    }

    void placeExitPoint() {
        lot[0][0] = "0";
        exitPoint = {0, 0};
    }

    void placeObstacles() {
        int numObstacles = level * 2;  // Increase the number of obstacles with level
        obstacles.clear();

        for (int i = 0; i < numObstacles; ++i) {
            int x, y;
            do {
                x = rand() % rows;
                y = rand() % cols;
            } while (lot[x][y] != "." || (x == 0 && y == 0) || isObstacle(x, y));

            lot[x][y] = "X";  // X represents an obstacle
            obstacles.emplace_back(x, y);
        }
    }

    bool isObstacle(int x, int y) const {
        for (const auto& obstacle : obstacles) {
            if (obstacle.first == x && obstacle.second == y) {
                return true;
            }
        }
        return false;
    }

public:
    ParkingLot(int level) : rows(5 + level * 2), cols(5 + level * 2), level(level) {
        lot.resize(rows, vector<string>(cols, "."));
        int numCars = level * 2 + 3;  // Increase the number of cars with level

        placeExitPoint();
        placeObstacles();

        for (int i = 1; i <= numCars; ++i) {
            Car car("C" + to_string(i), 0, 0);
            placeCar(car);
            cars.push_back(car);
        }

        do {
            playerPosition = {rand() % rows, rand() % cols};
        } while (lot[playerPosition.first][playerPosition.second] != ".");
    }

    void displayLot() const {
#ifdef _WIN32
        system("cls");  // Use "cls" for Windows
#else
        system("clear");  // Use "clear" for Unix/Linux/Mac
#endif
        cout << "----------------------------------------" << endl;
        cout << "|          Welcome to Parking Lot       |" << endl;
        cout << "|                Level " << level << "                |" << endl;
        cout << "----------------------------------------" << endl;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (i == playerPosition.first && j == playerPosition.second) {
                    cout << (lot[i][j] == "0" ? "0 " : "P ");
                } else {
                    cout << lot[i][j] << " ";
                }
            }
            cout << endl;
        }
        cout << "----------------------------------------" << endl;
    }

    void movePlayer(char direction) {
        int newX = playerPosition.first;
        int newY = playerPosition.second;

        switch (direction) {
            case 'w': if (newX > 0) newX--; break;
            case 's': if (newX < rows - 1) newX++; break;
            case 'a': if (newY > 0) newY--; break;
            case 'd': if (newY < cols - 1) newY++; break;
            default:
                cout << "Invalid direction. Use W, A, S, or D." << endl;
                return;
        }

        if (lot[newX][newY] == "." || lot[newX][newY] == "0") {
            playerPosition = {newX, newY};
        } else {
            cout << "You can't move there!" << endl;
        }
    }

    bool removeCar() {
        int x = playerPosition.first;
        int y = playerPosition.second;

        if (lot[x][y] != "." && lot[x][y] != "0" && lot[x][y] != "X") {
            string carId = lot[x][y];
            lot[x][y] = ".";

            auto it = remove_if(cars.begin(), cars.end(), [&](const Car& car) {
                return car.id == carId;
            });
            cars.erase(it, cars.end());

            cout << "Removed car with ID " << carId << endl;
            return true;
        } else {
            cout << "No car at your current position." << endl;
            return false;
        }
    }

    bool isCleared() const {
        return cars.empty();
    }

    bool isAtExit() const {
        return playerPosition == exitPoint;
    }

    bool selectCar(int x, int y) {
        if (x < 0 || x >= rows || y < 0 || y >= cols) {
            cout << "Invalid position. Try again." << endl;
            return false;
        }

        string carId = lot[x][y];
        if (carId == "." || carId == "0" || carId == "X") {
            cout << "You selected an empty space, obstacle, or the exit point. Please select a car!" << endl;
            return false;
        }

        selectedCarId = carId;
        playerPosition = {x, y};
        cout << "Selected car with ID " << selectedCarId << endl;
        return true;
    }

    void moveSelectedCar(char direction) {
        int newX = playerPosition.first;
        int newY = playerPosition.second;

        switch (direction) {
            case 'w': if (newX > 0) newX--; break;
            case 's': if (newX < rows - 1) newX++; break;
            case 'a': if (newY > 0) newY--; break;
            case 'd': if (newY < cols - 1) newY++; break;
            default:
                cout << "Invalid direction. Use W, A, S, or D." << endl;
                return;
        }

        if (lot[newX][newY] == "." || lot[newX][newY] == "0") {
            lot[playerPosition.first][playerPosition.second] = ".";
            playerPosition = {newX, newY};
            lot[playerPosition.first][playerPosition.second] = selectedCarId;
        } else {
            cout << "Move blocked by another car or obstacle!" << endl;
        }
    }
};

class Game {
private:
    int level;
    ParkingLot* parkingLot;

public:
    Game() : level(1), parkingLot(nullptr) {
        srand(static_cast<unsigned>(time(0)));
    }

    ~Game() {
        delete parkingLot;
    }

    void start() {
        while (true) {
            parkingLot = new ParkingLot(level);

            while (true) {
                parkingLot->displayLot();

                if (parkingLot->isCleared() && parkingLot->isAtExit()) {
                    cout << "----------------------------------------" << endl;
                    cout << "|          Congratulations!            |" << endl;
                    cout << "|       You've cleared level " << level << "        |" << endl;
                    cout << "|     Now moving to level " << level + 1 << "      |" << endl;
                    cout << "----------------------------------------" << endl;
                    
                    char choice;
                    cout << "Do you want to continue to the next level? (Y to continue, Q to quit): ";
                    cin >> choice;

                    if (choice == 'Y' || choice == 'y') {
                        ++level;
                        break;  // Move to the next level
                    } else if (choice == 'Q' || choice == 'q') {
                        cout << "Exiting the game. Thank you for playing!" << endl;
                        return;  // Exit the game
                    } else {
                        cout << "Invalid choice. Exiting the game." << endl;
                        return;  // Exit if any other key is pressed
                    }
                } else if (parkingLot->isCleared()) {
                    cout << "All cars are removed. Now head to the exit!" << endl;
                }

                int x, y;
                cout << "Enter the row and column of the car you want to select (e.g., 2 3): ";
                cin >> x >> y;

                if (!parkingLot->selectCar(x - 1, y - 1)) {
                    continue;  // Retry selection if invalid
                }

                char move;
                while (true) {
                    parkingLot->displayLot();
                    cout << "Move the car using W (up), S (down), A (left), D (right): ";
                    cin >> move;

                    if (move == 'Q' || move == 'q') {
                        cout << "Exiting the game." << endl;
                        return;  // Exit if user presses Q
                    }

                    if (move == 'R' || move == 'r') {
                        if (parkingLot->removeCar()) {
                            break;  // Exit inner loop to select another car
                        }
                    } else if (move == 'w' || move == 's' || move == 'a' || move == 'd') {
                        parkingLot->moveSelectedCar(move);
                        if (parkingLot->isAtExit()) {
                            break;  // Exit inner loop if reached exit
                        }
                    } else {
                        cout << "Invalid move. Please use W, A, S, or D." << endl;
                    }
                }
            }

            delete parkingLot;
        }
    }
};

int main() {
    Game game;
    game.start();
    return 0;
}
