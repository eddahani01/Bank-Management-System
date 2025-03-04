

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>
#include <iomanip>

using namespace std;

const string ClientstFileName = "ClientsData.txt";
const string UserstFileName = "Users.txt";


struct stUser
{
	string Username;
	string Password;
	int Permissions;
	bool MarkForDelete = false;
};

stUser CurrentUser;

enum enMainMenuOptions {
	eListClient = 1, eAddNewClient = 2,
	eDeleteClient = 3, eUpdateClient = 4,
	eFindClient = 5, eShowTransactionsMenu = 6,
	eManageUsers = 7, eExit = 8
};

enum enMainMenuPermissions {
	eAll = -1, pListClients = 1, pAddNewClient = 2, pDeleteClient = 4,
	pUpdateClients = 8, pFindClient = 16, pTransactions = 32, pManageUsers = 64
};

void ShowMainMenu();
void GoBackToMainMenu();
void GoBackToTransactionsMenu();
void ShowManageUsersScreen();
void ShowAccessDeniedMessage();
bool CheckAccessPermission(enMainMenuPermissions Permission);
void Login();


struct stClient
{
	string AccountNumber;
	string PinCode;
	string ClientName;
	string Phone;
	double AccountBalance = 0;
	bool MarkForDelete = false;
};

vector <string> SplitString(string S1, string delimiter)
{
	vector <string> vString;

	short pos = 0;
	string sWord; // define a string variable

	// use find() function to get the position of the delimiter

	while ((pos = S1.find(delimiter)) != std::string::npos)
	{
		sWord = S1.substr(0, pos); //store the word
		if (sWord != "")
			vString.push_back(sWord);


		S1.erase(0, pos + delimiter.length()); // erase() until position and move to the next word
		// we use delimiter.length() because delimiter can also be many character or symbols like (#,,#) (,,) ...

	}
	if (S1 != "")
		vString.push_back(S1); // it add last word ofthe string

	return vString;
}

stClient ConvertLineToRecord(string Line, string Seperator = "#//#")
{
	stClient Client;
	vector <string> vClientData;

	vClientData = SplitString(Line, Seperator);
	Client.AccountNumber = vClientData[0];
	Client.PinCode = vClientData[1];
	Client.ClientName = vClientData[2];
	Client.Phone = vClientData[3];
	Client.AccountBalance = stod(vClientData[4]);

	return Client;
}

string ConvertRecordToLine(stClient Client, string Seperator = "#//#")
{
	string stClientRecord = "";

	stClientRecord += Client.AccountNumber + Seperator;
	stClientRecord += Client.PinCode + Seperator;
	stClientRecord += Client.ClientName + Seperator;
	stClientRecord += Client.Phone + Seperator;
	stClientRecord += to_string(Client.AccountBalance);

	return stClientRecord;

}

vector <stClient> LoadClientDataFromFile(string FileName)
{
	vector <stClient> vClients;

	fstream MyFile;
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string line;
		stClient Client;

		while (getline(MyFile, line))
		{
			Client = ConvertLineToRecord(line);
			vClients.push_back(Client);
		}
		MyFile.close();
	}
	return vClients;
}

bool ClientExistsByAccountNumber(string AccountNumber, string FileName)
{
	vector <stClient> vClients;

	fstream MyFile;

	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string line;
		stClient Client;

		while (getline(MyFile, line))
		{
			Client = ConvertLineToRecord(line);
			if (Client.AccountNumber == AccountNumber)
			{
				MyFile.close();
				return true;
			}
			vClients.push_back(Client);
		}
		MyFile.close();
	}
	return false;
}

stClient ReadNewClient()
{
	stClient Client;



	cout << "\nEnter Account Number? ";

	//Usage of std::ws will extract all the whitespace character
	getline(cin >> ws, Client.AccountNumber);

	while (ClientExistsByAccountNumber(Client.AccountNumber, ClientstFileName))
	{
		cout << "\nClient with [" << Client.AccountNumber << "] already exists, Enter another Account Number? ";
		getline(cin >> ws, Client.AccountNumber);
	}

	cout << "\nEnter PinCode? ";
	getline(cin, Client.PinCode);

	cout << "\nEnter Name? ";
	getline(cin, Client.ClientName);

	cout << "\nEnter Phone? ";
	getline(cin, Client.Phone);

	cout << "\nEnter Account Balance? ";
	cin >> Client.AccountBalance;

	return Client;
}

