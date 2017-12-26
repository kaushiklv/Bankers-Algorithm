#include <bits/stdc++.h>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {

	int N, R, Q;

	ifstream inputFile(argv[1]);
	ofstream outputFile;
	outputFile.open(argv[2]);
	
	inputFile >> N >> R >> Q;

	vector<int> process(N), arrivalTime(N), available(R);
	vector<vector<int> > maximum, allocated, need, request;

	multimap<int, int> arrivalProcess;
	multimap<int, vector<int> > reqMap;
	multimap<float, vector<int> > multiReqMap;

	int time = 0;
	// Taking all the inputs from the input file ---------------------------------------------------------------------

	for(int i = 0; i < N; i++) {		
		inputFile >> process[i]; 
	}

	for(int i = 0; i < N; i++) {		
		inputFile >> arrivalTime[i]; 
	}

	for(int i = 0; i < N; i++) {
		arrivalProcess.insert({arrivalTime[i], process[i]});
	}

	for(int i = 0; i < R; i++) {		
		inputFile >> available[i]; 
	}

	for (int i = 0; i < N; i++)	{
		vector<int> temp;
		for (int j = 0; j < R; j++)	{
			int a;
			inputFile >> a;
			temp.push_back(a);
		}
		maximum.push_back(temp);
	}
	
	for (int i = 0; i < N; i++)	{
		vector<int> temp;
		for (int j = 0; j < R; j++)	{
			int a;
			inputFile >> a;
			temp.push_back(a);
		}
		allocated.push_back(temp);
	}
	
	for (int i = 0; i < maximum.size(); i++)	{
		vector<int> temp;
		for (int j = 0; j < maximum[i].size(); j++)	{
			int a;
			a = maximum[i][j] - allocated[i][j];
			temp.push_back(a);
		}
		need.push_back(temp);
	}	
	
	for (int i = 0; i < Q; i++) {
		vector<int> temp;
		for (int j = 0; j <= R; j++) {
			int a;
			inputFile >> a;
			temp.push_back(a);
		}
		request.push_back(temp);
	}

	for (int i = 0; i < Q; i++)	{
		int pr = 0;
		vector<int> temp;
		pr = request[i][0];
		for (int j = 1; j <= R; j++) {
			temp.push_back(request[i][j]);
		}
		reqMap.insert(make_pair(pr, temp));
	}

	float f = 0.0;
	vector<float> temp;

	for(multimap<int, vector<int> >::iterator i = reqMap.begin(); i != reqMap.end(); i++) {
		temp.push_back(i->first);
	}

	for(int i = 1; i < temp.size(); i++) {		
		if(temp[i] != floor(temp[i-1])) {
			f = 0.0;
		}
		else {
			 	f += 0.1;			
		}
		temp[i] += f;
	}

	for(multimap<float, vector<int> >::iterator i = multiReqMap.begin(); i != multiReqMap.end(); i++) {
		temp.push_back(i->first);
	}
	int j = 0;
	for(multimap<int, vector<int> >::iterator i = reqMap.begin(); i != reqMap.end(); i++,j++) {
		multiReqMap.insert(make_pair(temp[j], i->second));
	}

	// Done taking inputs ----------------------------------------------------------------------------------------------------
	
	// Fetch the Pid which is equal to AT from map1 and insert into set
	// Fetch the corresponding requests from map2
	// check for banker's conditions
	// If deny just inc timer
	// If partial grant, grant resources and inc timer
	// If full grant, release resources and inc timer

	set<float> readyQueue;
	set<float> denyQueue;
	vector<int> safeSequence;
	int grantCount = 0;
	int denyCount = 0;

	multimap<int, int>::iterator jk = arrivalProcess.begin();
	time = jk->first;
	for(auto i = arrivalProcess.begin(); i != arrivalProcess.end(); i++) {
		if(i->first == time) {
			for(auto re = multiReqMap.begin(); re != multiReqMap.end(); re++) {
				if(i->second == floor(re->first))
					readyQueue.insert(re->first);
			}
		}
	}

	// setting the maximum arrival time
	multimap<int, int>::iterator m = arrivalProcess.end();
	int maxArrival = m->first;
	set<float>::iterator pr;

	start:
	while(!readyQueue.empty()) {
		pr = readyQueue.begin();
		float p = *pr;
		readyQueue.erase(p);
		multimap<float, vector<int> >::iterator it = multiReqMap.find(p);
		if(it == multiReqMap.end())
			goto end;

		vector<int> request = it->second;

		int partialGrant = 0;
		int fullGrant = 0;
		int Grant = 0;
		int k;
		int denyFlag = 0;
		
		// Processing the request - 
		for(int i = 0; i < request.size(); i++) {				
			for(k = 0; k < N; k++) {
				if(process[k] == floor(p)) {
					break;
				}
			}

			// Normal case - Request can be granted
			if(request[i] > available[i] || request[i] > need[k][i]) {
				// Request is more than available (or) need then deny
				// denyFlag = 1;
				Grant = 0;
				//cout << p << " Deny\n";
				outputFile << "Deny\n";
				denyQueue.insert(p);
				denyCount++;
				break;
			}	
			else {
				Grant = 1;
			}						
		}


		if(Grant) {
			// If Grant is set make changes to available, need and allocated
			for (int i = 0; i < request.size(); i++) {
				available[i] -= request[i];
				need[k][i] -= request[i];
				allocated[k][i] += request[i];
			}	

			// Checking if its a partial or a full grant
			for(int j = 0; j < request.size(); j++) {
				if(need[k][j] != 0) {
					partialGrant = 1;
				}
			}

			if(partialGrant == 0)
				fullGrant = 1;			

			if(partialGrant) {
				//cout << p << " Grant\n";
				outputFile << "Grant\n";
				grantCount++;
				denyCount = 0;
				safeSequence.push_back(p);
				time++;
				// Push the next arriving process into the ready queue
				for(int i = 0; i < N; i++) {
					if(arrivalTime[i] == time) {
						multimap<int, int>::iterator it = arrivalProcess.find(time);
						for(auto re = multiReqMap.begin(); re != multiReqMap.end(); re++) {
							if(it->second == floor(re->first))
								readyQueue.insert(re->first);
						}
					}
				}
				// Push the denied processes back into ready queue
				while(!denyQueue.empty()) {
					set<float>::iterator itr = denyQueue.begin();
					readyQueue.insert(*itr);
					denyQueue.erase(*itr);
				}
				for(auto it = multiReqMap.begin(); it != multiReqMap.end(); it++) {
					if(it->first == p) {
						multiReqMap.erase(p);
						break;
					}
				}
			}
			else if(fullGrant) {
				// cout << p << " Grant\n";
				outputFile << "Grant\n";
				grantCount++;
				denyCount = 0;
				// Release its resources
				for(int i = 0; i < request.size(); i++) {
					available[i] += allocated[k][i];
				}
				safeSequence.push_back(p);

				// Push the denied processes back into ready queue
				while(!denyQueue.empty()) {				
					set<float>::iterator itr = denyQueue.begin();
					readyQueue.insert(*itr);
					denyQueue.erase(*itr);
				}
				// Erase the request from the multimap
				for(auto it = multiReqMap.begin(); it != multiReqMap.end(); it++) {
					if(it->first == p) {
						multiReqMap.erase(p);
						break;
					}
				}
				time++;
				// Push the next arriving process into the ready queue
				for(int i = 0; i < N; i++) {
					if(arrivalTime[i] == time) {
						multimap<int, int>::iterator it = arrivalProcess.find(time);
						for(auto re = multiReqMap.begin(); re != multiReqMap.end(); re++) {
							if(it->second == floor(re->first))
								readyQueue.insert(re->first);
						}
					}
				}
			}				
		}
		// if(denyCount + grantCount > Q && time > maxArrival)
		// 	goto end;
		if(denyQueue.size() == N && time > maxArrival)
			goto end;
	}
	// If ready queue is empty wait till the next process arrives
	while (readyQueue.empty()) {
		time++;
		while(!denyQueue.empty()) {
			set<float>::iterator itr = denyQueue.begin();
			readyQueue.insert(*itr);
			denyQueue.erase(*itr);
		}
		for(int i = 0; i < N; i++) {
			// If process arrived schedule it
			if(arrivalTime[i] == time) {
				multimap<int, int>::iterator it = arrivalProcess.find(time);
				for(auto re = multiReqMap.begin(); re != multiReqMap.end(); re++) {
					if(it->second == floor(re->first))
						readyQueue.insert(re->first);
				}
			}
		}
		if(!multiReqMap.empty())
			goto start;
		else
			goto end;
	}
	end:
	// Printing the safe sequence
	if(safeSequence.size() == Q){
		for (int i = 0; i < safeSequence.size(); ++i)
		{
			//cout << safeSequence[i] << " ";
			outputFile << safeSequence[i] << " ";
		}
	}
	else if(safeSequence.size() != Q) {
		//cout << "Not Safe\n";
		outputFile << "Not Safe\n";	
	}


	// // End of Taking inputs  -------------------------------------------------------------------------------------------

	// for(int i = 0; i < Q; i++) {
	// 	int pr = request[i][0];
	// 	for (int j = 0; j < R; ++j)
	// 	{
	// 		if(request[i+1][j] > need[i][j])
	// 			cout << "Deny\n";
	// 		else if(request[i+1][j] > available[j])
	// 			cout << "Deny\n";
	// 		else {
	// 			available[j] -= request[i+1][j];
	// 			allocated[i][j] += request[i+1][j];
	// 			need[i][j] -= request[i+1][j];
	// 		}
	// 	}
	// }
	// for(int i = 0; i < R; i++) {
	// 	cout << available[i] << " " << arrivalTime[i] << "\n";
	// }

	// cout << "Available -> ";
		// for(int i = 0; i < available.size(); i++) {
		// 	cout << available[i] << " ";
		// }
		// cout << endl;
		// cout << "Request -> ";
		// for(int i = 0; i < request.size(); i++) {
		// 	cout << request[i] << " ";
		// }
		// cout << endl;
		// cout << "Need -> ";
		// for(int i = 0; i < need[p].size(); i++) {
		// 	cout << need[p][i] << " ";
		// }
		// cout << endl;
		// for(int r = 0; r < Q; r++) {

	return 0;
}