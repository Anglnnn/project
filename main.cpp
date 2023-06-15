#include <iostream>
#include <fstream>
#include <list>
#include <queue>
#include <stack>
#include <mutex>
#include <condition_variable>
#include <thread>

std::list<int>* myList; // Вказівник на список
std::queue<int>* myQueue; // Вказівник на чергу
std::stack<int>* myStack; // Вказівник на стек
std::mutex listMtx; // М'ютекс для доступу до списку
std::mutex queueMtx; // М'ютекс для доступу до черги
std::mutex stackMtx; // М'ютекс для доступу до стеку
std::condition_variable cv; // Умовна змінна для сповіщення потоків

// Функція для додавання елемента до списку
void listPush(int value) {
  std::unique_lock<std::mutex> lock(listMtx); // Захоплюємо м'ютекс списку
  cv.wait(lock, [] { return myList->size() < 10; }); // Чекаємо, доки список не буде мати менше 10 елементів
  myList->push_back(value);
  std::cout << "Додано до списку: " << value << std::endl;
  cv.notify_all(); // Сповіщаємо всі потоки, що список був змінений
}

// Функція для видалення елемента зі списку
void listPop() {
  std::unique_lock<std::mutex> lock(listMtx); // Захоплюємо м'ютекс списку
  cv.wait(lock, [] { return !myList->empty(); }); // Чекаємо, доки список не стане непустим
  int value = myList->front();
  myList->pop_front();
  std::cout << "Видалено зі списку: " << value << std::endl;
  cv.notify_all(); // Сповіщаємо всі потоки, що список був змінений
}

// Функція для додавання елемента до черги
void queueEnqueue(int value) {
  std::unique_lock<std::mutex> lock(queueMtx); // Захоплюємо м'ютекс черги
  cv.wait(lock, [] { return myQueue->size() < 10; }); // Чекаємо, доки черга не буде мати менше 10 елементів
  myQueue->push(value);
  std::cout << "Додано до черги: " << value << std::endl;
  cv.notify_all(); // Сповіщаємо всі потоки, що черга була змінена
}

// Функція для видалення елемента з черги
void queueDequeue() {
  std::unique_lock<std::mutex> lock(queueMtx); // Захоплюємо м'ютекс черги
  cv.wait(lock, [] { return !myQueue->empty(); }); // Чекаємо, доки черга не стане непустою
  int value = myQueue->front();
  myQueue->pop();
  std::cout << "Видалено з черги: " << value << std::endl;
  cv.notify_all(); // Сповіщаємо всі потоки, що черга була змінена
}

// Функція для додавання елемента до стеку
void stackPush(int value) {
  std::unique_lock<std::mutex> lock(stackMtx); // Захоплюємо м'ютекс стеку
  cv.wait(lock, [] { return myStack->size() < 10; }); // Чекаємо, доки стек не буде мати менше 10 елементів
  myStack->push(value);
  std::cout << "Додано до стеку: " << value << std::endl;
  cv.notify_all(); // Сповіщаємо всі потоки, що стек був змінений
}

// Функція для видалення елемента зі стеку
void stackPop() {
  std::unique_lock<std::mutex> lock(stackMtx); // Захоплюємо м'ютекс стеку
  cv.wait(lock, [] { return !myStack->empty(); }); // Чекаємо, доки стек не стане непустим
  int value = myStack->top();
  myStack->pop();
  std::cout << "Видалено зі стеку: " << value << std::endl;
  cv.notify_all(); // Сповіщаємо всі потоки, що стек був змінений
}

// Функція для зчитування даних з файлу та додавання їх до списку
void readListFromFile(const std::string& filename) {
  std::ifstream file(filename);
  if (file.is_open()) {
    int value;
    while (file >> value) {
      listPush(value);
    }
    file.close();
  }
  else {
    std::cout << "Не вдалося відкрити файл: " << filename << std::endl;
  }
}

// Функція для зчитування даних з файлу та додавання їх до черги
void readQueueFromFile(const std::string& filename) {
  std::ifstream file(filename);
  if (file.is_open()) {
    int value;
    while (file >> value) {
      queueEnqueue(value);
    }
    file.close();
  }
  else {
    std::cout << "Не вдалося відкрити файл: " << filename << std::endl;
  }
}

// Функція для зчитування даних з файлу та додавання їх до стеку
void readStackFromFile(const std::string& filename) {
  std::ifstream file(filename);
  if (file.is_open()) {
    int value;
    while (file >> value) {
      stackPush(value);
    }
    file.close();
  }
  else {
    std::cout << "Не вдалося відкрити файл: " << filename << std::endl;
  }
}

int main() {
  myList = new std::list<int>(); // Виділяємо пам'ять під список
  myQueue = new std::queue<int>(); // Виділяємо пам'ять під чергу
  myStack = new std::stack<int>(); // Виділяємо пам'ять під стек

  std::thread listReader(readListFromFile, "list.txt"); // Потік для зчитування з файлу до списку
  std::thread queueReader(readQueueFromFile, "queue.txt"); // Потік для зчитування з файлу до черги
  std::thread stackReader(readStackFromFile, "stack.txt"); // Потік для зчитування з файлу до стеку

  // Очікуємо завершення потоків
  listReader.join(); // Очікуємо завершення потоку для списку
  queueReader.join(); // Очікуємо завершення потоку для черги
  stackReader.join(); // Очікуємо завершення потоку для стеку

  // Видаляємо з 1 по 4 елементи зі стеку
  for (int i = 0; i < 4; i++) {
    stackPop();
  }

  // Видаляємо 1 елемент з черги
  queueDequeue();

  // Виведення елементів списку
  std::cout << "Елементи списку: ";
  for (int value : *myList) {
    std::cout << value << " ";
  }
  std::cout << std::endl;

  // Виведення елементів черги
  std::cout << "Елементи черги: ";
  while (!myQueue->empty()) {
    std::cout << myQueue->front() << " ";
    myQueue->pop();
  }
  std::cout << std::endl;

  // Виведення елементів стеку
  std::cout << "Елементи стеку: ";
  while (!myStack->empty()) {
    std::cout << myStack->top() << " ";
    myStack->pop();
  }
  std::cout << std::endl;

  delete myList; // Звільняємо пам'ять, виділену під список
  delete myQueue; // Звільняємо пам'ять, виділену під чергу
  delete myStack; // Звільняємо пам'ять, виділену під стек

  return 0;
}