void AddDataLineToFile(string FileName, string strDataLine)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out | ios::app);

	if (MyFile.is_open())
	{
		MyFile << strDataLine << endl;

		MyFile.close();
	}
}

void AddNewClient()
{
	stClient Client;
	Client = ReadNewClient();
	AddDataLineToFile(ClientstFileName, ConvertRecordToLine(Client));
}

void AddNewClients()
{
	char AddMore = 'Y';

	do
	{
		system("cls");
		cout << "Adding New Client: \n\n";

		AddNewClient();
		cout << "\nClient Added successfully, Do You want to add more clients ? Y / N ";
		cin >> AddMore;

	} while (toupper(AddMore) == 'Y');
}

bool FindClientByAccountNumber(string AccountNumber, vector <stClient>& vClients, stClient& Client)
{
	for (stClient C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			Client = C;
			return true;
		}
	}
	return false;
}

void PrintClientCard(stClient Client)
{
	cout << "\nThe following are the client details:\n";
	cout << string(40, '-') << endl;
	cout << "Accout Number: " << Client.AccountNumber;
	cout << "\nPin Code : " << Client.PinCode;
	cout << "\nName : " << Client.ClientName;
	cout << "\nPhone : " << Client.Phone;
	cout << "\nAccount Balance: " << Client.AccountBalance << endl;
	cout << string(40, '-') << endl << endl;
}

bool MarkClientForDeleteByAccountNumber(string AccountNumber, vector <stClient>& vClients)
{
	for (stClient& C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			C.MarkForDelete = true;
			return true;
		}
	}
	return false;

}

vector <stClient> SaveClientsDataToFile(string FileName, vector<stClient> vClients) {

	fstream MyFile;
	MyFile.open(FileName, ios::out); //overwrite

	string DataLine;
	if (MyFile.is_open()) {

		for (stClient C : vClients)
		{
			if (C.MarkForDelete == false)
			{
				//we only write record that are not marked for delete.

				DataLine = ConvertRecordToLine(C);
				MyFile << DataLine << endl;
			}
		}
		MyFile.close();
	}
	return vClients;
}

bool DeleteClientByAccountNumber(string AccountNumber, vector <stClient>& vClients)
{
	stClient Client;
	char Answer = 'n';

	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		PrintClientCard(Client);

		cout << "\n\n Are you sure you want to delete this client? y/n ? ";
		cin >> Answer;

		if (Answer == 'y' || Answer == 'Y')
		{
			MarkClientForDeleteByAccountNumber(AccountNumber, vClients);
			SaveClientsDataToFile(ClientstFileName, vClients);

			//Refresh Clients
			vClients = LoadClientDataFromFile(ClientstFileName);

			cout << "\n\nClient deleted successfully.\n";

			return true;
		}

	}
	else
	{
		cout << "\n Client with Account Number (" << AccountNumber << ") is Not Found." << endl;
		return false;
	}
}

string ReadClientAccountNumber()
{
	string AccountNumber = "";

	cout << "\nPlease Enter Account Number? ";
	getline(cin >> ws, AccountNumber);

	return AccountNumber;
}

stClient ChangeClientRecord(string AccountNumber)
{
	stClient Client;

	Client.AccountNumber = AccountNumber;
	//Usage of std::ws will extract all the whitespace character
	cout << "\nEnter PinCode? ";
	getline(cin >> ws, Client.PinCode);
	cout << "\nEnter Name? ";
	getline(cin, Client.ClientName);
	cout << "\nEnter Phone? ";
	getline(cin, Client.Phone);
	cout << "\nEnter Account Balance? ";
	cin >> Client.AccountBalance;

	return Client;
}

bool UpdateClientByAccountNumber(string AccountNumber, vector <stClient>& vClients)
{
	stClient Client;
	char Answer = 'n';

	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		PrintClientCard(Client);

		cout << "\n\n Are you sure you want to update this client? y/n ? ";
		cin >> Answer;

		if (Answer == 'y' || Answer == 'Y')
		{

			for (stClient& C : vClients)
			{
				if (C.AccountNumber == AccountNumber)
				{
					C = ChangeClientRecord(AccountNumber);
					break;
				}
			}


			SaveClientsDataToFile(ClientstFileName, vClients);

			cout << "\n\nClient updated successfully.\n";

			return true;
		}


	}
	else
	{
		cout << "\n Client with Account Number (" << AccountNumber << ") is Not Found." << endl;
		return false;
	}
}

