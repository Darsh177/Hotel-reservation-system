#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iomanip>

using namespace std;

const int DAYS = 7;
const int PERIODS = 3;
const int bookings_count = 100;

struct Booking {
    int bookingID;
    int userID;
    int roomID;
    int startday;
    int endday;
};

struct Room {
    int roomID;
    string roomtype;
    bool availability[DAYS][PERIODS];
    string location;
    float price;
};

struct date {
    int day;
    int Periods;
};

struct User {
    int userID;
    string userType;
    string username;
    string password;
    Booking bookings[bookings_count];
};

// --- Function Prototypes ---
bool Register(vector<User>& users);
bool login(vector<User>& users, vector<Room>& rooms);
bool findID(int IDnum, vector<User> users);
bool find(string username, string password, vector<User> users);
int menu();

void add_room(vector<Room>& rooms);
void readRooms(vector<Room>& rooms);
void writeRooms(vector<Room> rooms);
void readUsers(vector<User>& users);
void writeUsers(vector<User> users);
void adminMenu(vector<Room>& rooms, vector<User>& users);
void manageRooms(vector<Room>& rooms, vector<User> users);
bool findRoom(int roomID, vector<Room> rooms);
void manageRoomsMenu(vector<Room> rooms);
void editRoomMenu();
void editRoom(Room& room, vector<Room>& rooms, vector<User> users);
string replaceChar(string str, char find, char with);
void promoteToAdmin(vector<User>& users);

// Updated Booking Prototypes
void book_room(vector<Room>& rooms); // Needs access to rooms vector
void view_booking();
void modify(vector<Room>& rooms); // Needs access to rooms vector
string getDay(int indexOfDay);
string getTime(int indexOfTime);
void showAvailableRooms(vector<Room>& rooms);
void displayAllRoomsAvailability(const vector<Room>& rooms);

// Global Temporary Variables
Room temp_room;
date temp_date_start, temp_date_end;
User temp_user;

// --- Main Function ---
int main() {
    int choice;
    char answer;
    vector<Room> rooms;
    vector<User> users;

    readRooms(rooms);
    readUsers(users);

    do {
        choice = menu();
        switch (choice) {
        case 1:
            Register(users);
            writeUsers(users);
            break;
        case 2:
            if (login(users, rooms)) {
                writeUsers(users);
                writeRooms(rooms);
            }
            break;
        default:
            cout << "Invalid choice.\n";
            continue;
        }
        cout << "\nDo you want to continue? (Y/N): ";
        cin >> answer;
    } while (answer == 'Y' || answer == 'y');

    writeRooms(rooms);
    writeUsers(users);
    return 0;
}

// --- Logic to Update Availability ---

void confirmBookingInSystem(vector<Room>& rooms) {
    for (int i = 0; i < rooms.size(); i++) {
        if (rooms[i].roomID == temp_room.roomID) {
            // Update the availability from start day/period to end day/period
            for (int d = temp_date_start.day; d <= temp_date_end.day; d++) {
                int startP = (d == temp_date_start.day) ? temp_date_start.Periods : 1;
                int endP = (d == temp_date_end.day) ? temp_date_end.Periods : 3;

                for (int p = startP; p <= endP; p++) {
                    rooms[i].availability[d - 1][p - 1] = false; // Booked!
                }
            }
            cout << "\n[Success] Room " << temp_room.roomID << " schedule has been updated.\n";
            return;
        }
    }
}

// --- User & Booking Functions ---

