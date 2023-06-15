#include <iostream>
#include <fstream>
#include <list>
#include <queue>
#include <stack>
#include <mutex>
#include <condition_variable>
#include <thread>

std::list<int>* myList; // �������� �� ������
std::queue<int>* myQueue; // �������� �� �����
std::stack<int>* myStack; // �������� �� ����
std::mutex listMtx; // �'����� ��� ������� �� ������
std::mutex queueMtx; // �'����� ��� ������� �� �����
std::mutex stackMtx; // �'����� ��� ������� �� �����
std::condition_variable cv; // ������ ����� ��� ��������� ������

// ������� ��� ��������� �������� �� ������
void listPush(int value) {
  std::unique_lock<std::mutex> lock(listMtx); // ���������� �'����� ������
  cv.wait(lock, [] { return myList->size() < 10; }); // ������, ���� ������ �� ���� ���� ����� 10 ��������
  myList->push_back(value);
  std::cout << "������ �� ������: " << value << std::endl;
  cv.notify_all(); // �������� �� ������, �� ������ ��� �������
}

// ������� ��� ��������� �������� � ������
void listPop() {
  std::unique_lock<std::mutex> lock(listMtx); // ���������� �'����� ������
  cv.wait(lock, [] { return !myList->empty(); }); // ������, ���� ������ �� ����� ��������
  int value = myList->front();
  myList->pop_front();
  std::cout << "�������� � ������: " << value << std::endl;
  cv.notify_all(); // �������� �� ������, �� ������ ��� �������
}

// ������� ��� ��������� �������� �� �����
void queueEnqueue(int value) {
  std::unique_lock<std::mutex> lock(queueMtx); // ���������� �'����� �����
  cv.wait(lock, [] { return myQueue->size() < 10; }); // ������, ���� ����� �� ���� ���� ����� 10 ��������
  myQueue->push(value);
  std::cout << "������ �� �����: " << value << std::endl;
  cv.notify_all(); // �������� �� ������, �� ����� ���� ������
}

// ������� ��� ��������� �������� � �����
void queueDequeue() {
  std::unique_lock<std::mutex> lock(queueMtx); // ���������� �'����� �����
  cv.wait(lock, [] { return !myQueue->empty(); }); // ������, ���� ����� �� ����� ��������
  int value = myQueue->front();
  myQueue->pop();
  std::cout << "�������� � �����: " << value << std::endl;
  cv.notify_all(); // �������� �� ������, �� ����� ���� ������
}

// ������� ��� ��������� �������� �� �����
void stackPush(int value) {
  std::unique_lock<std::mutex> lock(stackMtx); // ���������� �'����� �����
  cv.wait(lock, [] { return myStack->size() < 10; }); // ������, ���� ���� �� ���� ���� ����� 10 ��������
  myStack->push(value);
  std::cout << "������ �� �����: " << value << std::endl;
  cv.notify_all(); // �������� �� ������, �� ���� ��� �������
}

// ������� ��� ��������� �������� � �����
void stackPop() {
  std::unique_lock<std::mutex> lock(stackMtx); // ���������� �'����� �����
  cv.wait(lock, [] { return !myStack->empty(); }); // ������, ���� ���� �� ����� ��������
  int value = myStack->top();
  myStack->pop();
  std::cout << "�������� � �����: " << value << std::endl;
  cv.notify_all(); // �������� �� ������, �� ���� ��� �������
}

// ������� ��� ���������� ����� � ����� �� ��������� �� �� ������
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
    std::cout << "�� ������� ������� ����: " << filename << std::endl;
  }
}

// ������� ��� ���������� ����� � ����� �� ��������� �� �� �����
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
    std::cout << "�� ������� ������� ����: " << filename << std::endl;
  }
}

// ������� ��� ���������� ����� � ����� �� ��������� �� �� �����
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
    std::cout << "�� ������� ������� ����: " << filename << std::endl;
  }
}

int main() {
  myList = new std::list<int>(); // �������� ���'��� �� ������
  myQueue = new std::queue<int>(); // �������� ���'��� �� �����
  myStack = new std::stack<int>(); // �������� ���'��� �� ����

  std::thread listReader(readListFromFile, "list.txt"); // ���� ��� ���������� � ����� �� ������
  std::thread queueReader(readQueueFromFile, "queue.txt"); // ���� ��� ���������� � ����� �� �����
  std::thread stackReader(readStackFromFile, "stack.txt"); // ���� ��� ���������� � ����� �� �����

  // ������� ���������� ������
  listReader.join(); // ������� ���������� ������ ��� ������
  queueReader.join(); // ������� ���������� ������ ��� �����
  stackReader.join(); // ������� ���������� ������ ��� �����

  // ��������� � 1 �� 4 �������� � �����
  for (int i = 0; i < 4; i++) {
    stackPop();
  }

  // ��������� 1 ������� � �����
  queueDequeue();

  // ��������� �������� ������
  std::cout << "�������� ������: ";
  for (int value : *myList) {
    std::cout << value << " ";
  }
  std::cout << std::endl;

  // ��������� �������� �����
  std::cout << "�������� �����: ";
  while (!myQueue->empty()) {
    std::cout << myQueue->front() << " ";
    myQueue->pop();
  }
  std::cout << std::endl;

  // ��������� �������� �����
  std::cout << "�������� �����: ";
  while (!myStack->empty()) {
    std::cout << myStack->top() << " ";
    myStack->pop();
  }
  std::cout << std::endl;

  delete myList; // ��������� ���'���, ������� �� ������
  delete myQueue; // ��������� ���'���, ������� �� �����
  delete myStack; // ��������� ���'���, ������� �� ����

  return 0;
}