enum enTransactionsMenuOptions
{
	eDeposit = 1, eWithdraw = 2,
	eShowTotalBalances = 3, eShowMainMenu = 4
};

short ReadTransactionsMenuOptions()
{
	cout << "Choose what do you want to do? [1 to 4]? ";
	short Choice = 0;
	cin >> Choice;

	return Choice;
}

bool DepositBalanceToClientByAccountNumber(string AccountNumber, double Amount, vector <stClient>& vClients)
{
	char Answer = 'n';

	cout << "\n\n Are you sure you want to perform this Transaction? y/n ? ";
	cin >> Answer;

	if (Answer == 'y' || Answer == 'Y')
	{
		for (stClient& C : vClients)
		{
			if (C.AccountNumber == AccountNumber)
			{
				C.AccountBalance += Amount;
				SaveClientsDataToFile(ClientstFileName, vClients);
				cout << "\n\nDone Successfully. New Balance is: " << C.AccountBalance;
				return true;
			}
			break;
		}

	}


	return false;
}

short ReadWithdrawAmount()
{
	cout << "Please enter Withdraw Amount? ";
	short Amount = 0;
	cin >> Amount;

	return Amount;
}

void ShowDepositScreen()
{
	system("cls");
	cout << string(40, '-') << endl;
	cout << "\tDeposit Screen \n";
	cout << string(40, '-') << endl << endl;

	vector <stClient> vClients = LoadClientDataFromFile(ClientstFileName);
	string AccountNumber = ReadClientAccountNumber();
	stClient Client;
	double Amount = 0;

	while (!FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		cout << "\n Client with Account Number (" << AccountNumber << ") does not exist." << endl;
		AccountNumber = ReadClientAccountNumber();
	};

	PrintClientCard(Client);

	cout << "\nPlease enter Deposit Amount? ";
	cin >> Amount;

	DepositBalanceToClientByAccountNumber(AccountNumber, Amount, vClients);

}

void ShowWithdrawScreen()
{
	system("cls");
	cout << string(40, '-') << endl;
	cout << "\tDeposit Screen \n";
	cout << string(40, '-') << endl << endl;

	vector <stClient> vClients = LoadClientDataFromFile(ClientstFileName);
	string AccountNumber = ReadClientAccountNumber();
	stClient Client;
	double Amount = 0;

	while (!FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		cout << "\n Client with Account Number (" << AccountNumber << ") does not exist." << endl;
		AccountNumber = ReadClientAccountNumber();
	};

	PrintClientCard(Client);

	cout << "\nPlease enter Withdraw Amount? ";
	cin >> Amount;


	//Validate that the amount does not exceeds the balance

	while (Amount > Client.AccountBalance)
	{
		cout << "\n Amount Exceeds the balance, you can withdraw up to : " << Client.AccountBalance << endl;
		cout << "Please enter another Amount? ";
		cin >> Amount;
	}

	DepositBalanceToClientByAccountNumber(AccountNumber, Amount * -1, vClients);

}

void PrintClientBalaneRecordLine(stClient Client)
{
	cout << " | " << setw(16) << left << Client.AccountNumber
		<< " | " << setw(26) << left << Client.ClientName
		<< " | " << setw(16) << left << Client.AccountBalance
		<< endl;
}

void ShowTotalBalances()
{
	vector<stClient>  vClient = LoadClientDataFromFile(ClientstFileName);
	double TotalBalances = 0;

	system("cls");
	cout << "\n\t\t\t\tBlances List (" << vClient.size() << ") Client(s).\n";
	cout << string(62, '_') << endl << endl;

	// Define column titles
	cout << " | " << setw(16) << left << "Account Number"
		<< " | " << setw(26) << left << "Client Name"
		<< " | " << setw(16) << left << "Balance"
		<< endl;

	// Print a separator line
	cout << string(62, '_') << endl << endl;

	if (vClient.size() == 0)
		cout << "\t\t\t\tNo Client Available in the System!\n";
	else

		for (stClient Client : vClient)
		{
			PrintClientBalaneRecordLine(Client);
			TotalBalances += Client.AccountBalance;
		}

	cout << string(62, '_') << endl << endl;

	cout << "\t\t\t\tTotal Balances = " << TotalBalances << endl;
}