void book_room(vector<Room>& rooms) {
    cout << "\n--- Booking System ---\n";
    cout << "Enter Room ID: "; cin >> temp_room.roomID;

    if (!findRoom(temp_room.roomID, rooms)) {
        cout << "Room ID not found!\n";
        return;
    }

    cout << "Arrival Day (1-Sunday to 7-Saturday): "; cin >> temp_date_start.day;
    cout << "Start Period (1-Morning, 2-Afternoon, 3-Evening): "; cin >> temp_date_start.Periods;
    cout << "End Day (1-7): "; cin >> temp_date_end.day;
    cout << "End Period (1-3): "; cin >> temp_date_end.Periods;

    if (temp_date_start.day > temp_date_end.day || (temp_date_start.day == temp_date_end.day && temp_date_start.Periods > temp_date_end.Periods)) {
        cout << "Invalid time selection.\n";
        return;
    }

    cout << "Confirm Room Type: "; cin >> temp_room.roomtype;
    view_booking();
    modify(rooms);
}

void modify(vector<Room>& rooms) {
    int choice;
    cout << "\n1. Re-enter Details\n2. Confirm & Finish\nChoice: ";
    cin >> choice;
    if (choice == 1) book_room(rooms);
    else if (choice == 2) {
        confirmBookingInSystem(rooms);
        cout << "Reservation complete!\n";
    }
}

bool login(vector<User>& users, vector<Room>& rooms) {
    string uname, pass;
    cout << "Username: "; cin >> uname;
    cout << "Password: "; cin >> pass;

    for (int i = 0; i < users.size(); i++) {
        if (users[i].username == uname && users[i].password == pass) {
            temp_user = users[i];
            cout << "\nWelcome " << uname << endl;
            if (users[i].userType == "admin") {
                adminMenu(rooms, users);
            }
            else {
                int userChoice;
                while (true) {
                    cout << "\n1. List Available Rooms\n2. View Full Schedule Grid\n3. Book a Room\n4. Logout\nChoice: ";
                    cin >> userChoice;
                    if (userChoice == 1) showAvailableRooms(rooms);
                    else if (userChoice == 2) displayAllRoomsAvailability(rooms);
                    else if (userChoice == 3) book_room(rooms);
                    else if (userChoice == 4) break;
                }
            }
            return true;
        }
    }
    return false;
}

// --- Helper & Admin Functions (Combined) ---

void displayAllRoomsAvailability(const vector<Room>& rooms) {
    cout << "\nLegend: [A] = Available | [U] = Booked\n";
    for (const auto& r : rooms) {
        cout << "\nRoom ID: " << r.roomID << " (" << r.roomtype << ")\n";
        cout << "           | P1 | P2 | P3 |\n";
        for (int d = 0; d < DAYS; d++) {
            cout << setw(10) << getDay(d + 1) << " | ";
            for (int p = 0; p < PERIODS; p++) {
                cout << (r.availability[d][p] ? "[A]" : "[U]") << " | ";
            }
            cout << endl;
        }
    }
}

