#include <iostream>
#include <thread>
#include <queue>
#include <chrono>
#include <conio.h>
#include <mutex>
#include <vector>
#include<atomic>
using namespace std;

mutex queue_mtx;
mutex Print_mtx;
mutex Arithmetic_mtx;
atomic<int> length{ 10 };
queue<int> request;
int rest_of_cash[10000];
atomic<bool> flag{false};


void ctrl_C() {

	while (true) {

		if (_getch() == 3) {

			cout << "Shutting down!" << endl;

			flag = true;

			return;
		}

	}

}

void Generate(int limit_of_request) {

	int key = 1;

	while (flag == false && key <= limit_of_request) {


		this_thread::sleep_for(chrono::seconds(rand() % 1 + 1));
		queue_mtx.lock();
		if (flag == false) {

			if (request.size() < length) {

				request.push(key);
				key++;

			}

		}
		queue_mtx.unlock();
	}


}

void Arithmetic_event(int ind) {

	int time = 1;

	while (!flag) {
		queue_mtx.lock();
		if (request.size() > 0) {

			int q = request.front();

			request.pop();
			queue_mtx.unlock();

			time = rand() % 2 + 1;

			Print_mtx.lock();

			cout << "ATM " << ind << " processing request: " << q << endl;
			cout << "Time left: " << time << " seconds" << endl;

			Print_mtx.unlock();
			this_thread::sleep_for(chrono::seconds(time));


			if (!flag) {


				int withdraw = rand() % 100 + 1;

				if (withdraw > rest_of_cash[ind - 1]) {
					Print_mtx.lock();
					cout << "ATM " << ind << " does not have enough funds." << endl;
					Print_mtx.unlock();
				}
				else {
					Print_mtx.lock();

					Arithmetic_mtx.lock();
					rest_of_cash[ind - 1] = rest_of_cash[ind - 1] - withdraw;
					cout << "ATM " << ind << " is processing your request (" << q << "). Left in the ATM: " << rest_of_cash[ind - 1] << ". Money withdrawn: " << withdraw << "." << endl;
					Arithmetic_mtx.unlock();

					Print_mtx.unlock();
				}
			}
		}
		else {

			Print_mtx.lock();
			cout << "ATM " << ind << " is in standby mode." << endl;
			Print_mtx.unlock();

			queue_mtx.unlock();

			this_thread::sleep_for(chrono::seconds(1));

		}
	}

}

int main() {

	int num_of_atm;
	int limit;

	int limit_of_request = 1000;
	cout << "set the number of ATM (choose 2 or more): ";
	cin >> num_of_atm;
	cout << endl << ("set limit to ATM capacity: ");
	cin >> limit;

	for (int i = 0; i < num_of_atm; ++i) {
		rest_of_cash[i] = limit;
	}

	thread shutdown(ctrl_C);
	thread generator(Generate, limit_of_request);
	thread* arr_of_threads = new thread[num_of_atm];

	for (int i = 0; i < num_of_atm; ++i) {
		int key = i + 1;
		arr_of_threads[i] = thread(Arithmetic_event, key);

	}
	shutdown.join();
	for (int i = 0; i < num_of_atm; ++i) {
		
			arr_of_threads[i].join();

	}
	
	generator.join();
	cout << "The program is completed";
	return 0;
}