void ShowTotalBalancesScreen()
{
	ShowTotalBalances();
}

void PerformTransactionsMenuOptions(enTransactionsMenuOptions TransactionsMenuOptions)
{
	switch (TransactionsMenuOptions)
	{
	case eDeposit:
		system("cls");
		ShowDepositScreen();
		GoBackToTransactionsMenu();
		break;
	case eWithdraw:
		system("cls");
		ShowWithdrawScreen();
		GoBackToTransactionsMenu();
		break;
	case eShowTotalBalances:
		system("cls");
		ShowTotalBalancesScreen();
		GoBackToTransactionsMenu();
		break;
	case eShowMainMenu:
		system("cls");
		ShowMainMenu();
		GoBackToTransactionsMenu();
		break;
	}
}

void ShowTransactionsScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::pTransactions))
	{
		ShowAccessDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	system("cls");
	cout << "==============================================\n";
	cout << "\t\Transactions Menu Screen\n";
	cout << "==============================================\n";
	cout << "\t[1] Deposit.\n";
	cout << "\t[2] Withdraw.\n";
	cout << "\t[3] Total Balances.\n";
	cout << "\t[4] Main Menu.\n";
	cout << "==============================================\n";
	PerformTransactionsMenuOptions((enTransactionsMenuOptions)ReadTransactionsMenuOptions());
}

void ShowEndScreen()
{
	system("cls");
	cout << string(40, '-') << endl;
	cout << "\tProgram Ends :-) \n";
	cout << string(40, '-') << endl << endl;
}

void ShowFindClientScreen()
{

	if (!CheckAccessPermission(enMainMenuPermissions::pFindClient))
	{
		ShowAccessDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	system("cls");

	cout << string(40, '-') << endl;
	cout << "\tFind Client Screen \n";
	cout << string(40, '-') << endl << endl;

	vector <stClient> vClients = LoadClientDataFromFile(ClientstFileName);
	string AccountNumber = ReadClientAccountNumber();
	stClient Client;

	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		PrintClientCard(Client);
	}
	else
	{
		cout << "\n Client with Account Number (" << AccountNumber << ") is Not Found." << endl;
	}
}

void ShowUpdateClientScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::pUpdateClients))
	{
		ShowAccessDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	system("cls");

	cout << string(40, '-') << endl;
	cout << "\tUpdate Client Info Screen \n";
	cout << string(40, '-') << endl << endl;

	vector <stClient> vClients = LoadClientDataFromFile(ClientstFileName);
	string AccountNumber = ReadClientAccountNumber();
	UpdateClientByAccountNumber(AccountNumber, vClients);
}

void ShowDeleteClientScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::pDeleteClient))
	{
		ShowAccessDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	system("cls");

	cout << string(40, '-') << endl;
	cout << "\tDelete Client screen \n";
	cout << string(40, '-') << endl << endl;

	vector <stClient> vClients = LoadClientDataFromFile(ClientstFileName);
	string AccountNumber = ReadClientAccountNumber();
	DeleteClientByAccountNumber(AccountNumber, vClients);
}

void ShowAddNewClientsScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::pAddNewClient))
	{
		ShowAccessDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	system("cls");

	cout << string(40, '-') << endl;
	cout << "\tAdd New Clients screen \n";
	cout << string(40, '-') << endl << endl;

	AddNewClient();
}

void PrintClientRecordLine(stClient Client)
{
	cout << " | " << setw(16) << left << Client.AccountNumber
		<< " | " << setw(14) << left << Client.PinCode
		<< " | " << setw(26) << left << Client.ClientName
		<< " | " << setw(16) << left << Client.Phone
		<< " | " << setw(16) << left << Client.AccountBalance
		<< endl;
}

bool CheckAccessPermission(enMainMenuPermissions Permission)
{
	if (CurrentUser.Permissions == enMainMenuPermissions::eAll)
		return true;

	if ((Permission & CurrentUser.Permissions) == Permission)
		return true;
	else
		return false;
}