string getDay(int i) {
    string d[] = { "", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    return (i >= 1 && i <= 7) ? d[i] : "???";
}

string getTime(int i) {
    string t[] = { "", "Morning", "Afternoon", "Evening" };
    return (i >= 1 && i <= 3) ? t[i] : "???";
}

void view_booking() {
    cout << "\nSUMMARY: Room " << temp_room.roomID << " from " << getDay(temp_date_start.day) << " P" << temp_date_start.Periods;
    cout << " to " << getDay(temp_date_end.day) << " P" << temp_date_end.Periods << endl;
}

// --- Basic Utility (Reused from previous) ---
bool Register(vector<User>& users) {
    User t; cout << "Username: "; cin >> t.username; cout << "Password: "; cin >> t.password;
    if (find(t.username, t.password, users)) return false;
    t.userType = (users.empty()) ? "admin" : "user";
    t.userID = rand() % 1000;
    for (int i = 0; i < bookings_count; i++) t.bookings[i].bookingID = 0;
    users.push_back(t);
    return true;
}

void add_room(vector<Room>& rooms) {
    Room t; cout << "ID: "; cin >> t.roomID; cout << "Type: "; cin >> t.roomtype;
    cout << "Price: "; cin >> t.price; cout << "Loc: "; cin.ignore(); getline(cin, t.location);
    for (int i = 0; i < 7; i++) for (int j = 0; j < 3; j++) t.availability[i][j] = true;
    rooms.push_back(t);
}

void adminMenu(vector<Room>& rooms, vector<User>& users) {
    int c;
    while (true) {
        cout << "\n1. Add Room\n2. Manage\n3. View Schedule Grid\n4. Promote User\n5. Exit\nChoice: "; cin >> c;
        if (c == 1) add_room(rooms);
        else if (c == 2) manageRooms(rooms, users);
        else if (c == 3) displayAllRoomsAvailability(rooms);
        else if (c == 4) promoteToAdmin(users);
        else if (c == 5) break;
    }
}

// --- Rest of standard functions ---
void showAvailableRooms(vector<Room>& rooms) {
    for (const auto& r : rooms) cout << "ID: " << r.roomID << " - " << r.roomtype << " - $" << r.price << endl;
}
string replaceChar(string s, char f, char w) {
    for (int i = 0; i < s.length(); i++) if (s[i] == f) s[i] = w; return s;
}
bool findID(int id, vector<User> v) { for (auto u : v) if (u.userID == id) return true; return false; }
bool find(string u, string p, vector<User> v) { for (auto user : v) if (user.username == u && user.password == p) return true; return false; }
int menu() { int c; cout << "\n1. Register\n2. Login\nChoice: "; cin >> c; return c; }
bool findRoom(int id, vector<Room> v) { for (auto r : v) if (r.roomID == id) return true; return false; }

void readRooms(vector<Room>& rooms) {
    ifstream in("rooms.txt"); if (!in) return; Room r;
    while (in >> r.roomID >> r.roomtype >> r.location >> r.price) {
        r.location = replaceChar(r.location, '_', ' ');
        for (int j = 0; j < 7; j++) for (int k = 0; k < 3; k++) in >> r.availability[j][k];
        rooms.push_back(r);
    } in.close();
}
void writeRooms(vector<Room> rooms) {
    ofstream out("rooms.txt");
    for (auto r : rooms) {
        out << r.roomID << " " << r.roomtype << " " << replaceChar(r.location, ' ', '_') << " " << r.price;
        for (int j = 0; j < 7; j++) for (int k = 0; k < 3; k++) out << " " << r.availability[j][k];
        out << endl;
    } out.close();
}
void readUsers(vector<User>& users) {
    ifstream in("users.txt"); if (!in) return; User u;
    while (in >> u.userID >> u.username >> u.userType >> u.password) {
        u.username = replaceChar(u.username, '_', ' ');
        for (int j = 0; j < 100; j++) in >> u.bookings[j].bookingID >> u.bookings[j].roomID >> u.bookings[j].userID >> u.bookings[j].startday >> u.bookings[j].endday;
        users.push_back(u);
    } in.close();
}
void writeUsers(vector<User> users) {
    ofstream out("users.txt");
    for (auto u : users) {
        out << u.userID << " " << replaceChar(u.username, ' ', '_') << " " << u.userType << " " << u.password << endl;
        for (int j = 0; j < 100; j++) out << u.bookings[j].bookingID << " " << u.bookings[j].roomID << " " << u.bookings[j].userID << " " << u.bookings[j].startday << " " << u.bookings[j].endday << " ";
        out << endl;
    } out.close();
}
void promoteToAdmin(vector<User>& users) {
    int id; cout << "User ID: "; cin >> id;
    for (auto& u : users) if (u.userID == id) { u.userType = "admin"; return; }
}
void manageRooms(vector<Room>& rooms, vector<User> users) {
    manageRoomsMenu(rooms);
}
void manageRoomsMenu(vector<Room> rooms) {
    for (auto r : rooms) cout << r.roomID << "\t" << r.roomtype << endl;
}
void editRoomMenu() {}
void editRoom(Room& room, vector<Room>& rooms, vector<User> users) {}