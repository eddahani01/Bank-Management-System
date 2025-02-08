

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>
#include <iomanip>
#include <algorithm>

using namespace std;

const string ClientstFileName = "ClientsData.txt";

void ShowMainMenu();

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
    MyFile.open(FileName,ios::in);

    if (MyFile.is_open())
    {
        string line;
        stClient Client;
         
        while (getline(MyFile,line))
        {
            Client = ConvertLineToRecord(line);
            vClients.push_back(Client);
        }
        MyFile.close();
    }
    return vClients;
}

enum enMainMenuOptions {
    eListClient = 1, eAddNewClient = 2,
    eDeleteClient = 3, eUpdateClient = 4,
    eFindClient = 5, eExit = 6
};

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
            if (Client.AccountNumber ==  AccountNumber)
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
    MyFile.open(FileName,ios::out | ios::app);

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
    AddDataLineToFile(ClientstFileName,ConvertRecordToLine(Client));
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
       
    } while (toupper(AddMore) =='Y');
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

void ShowEndScreen()
{
    system("cls");
    cout << string(40, '-') << endl;
    cout << "\Program Ends :-) \n";
    cout << string(40, '-') << endl << endl;
}

void ShowFindClientScreen()
{
    system("cls");

    cout << string(40, '-') << endl;
    cout << "\Find Client Screen \n";
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
    system("cls");

    cout << string(40, '-') << endl;
    cout << "\tDelete Client screen \n";
    cout << string(40, '-') << endl << endl;

    vector <stClient> vClients = LoadClientDataFromFile(ClientstFileName);
    string AccountNumber = ReadClientAccountNumber();
    DeleteClientByAccountNumber(AccountNumber,vClients);
}

void ShowAddNewClientsScreen()
{
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

void ShowAllClientsScreen()
{
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

    if (vClient.size()==0)
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
    cout << "Choose what do you want to do? [1 to 6]?";
    short Choice = 0;
    cin >> Choice;

    return Choice;
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
    case eExit:
        system("cls");
        ShowEndScreen();
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
    cout << "\t[6] Exit.\n";
    cout << "==============================================\n";
    PerformMainMenuOptions((enMainMenuOptions)ReadMainMenuOptions());

}

int main()
{
    ShowMainMenu();
    system("pause>0");

    return 0;
}