void ShowAllClientsScreen()
{

	if (!CheckAccessPermission(enMainMenuPermissions::pListClients))
	{
		ShowAccessDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	vector<stClient>  vClient = LoadClientDataFromFile(ClientstFileName);

	system("cls");
	cout << "\n\t\t\t\tClient List (" << vClient.size() << ") Client(s).\n";
	cout << string(96, '_') << endl << endl;

	// Define column titles
	cout << " | " << setw(16) << left << "Account Number"
		<< " | " << setw(14) << left << "Pin Code"
		<< " | " << setw(26) << left << "Client Name"
		<< " | " << setw(16) << left << "Phone"
		<< " | " << setw(16) << left << "Balance"
		<< endl;

	// Print a separator line
	cout << string(96, '_') << endl << endl;

	if (vClient.size() == 0)
		cout << "\t\t\t\tNo Client Available in the System!\n";
	else

		for (stClient Client : vClient)
		{
			PrintClientRecordLine(Client);

		}

	cout << string(96, '_') << endl << endl;
}

void GoBackToMainMenu()
{
	cout << "\n\nPress any key to go back to Main Menu ...";
	system("pause>0");
	ShowMainMenu();
}

short ReadMainMenuOptions()
{
	cout << "Choose what do you want to do? [1 to 8]? ";
	short Choice = 0;
	cin >> Choice;

	return Choice;
}

void GoBackToTransactionsMenu()
{
	cout << "\n\nPress any key to go back to Transactions Menu ...";
	system("pause>0");
	ShowTransactionsScreen();
}

void PerformMainMenuOptions(enMainMenuOptions MainMenuOption)
{
	switch (MainMenuOption)
	{
	case eListClient:
		system("cls");
		ShowAllClientsScreen();
		GoBackToMainMenu();
		break;
	case eAddNewClient:
		system("cls");
		ShowAddNewClientsScreen();
		GoBackToMainMenu();
		break;
	case eDeleteClient:
		system("cls");
		ShowDeleteClientScreen();
		GoBackToMainMenu();
		break;
	case eUpdateClient:
		system("cls");
		ShowUpdateClientScreen();
		GoBackToMainMenu();
		break;
	case eFindClient:
		system("cls");
		ShowFindClientScreen();
		GoBackToMainMenu();
		break;
	case eShowTransactionsMenu:
		system("cls");
		ShowTransactionsScreen();
		break;
	case eManageUsers:
		system("cls");
		ShowManageUsersScreen();
		break;
	case eExit:
		system("cls");
		//ShowEndScreen();
		Login();

		break;
	}
}

void ShowMainMenu()
{
	system("cls");
	cout << "==============================================\n";
	cout << "\t\tMain Menu Screen\n";
	cout << "==============================================\n";
	cout << "\t[1] Show Client List.\n";
	cout << "\t[2] Add New Client.\n";
	cout << "\t[3] Delete Client.\n";
	cout << "\t[4] Update Client.\n";
	cout << "\t[5] Find Client.\n";
	cout << "\t[6] Transactions.\n";
	cout << "\t[7] Manage Users.\n";
	cout << "\t[8] Logout.\n";
	cout << "==============================================\n";
	PerformMainMenuOptions((enMainMenuOptions)ReadMainMenuOptions());

}

stUser ReadUserLogin()
{
	stUser User;
	cout << "\nEnter Username? ";
	cin >> User.Username;
	cout << "\nEnter Password? ";
	cin >> User.Password;
	return User;
}

stUser ConvertUserLineToRecord(string Line, string Seperator = "#//#")
{
	stUser User;
	vector <string> vUsersData;

	vUsersData = SplitString(Line, Seperator);
	User.Username = vUsersData[0];
	User.Password = vUsersData[1];
	User.Permissions = stoi(vUsersData[2]);

	return User;
}

vector <stUser> LoadUserDataFromFile(string FileName)
{
	vector <stUser> vUsers;

	fstream MyFile;
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string line;
		stUser User;

		while (getline(MyFile, line))
		{
			User = ConvertUserLineToRecord(line);
			vUsers.push_back(User);
		}
		MyFile.close();
	}
	return vUsers;
}

short ReadManageUsersMenuOptions()
{
	cout << "Choose what do you want to do? [1 to 6]? ";
	short Choice = 0;
	cin >> Choice;

	return Choice;
}

enum enManageUsersMenuOptions {
	eListUsers = 1, eAddNewUser = 2,
	eDeleteUser = 3, eUpdateUser = 4,
	eFindUser = 5, eShowMainMenuFromManageUserScreen = 6
};

void GoBackToManageUsersMenu()
{
	cout << "\n\nPress any key to go back to Manage Users Menu ...";
	system("pause>0");
	ShowManageUsersScreen();
}

void PrintUserRecordLine(stUser User)
{
	cout << " | " << setw(16) << left << User.Username
		<< " | " << setw(14) << left << User.Password
		<< " | " << setw(26) << left << User.Permissions
		<< endl;
}

void ShowAllUsersScreen()
{
	vector<stUser>  vUsers = LoadUserDataFromFile(UserstFileName);

	system("cls");
	cout << "\n\t\t\t\t Users List (" << vUsers.size() << ") User(s).\n";
	cout << string(96, '_') << endl << endl;

	// Define column titles
	cout << " | " << setw(16) << left << "User Name"
		<< " | " << setw(14) << left << "Password"
		<< " | " << setw(26) << left << "Permissions"
		<< endl;

	// Print a separator line
	cout << string(96, '_') << endl << endl;

	if (vUsers.size() == 0)
		cout << "\t\t\t\tNo User Available in the System!\n";
	else

		for (stUser User : vUsers)
		{
			PrintUserRecordLine(User);

		}

	cout << string(96, '_') << endl << endl;
}

void ShowListUsersScreen()
{
	ShowAllUsersScreen();
}

bool UserExistsByUsername(string Username, string FileName)
{
	vector <stUser> vUsers;

	fstream MyFile;

	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string line;
		stUser User;

		while (getline(MyFile, line))
		{
			User = ConvertUserLineToRecord(line);

			if (User.Username == Username)
			{
				MyFile.close();
				return true;
			}
			vUsers.push_back(User);
		}
		MyFile.close();
	}
	return false;
}

int ReadPermissionsToSet()
{
	int Permissions = 0;

	char GiveAccess = 'n';

	cout << "\nDo you want to give full access? y/n? ";
	cin >> GiveAccess;

	if (GiveAccess == 'y' || GiveAccess == 'Y')
	{
		return enMainMenuPermissions::eAll;
	}

	cout << "\nShow Client List? y/n? ";
	cin >> GiveAccess;
	if (GiveAccess == 'y' || GiveAccess == 'Y')
	{
		Permissions += enMainMenuPermissions::pListClients;
	}
	cout << "\nAdd New Client? y/n? ";
	cin >> GiveAccess;
	if (GiveAccess == 'y' || GiveAccess == 'Y')
	{
		Permissions += enMainMenuPermissions::pAddNewClient;
	}
	cout << "\nDelete Client? y/n? ";
	cin >> GiveAccess;
	if (GiveAccess == 'y' || GiveAccess == 'Y')
	{
		Permissions += enMainMenuPermissions::pDeleteClient;
	}
	cout << "\nUpdate Client Info? y/n? ";
	cin >> GiveAccess;
	if (GiveAccess == 'y' || GiveAccess == 'Y')
	{
		Permissions += enMainMenuPermissions::pUpdateClients;
	}
	cout << "\nFind Client? y/n? ";
	cin >> GiveAccess;
	if (GiveAccess == 'y' || GiveAccess == 'Y')
	{
		Permissions += enMainMenuPermissions::pFindClient;
	}
	cout << "\nTransactions? y/n? ";
	cin >> GiveAccess;
	if (GiveAccess == 'y' || GiveAccess == 'Y')
	{
		Permissions += enMainMenuPermissions::pTransactions;
	}
	cout << "\nManage Users? y/n? ";
	cin >> GiveAccess;
	if (GiveAccess == 'y' || GiveAccess == 'Y')
	{
		Permissions += enMainMenuPermissions::pManageUsers;
	}

	return Permissions;
}

stUser ReadNewUser()
{
	stUser User;

	cout << "\nEnter Username? ";

	//Usage of std::ws will extract all the whitespace character
	getline(cin >> ws, User.Username);

	while (UserExistsByUsername(User.Username, UserstFileName))
	{
		cout << "\nUser with [" << User.Username << "] already exists, Enter another Username? ";
		getline(cin >> ws, User.Username);
	}

	cout << "\nEnter Password? ";
	getline(cin >> ws, User.Password);


	User.Permissions = ReadPermissionsToSet();


	return User;
}

string ConvertUserRecordToLine(stUser User, string Seperator = "#//#")
{
	string stUserRecord = "";

	stUserRecord += User.Username + Seperator;
	stUserRecord += User.Password + Seperator;
	stUserRecord += to_string(User.Permissions);

	return stUserRecord;

}

void AddNewUser()
{
	stUser User;
	User = ReadNewUser();
	AddDataLineToFile(UserstFileName, ConvertUserRecordToLine(User));
}

void AddNewUsers()
{
	char AddMore = 'Y';

	do
	{
		//system("cls");
		cout << "Adding New User: \n\n";

		AddNewUser();
		cout << "\nUser Added successfully, Do You want to add more Users ? Y / N ";
		cin >> AddMore;

	} while (toupper(AddMore) == 'Y');
}

void ShowAddNewUsersScreen()
{
	system("cls");

	cout << string(40, '-') << endl;
	cout << "\tAdd New Users screen \n";
	cout << string(40, '-') << endl << endl;

	AddNewUsers();
}

bool FindUserByUsername(string Username, vector <stUser> vUsers, stUser& User)
{
	for (stUser U : vUsers)
	{
		if (U.Username == Username)
		{
			User = U;
			return true;
		}
	}
	return false;
}

bool FindUserByUsernameAndPassword(string Username, string Password, stUser& User)
{
	vector <stUser> vUsers = LoadUserDataFromFile(UserstFileName);

	for (stUser U : vUsers)
	{
		if (U.Username == Username && U.Password == Password)
		{
			User = U;
			return true;
		}
	}
	return false;
}

bool MarkUserForDeleteByUsername(string Username, vector <stUser>& vUsers)
{
	for (stUser& U : vUsers)
	{
		if (U.Username == Username)
		{
			U.MarkForDelete = true;
			return true;
		}
	}
	return false;

}

vector <stUser> SaveUsersDataToFile(string FileName, vector<stUser> vUsers) {

	fstream MyFile;
	MyFile.open(FileName, ios::out); //overwrite

	string DataLine;
	if (MyFile.is_open()) {

		for (stUser U : vUsers)
		{
			if (U.MarkForDelete == false)
			{
				//we only write record that are not marked for delete.

				DataLine = ConvertUserRecordToLine(U);
				MyFile << DataLine << endl;
			}
		}
		MyFile.close();
	}
	return vUsers;
}

void PrintUserCard(stUser User)
{
	cout << "\nThe following are the User details:\n";
	cout << string(40, '-') << endl;
	cout << "Username: " << User.Username;
	cout << "\nPassword : " << User.Password;
	cout << "\nPermissions : " << User.Permissions << endl;
	cout << string(40, '-') << endl << endl;
}

bool DeleteUserByUsername(string Username, vector <stUser>& vUsers)
{
	stUser User;
	char Answer = 'n';

	if (FindUserByUsername(Username, vUsers, User))
	{
		if (Username == "Admin")
		{
			cout << "\n\n You cannot Delete this user.\n";
			return false;
		}
		PrintUserCard(User);

		cout << "\n\n Are you sure you want to delete this User? y/n ? ";
		cin >> Answer;

		if (Answer == 'y' || Answer == 'Y')
		{
			MarkUserForDeleteByUsername(Username, vUsers);
			SaveUsersDataToFile(UserstFileName, vUsers);

			//Refresh Clients
			vUsers = LoadUserDataFromFile(UserstFileName);

			cout << "\n\nUser deleted successfully.\n";

			return true;
		}

	}
	else
	{
		cout << "\n User with Username (" << Username << ") is Not Found." << endl;
		return false;
	}
}

string ReadUsername()
{
	string Username = "";

	cout << "\nPlease Enter Username? ";
	getline(cin >> ws, Username);

	return Username;
}

void ShowDeleteUserScreen()
{
	system("cls");

	cout << string(40, '-') << endl;
	cout << "\tDelete User screen \n";
	cout << string(40, '-') << endl << endl;

	vector <stUser> vUsers = LoadUserDataFromFile(UserstFileName);
	string Username = ReadUsername();
	DeleteUserByUsername(Username, vUsers);
}

stUser ChangeUserRecord(string Username)
{
	stUser User;

	User.Username = Username;

	//Usage of std::ws will extract all the whitespace character
	cout << "\nEnter Password? ";
	getline(cin >> ws, User.Password);

	User.Permissions = ReadPermissionsToSet();

	return User;
}

bool UpdateUserByUsername(string Username, vector <stUser>& vUsers)
{
	stUser User;
	char Answer = 'n';

	if (FindUserByUsername(Username, vUsers, User))
	{
		PrintUserCard(User);

		cout << "\n\n Are you sure you want to update this User? y/n ? ";
		cin >> Answer;

		if (Answer == 'y' || Answer == 'Y')
		{

			for (stUser& U : vUsers)
			{
				if (U.Username == Username)
				{
					U = ChangeUserRecord(Username);
					break;
				}
			}


			SaveUsersDataToFile(UserstFileName, vUsers);

			cout << "\n\nUser updated successfully.\n";

			return true;
		}


	}
	else
	{
		cout << "\n User with Username (" << Username << ") is Not Found." << endl;
		return false;
	}
}

void ShowUpdateUserScreen()
{
	system("cls");

	cout << string(40, '-') << endl;
	cout << "\tUpdate Client Info Screen \n";
	cout << string(40, '-') << endl << endl;

	vector <stUser> vUsers = LoadUserDataFromFile(UserstFileName);
	string Username = ReadUsername();
	UpdateUserByUsername(Username, vUsers);
}

void ShowFindUserScreen()
{
	system("cls");

	cout << string(40, '-') << endl;
	cout << "\tFind User Screen \n";
	cout << string(40, '-') << endl << endl;

	vector <stUser> vUsers = LoadUserDataFromFile(UserstFileName);
	string Username = ReadUsername();

	stUser User;

	if (FindUserByUsername(Username, vUsers, User))
	{
		PrintUserCard(User);
	}
	else
	{
		cout << "\n User with Username (" << Username << ") is Not Found." << endl;
	}
}

void ShowAccessDeniedMessage()
{
	system("cls");

	cout << string(40, '-') << endl;
	cout << " Access Denied, \n";
	cout << " You don't have permission to do this, \n";
	cout << " Please Contact your Admin. \n";

	cout << string(40, '-') << endl << endl;
}

void PerformManageUsersMenuOptions(enManageUsersMenuOptions ManageUsersMenuOptions)
{
	switch (ManageUsersMenuOptions)
	{
	case eListUsers:
		system("cls");
		ShowListUsersScreen();
		GoBackToManageUsersMenu();
		break;
	case eAddNewUser:
		system("cls");
		ShowAddNewUsersScreen();
		GoBackToManageUsersMenu();
		break;
	case eDeleteUser:
		system("cls");
		ShowDeleteUserScreen();
		GoBackToManageUsersMenu();
		break;
	case eUpdateUser:
		system("cls");
		ShowUpdateUserScreen();
		GoBackToManageUsersMenu();
		break;
	case eFindUser:
		system("cls");
		ShowFindUserScreen();
		GoBackToManageUsersMenu();
		break;
	case eShowMainMenuFromManageUserScreen:
		system("cls");
		ShowMainMenu();
		break;
	}
}

void ShowManageUsersScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::pManageUsers))
	{
		ShowAccessDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	system("cls");
	cout << "==============================================\n";
	cout << "\t\tManage Users Menu Screen\n";
	cout << "==============================================\n";
	cout << "\t[1] List Users.\n";
	cout << "\t[2] Add New User.\n";
	cout << "\t[3] Delete User.\n";
	cout << "\t[4] Update User.\n";
	cout << "\t[5] Find User.\n";
	cout << "\t[6] Main Menu.\n";
	cout << "==============================================\n";
	PerformManageUsersMenuOptions((enManageUsersMenuOptions)ReadManageUsersMenuOptions());
}

bool LoadUserInfo(string Username, string Password)
{
	if (FindUserByUsernameAndPassword(Username, Password, CurrentUser))
		return true;
	else
		return false;
}

void Login()
{
	bool LoginFaild = false;

	string Username, Password;

	do
	{
		system("cls");
		cout << "==============================================\n";
		cout << "\t\tLogin Screen\n";
		cout << "==============================================\n";

		if (LoginFaild)
		{
			cout << "Invalid Username/Password!" << endl;
		}
		cout << "\nEnter Username? ";
		cin >> Username;
		cout << "\nEnter Password? ";
		cin >> Password;

		LoginFaild = !LoadUserInfo(Username, Password);

	} while (LoginFaild);

	ShowMainMenu();

}

int main()
{
	Login();

	system("pause>0");

	return 0;
}